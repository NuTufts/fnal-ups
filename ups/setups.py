import os
import re
import sys


class setups:
    def __init__(self):
	os.environ['UPS_SHELL'] = "sh"
	# f = os.popen( '. ${Setups_Home}/setups.sh; echo os.environ\\[\\"UPS_DIR\\"\\]=\\"$UPS_DIR\\"; echo os.environ\\[\\"PRODUCTS\\"\\]=\\"$PRODUCTS\\"; echo os.environ\\[\\"SETUP_UPS\\"\\]=\\"$SETUP_UPS\\"')
	f = os.popen( '. /usr/local/etc/setups.sh; echo os.environ\\[\\"UPS_DIR\\"\\]=\\"$UPS_DIR\\"; echo os.environ\\[\\"PRODUCTS\\"\\]=\\"$PRODUCTS\\"; echo os.environ\\[\\"SETUP_UPS\\"\\]=\\"$SETUP_UPS\\"')
	exec f.read()
	f.close()

    def setup(self, arg):
	self.inhaleresults(os.environ["UPS_DIR"] + '/bin/ups setup ' + arg)

    def unsetup(self, arg):
	self.inhaleresults(os.environ["UPS_DIR"] + '/bin/ups unsetup ' + arg)

    def inhaleresults(self, cmd):

	f = os.popen(cmd)
	filename = f.read()
	filename = filename[0:-1]
	f.close()

	setup = open(filename,"a")
	setup.write(
	    ''' 
echo "--------------cut here-------------"
cat <<'EOF' | python
import os
import re
for v in os.environ.keys():
    fix= re.sub( "\'", "\\'", os.environ[v])
    print "os.environ['"+v+"'] = '" + fix + "'" 
EOF
	    ''')
	setup.close()

	f = os.popen("/bin/sh " + filename)
	c1 = f.read()
	f.close()
	c1 = re.sub( '.*--------------cut here-------------', '', c1)
	os.environ = {}
	exec c1

    def use_python(self,v):
	if ( v == "" ):
	    vm = '.*';
	else:
	    vm = v;

	if ( not os.environ.has_key('SETUP_PYTHON') or None == re.search('python '+vm,os.environ.get('SETUP_PYTHON',''))):
	    if os.environ.has_key('SETUP_PYTHON'):
		self.unsetup("python")
	    self.setup("python " + v)
	    print "Switching to python in",os.environ['PYTHON_DIR']
	    print "PYTHONPATH is", os.environ['PYTHONPATH']
	    print "PYTHON_DIR is", os.environ['PYTHON_DIR']
	    sys.argv.insert(0, os.environ['PYTHON_DIR']+'/bin/python')
	    os.execve( os.environ['PYTHON_DIR']+'/bin/python', sys.argv, os.environ )
