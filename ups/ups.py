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
#       ups.getUps( [setupsDir=/path/to/setups.sh], ['pythonVersion'='python instance'] )
#  
#  ups.setup('my project spec')
#############################################################

def set_setupsDir(setupsDir=DEFAULT_SETUPS_DIR):
    global DEFAULT_SETUPS_DIR
    DEFAULT_SETUPS_DIR = setupsDir

def set_pyVersion(pythonVersion=DEFAULT_PYTHON_VERSION):
    global DEFAULT_PYTHON_VERSION
    DEFAULT_PYTHON_VERSION = pythonVersion
    
#############################################################
# returns the address of a singleton upsManager object:

singletonUps = None
def getUps(setupsDir=DEFAULT_SETUPS_DIR, pythonVersion=DEFAULT_PYTHON_VERSION):
    global singletonUps
    if ( singletonUps is None ):
        singletonUps = upsManager(setupsDir, pythonVersion)
    return singletonUps

def setup(arg, setupsDir=DEFAULT_SETUPS_DIR, pythonVersion=DEFAULT_PYTHON_VERSION):
    upsmgr = getUps()
    upsmgr.setup(arg)

##############################################################
class upsException(Exception):
    def __init__(self, msg):
        self.args = msg
    
class upsManager:
    def __init__(self, setupsDir=DEFAULT_SETUPS_DIR, pythonVersion=DEFAULT_PYTHON_VERSION):

        # initial setup of ups itself:
        os.environ['UPS_SHELL'] = 'sh'
	f = os.popen( '. %s/setups.sh; ' % setupsDir + \
                      'echo os.environ\\[\\"UPS_DIR\\"\\]=\\"$UPS_DIR\\"; ' + \
                      'echo os.environ\\[\\"PRODUCTS\\"\\]=\\"$PRODUCTS\\";' + \
                      'echo os.environ\\[\\"SETUP_UPS\\"\\]=\\"$SETUP_UPS\\"')
	exec f.read()
	f.close()

        # do we need a specific python version?  (If not specified, assume we don't).
        if ( pythonVersion != DEFAULT_PYTHON_VERSION ):
            self._use_python(pythonVersion)

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
    def _use_python(self,v):
	if ( v == "" ):
	    vm = '.*';
	else:
	    vm = v;

	if ( not os.environ.has_key('SETUP_PYTHON') or None == re.search('python '+vm,os.environ.get('SETUP_PYTHON',''))):
	    if os.environ.has_key('SETUP_PYTHON'):
		self._inhaleresults(os.environ["UPS_DIR"] + '/bin/ups unsetup python')
            self._inhaleresults(os.environ["UPS_DIR"] + '/bin/ups setup python ' + v)

            # were we running the python interpreter itself?  Special handling required!
            if ( sys.argv == [''] ):
                sys.argv = [ os.environ['PYTHON_DIR'] + '/bin/python' ]
            else:
                sys.argv.insert(0, os.environ['PYTHON_DIR'] + '/bin/python')
	    os.execve( os.environ['PYTHON_DIR']+'/bin/python', sys.argv, os.environ )
    ##############################################################################
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
echo "--------------cut here-------------"
cat <<'EOF' | python
import os
import re
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
	c1 = re.sub( '.*--------------cut here-------------', '', c1)
	os.environ = {}
	exec c1




