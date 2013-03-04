
. unittest.bash

setup_test_db() {
  
   SAVEPRODUCTS=$PRODUCTS
   export workdir=/tmp/p$$
   . `ups setup upd v4_8_0`
   mkprd -r $workdir
   echo 'SETUPS_DIR=${UPS_THIS_DB}' >> $workdir/.upsfiles/dbconfig
   PRODUCTS=$workdir
   SETUPS_DIR=$workdir
   mkdir -p  $workdir/ups/devel
   cp -r $UPS_DIR $workdir/ups/devel/Linux+2
   ups declare -r $workdir/ups/devel/Linux+2 -C -M ups -m ups.table ups devel -2 
}

teardown_test_db() {
   rm -rf $workdir
   PRODUCTS=$SAVEPRODUCTS
}

test_env() {
   echo PRODUCTS is $PRODUCTS
   echo UPS_DIR is $UPS_DIR
   ups list -aK+ ups 
   [ `ups list -aK+ ups | wc -l` = 1 ]
}

test_current() {
   rm $SETUPS_DIR/setups_layout
   ups declare -c ups devel -2 || true

   ls -al $workdir
   ups list -a 

   [ -r $SETUPS_DIR/setups_layout ] &&
   [ `ls -l $workdir/set* | wc -l` = 7 ] &&
   [ `ups list -K+ ups | wc -l` = 1 ]
}

test_empty_sh() {
   ups declare -c ups devel -2 || true
   env -i - PATH=/bin:/usr/bin HOME=$HOME /bin/sh << EOF
       . $workdir/setups.sh
       ups list -aK+ ups
       [  \`ups list -aK+ ups | wc -l\` = 1  ]
EOF
}

test_empty_csh() {
   ups declare -c ups devel -2 || true
   env -i - PATH=/bin:/usr/bin HOME=$HOME /bin/csh << EOF
       source  $workdir/setups.csh
       ups list -aK+ ups
       [  \`ups list -aK+ ups | wc -l\` = 1  ]
EOF
}

test_over_old() {
    upd install -G -c ups v4_7
    ups declare -c ups devel -2
    
   ls -l $workdir/set*
   ls -l $workdir/.old

   [ `ls -l $workdir/set* | wc -l` = 7 ] &&
   [ `ups list -K+ ups | wc -l` = 1 ]

}

testsuite setups_suite \
        -s setup_test_db \
	-t teardown_test_db \
	test_env   	\
	test_current	\
	test_empty_sh	\
	test_empty_csh  

setups_suite "$@"

