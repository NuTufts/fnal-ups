import os
import re
import sys
import string

DEFAULT_PYTHON_VERSION = ''
if os.environ.has_key('SETUPS_DIR'):
    DEFAULT_SETUPS_DIR = os.environ['SETUPS_DIR']
else:
    DEFAULT_SETUPS_DIR = '/usr/local/etc'

#############################################################
# User interface:
#  import ups
#  optional:
#       ups.getUps( [setupsDir=/path/to/setups.sh] )
#  
#  ups.setup('my project spec' [, setupsDir=/path/to/setups.sh])
#############################################################
# returns the address of a singleton upsManager object:

singletonUps = None
def getUps(setupsDir=DEFAULT_SETUPS_DIR):
    global singletonUps
    if ( singletonUps is None ):
        singletonUps = upsManager(setupsDir)
    return singletonUps

def setup(arg, setupsDir=DEFAULT_SETUPS_DIR):
    upsmgr = getUps()
    upsmgr.setup(arg)

#############################################################

def set_setupsDir(setupsDir=DEFAULT_SETUPS_DIR):
    global DEFAULT_SETUPS_DIR
    DEFAULT_SETUPS_DIR = setupsDir
#############################################################

# force a reload and exec of the requested version of
# python:
def use_python(pythonVersion=DEFAULT_PYTHON_VERSION):
    if ( pythonVersion != DEFAULT_PYTHON_VERSION ):
        vm = '.*'
    else:
        vm = pythonVersion

    obj = getUps()

    # did we setup python at all?  Or are we getting it by default from the path?
    alreadySetup = ( os.environ.has_key('SETUP_PYTHON') and
                     os.environ['SETUP_PYTHON'] != '' )

    # are we already using the requested version?
    alreadyUsing = ( os.environ.has_key('SETUP_PYTHON') and
                     re.search('python ' + pythonVersion, os.environ['SETUP_PYTHON']) != None )

    if ( alreadyUsing ):
        # already using the requested version, no-op.
        pass
    else:
        if ( alreadySetup ):
            obj._inhaleresults(os.environ['UPS_DIR'] + '/bin/ups unsetup python')
        obj._inhaleresults(os.environ['UPS_DIR'] + '/bin/ups setup python ' + pythonVersion)

        # now exec into the context of the requested version of python:
        # were we running the python interpreter itself?  Special handling required!
        if ( sys.argv == [''] ):
            sys.argv = [ os.environ['PYTHON_DIR'] + '/bin/python' ]
        else:
            sys.argv.insert(0, os.environ['PYTHON_DIR'] + '/bin/python')

        # bye bye, exec into another context:
        os.execve( os.environ['PYTHON_DIR']+'/bin/python', sys.argv, os.environ )

##############################################################################

class upsException(Exception):
    def __init__(self, msg):
        self.args = msg
    
class upsManager:
    def __init__(self, setupsDir=DEFAULT_SETUPS_DIR):

        # initial setup of ups itself:
        os.environ['UPS_SHELL'] = 'sh'
	f = os.popen( '. %s/setups.sh; ' % setupsDir + \
                      'echo os.environ\\[\\"UPS_DIR\\"\\]=\\"$UPS_DIR\\"; ' + \
                      'echo os.environ\\[\\"PRODUCTS\\"\\]=\\"$PRODUCTS\\";' + \
                      'echo os.environ\\[\\"SETUP_UPS\\"\\]=\\"$SETUP_UPS\\"')
	exec f.read()
	f.close()

        # we need to initialize the following so that we can
        #  make the correct changes to sys.path later when products
        #  we setup modify PYTHONPATH
        try:
            self._pythonPath = os.environ['PYTHONPATH']
        except KeyError:
            self._pythonPath = ''
        self._sysPath = sys.path
        (self._internalSysPathPrepend, self._internalSysPathAppend) = self._getInitialSyspathElements()

    ############################################################################
    # set a product up:
    def setup(self, arg):
        try:
            self._inhaleresults(os.environ["UPS_DIR"] + '/bin/ups setup ' + arg)
        except upsException:
            raise
        self._updateImportPath()

    ############################################################################
    # unsetup a product:
    def unsetup(self, arg):
        try:
            self._inhaleresults(os.environ["UPS_DIR"] + '/bin/ups unsetup ' + arg)
        except upsException:
            raise
        self._updateImportPath()

    ############################################################################ 
    # PRIVATE METHODS BELOW THIS POINT.
    #
    def _getInitialSyspathElements(self):
        pyPath = string.split(self._pythonPath, ':')
        sysPath = self._sysPath

        # sysPath always includes '' at the front of the list
        #  (current working directory)
        internalSysPathPrepend = ['',]
        sysPath = sysPath[1:]

        # now, what else is in sysPath that is NOT in PYTHONPATH
        # (and hence must have come from the internals)?
        internalSysPathAppend = []
        for element in sysPath:
            if ( element not in pyPath ):
                internalSysPathAppend.append(element)
        return (internalSysPathPrepend, internalSysPathAppend)
        
    ############################################################################
    def _setPythonPath(self):
        self._pythonPath = os.environ['PYTHONPATH']

    ############################################################################
    def _updateImportPath(self):
        # make sure that sys.path reflects what is in PYTHONPATH after
        # any setups are performed!
        if (os.environ['PYTHONPATH'] != self._pythonPath):
            pypathElements = string.split( os.environ['PYTHONPATH'], ':' )
            # sys.path includes the current working directory as the first element:
            sys.path = self._internalSysPathPrepend
            # now append each pythonpath element
            for element in pypathElements:
                sys.path.append(element)
            # now append whatever was 'internal' originally
            sys.path = sys.path + self._internalSysPathAppend
        # update our reference to PYTHONPATH
        self._setPythonPath()

    ############################################################################
    def _inhaleresults(self, cmd):
	(stdin,stdout,stderr) = os.popen3(cmd)
        filename = stdout.read()
	filename = filename[0:-1]
        if ( filename == "/dev/null" ):
            msg = stderr.read()
            raise upsException(msg)
        stdin.close()
        stdout.close()
        stderr.close()

	setup = open(filename,"a")
	setup.write(""" 
cat <<'EOF' | python
import os
import re
print "--------------cut here-------------"
for v in os.environ.keys():
  fix= re.sub( "\'", "\\'", os.environ[v])
  print "os.environ['"+v+"'] = '" + fix + "'" 
EOF
"""
        )
	setup.close()

	f = os.popen("/bin/sh " + filename)
	c1 = f.read()
	f.close()
        (realUpsStuff, ourOsEnvironmentStuff) = re.split('.*--------------cut here-------------', c1)
	os.environ = {}
        exec ourOsEnvironmentStuff




