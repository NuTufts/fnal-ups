#out = '.'

import os
from waflib.Utils import O755

def options(opt):
    opt.load('compiler_c waf_unit_test')

def configure(cfg):
    cfg.load('compiler_c waf_unit_test')
    cfg.env.env = dict(os.environ)
    cfg.env.env['UPS_DIR'] = cfg.srcnode.abspath()
    pass

def build(bld):
    # library
    bld.stlib(target='ups', name='staticlib',
              source = bld.path.ant_glob('src/ups*.c', excl=['ups_main.c']),
              includes = 'inc', features='c', install_path='${PREFIX}/lib')
    
    # headers
    bld.install_files('${PREFIX}/include', bld.path.ant_glob('inc/*.h'))
    
    # main program
    bld.program(target = 'bin/ups', source = 'src/ups_main.c', use = 'staticlib',
                includes = 'inc', features='c cprogram', install_path='${PREFIX}/bin')
    
    # support
    bld.install_files('${PREFIX}/', bld.path.ant_glob('ups/**'), relative_trick = True)

    def gen_help(t):
        cmd = '%s > %s' % (t.inputs[0].abspath(), t.outputs[0].abspath())
        return t.exec_command(cmd, env=t.env.env)
    bld(rule=gen_help,
        source = 'doc/build_help_file.pl src/upsact.c doc/ups_help.in',
        target = 'ups_help.dat')
    bld.install_as('${PREFIX}/doc/ups_help.dat', 'ups_help.dat')


    for tf,wf in [('true','win'),('false','fail')]:
        setup_wf = 'bin/setup_'+wf
        bld(rule=lambda t: t.outputs[0].write('#!/bin/sh\n%s\n'%tf),
            update_outputs = True,
            target = setup_wf, chmod=O755)
        bld.install_files('${PREFIX}/bin', [setup_wf], chmod=O755)
    for pl in ['parent','active']:
        bld.install_as('${PREFIX}/bin/ups_%s'%pl, 'src/ups_%s.pl'%pl, chmod=O755)
        



    for tst in bld.path.ant_glob('test/test_*.c'):
        bld.program(features = 'c cprogram',
                    target = 'test/'+str(tst).replace('.c',''),
                    includes = 'inc',
                    source = [tst], 
                    use = 'staticlib', 
                    install_path=None)


    ups_test = bld.path.find_or_declare('test/ups_test')
    bld.program(features = 'c cprogram',
                target = ups_test,
                includes = 'inc',
                source = bld.path.ant_glob('test/ups*.c'),
                use = 'staticlib',
                install_path = None)

    # running tests requires in-source build or more tweakage than I care to do
    # for tst in bld.path.ant_glob('test/scripts/upstst_*', excl=['test/scripts/upstst_all']):
    #     bld(rule= '%s > log-%s 2>&1' % (tst.abspath(),str(tst)),
    #         features = 'test', 
    #         source = [tst, ups_test] + bld.path.ant_glob('**/test/scripts'),
    #         cwd = 'test/scripts')
            
