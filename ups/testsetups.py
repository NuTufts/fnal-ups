
import sys
sys.path.insert(0,"/usr/local/etc")
import setups
import os
 
ups = setups.setups()
ups.use_python("v1_5_2")
ups.setup("xemacs")
print "XEMACS_DIR is ", os.environ["XEMACS_DIR"]
print "PYTHON_DIR is ", os.environ["PYTHON_DIR"]
print ups.ups("list","upd")
