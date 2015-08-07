#!/usr/bin/env python
"""
Author  : Philip Ilten
Created : 2015-08-03

Script to generate LPAIR code that can be used with Gauss. 

"""

import os, urllib2, tarfile, shutil, Fortran
ver = {'LPAIR': 'LPair.120920', 'CERNLIB': '2006'}
url = {'LPAIR': None, 'CERNLIB': 'http://cernlib.web.cern.ch/cernlib/'
       'download/2006_source/tar/'}

# Untar the LPAIR source.
if not os.path.isdir(ver['LPAIR']):
    tgz  = tarfile.open(ver['LPAIR'] + '.tgz', 'r:gz')
    tgz.extractall()
    tgz.close()

# Untar the CERNLIB source.
if not os.path.isdir(ver['CERNLIB']):
    info = urllib2.urlopen(url['CERNLIB'] + ver['CERNLIB'] + '_src.tar.gz')
    data = info.read()
    code = file(ver['CERNLIB'] + '.tgz', 'w')
    code.write(data)
    code.close()
    tgz  = tarfile.open(ver['CERNLIB'] + '.tgz', 'r:gz')
    tgz.extractall()
    tgz.close()

# Helper function to copy files.
def copy(src, dst, include = None, exclude = None):
    filenames = [os.path.join(src, f) for f in os.listdir(src)]
    for f in filenames:
        if exclude and os.path.basename(f) in exclude: continue
        if include and not os.path.basename(f) in include: continue
        if os.path.isfile(f):
            shutil.copy(f, os.path.join(dst, os.path.basename(f)))
        elif not os.path.isdir(os.path.join(dst, os.path.basename(f))):
            shutil.copytree(f, os.path.join(dst, os.path.basename(f)))
        
# Copy the needed source files.
copy(ver['LPAIR'] + '/SRC', '../src',
     exclude = ['README', 'gmupdf.f', 'treat_atlas.f', 'zdis.f'])
copy(ver['LPAIR'] + '/PHOTOS', '../src',
     exclude = ['MakeLibrary.sh'])
copy(ver['CERNLIB'] + '/src/packlib/ffread/sffread', '../src',
     include = ['ffkey.F', 'ffupca.F', 'fffind.F', 'ffinit.F', 'ffuser.F',
                'ffuset.F', 'ffgo.F', 'ffcard.F', 'ffskip.F'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/ccgen', '../src',
     include = ['systemf.F', 'systei.c'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/ccgenu', '../src',
     include = ['fchtak.c'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/tcgen', '../src',
     include = ['lenocc.F', 'icfnbl.F', 'iclocl.F', 'icdeci.F'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/tcgens', '../src',
     include = ['uhtoc.F', 'uctoh.F', 'iucomp.F', 'ucopy2.F', 'vfill.F',
                'ucopy.F'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen/tcgenc', '../src',
     include = ['locf.F', 'timex.F', 'jumpxn.F', 'abend.F'])
copy(ver['CERNLIB'] + '/src/mclibs/jetset/jetset', '../src',
     include = ['ulmass.F', 'luexec.F', 'lujoin.F', 'lulist.F',
                'luchge.F', 'luprep.F', 'lucomp.F', 'lushow.F', 'luindf.F',
                'ludecy.F', 'luerrm.F', 'luboei.F', 'lustrf.F', 'lurobo.F',
                'lukfdi.F', 'ulangl.F', 'lutaud.F', 'luptdi.F', 'luzdis.F',
                'lulogo.F', 'luname.F', 'plu.F'])
copy(ver['CERNLIB'] + '/src/mclibs/jetset/pythia', '../src',
     include = ['pyspli.F'])

# Copy the needed header folders.
copy(ver['CERNLIB'] + '/src/packlib/ffread/', '../src',
     include = ['ffread'])
copy(ver['CERNLIB'] + '/src/packlib/kernlib/kerngen', '../src',
     include = ['kerngen'])

# Fix the warnings in the C-code.
systei = file('../src/systei.c', 'r')
lines  = systei.readlines()
systei.close()
systei = file('../src/systei.c', 'w')
for l in lines:
    if l.startswith('#include <stdio.h>'): systei.write('#include <stdlib.h>\n')
    systei.write(l)
systei.close()
fchtak = file('../src/fchtak.c', 'r')
lines  = fchtak.readlines()
fchtak.close()
fchtak = file('../src/fchtak.c', 'w')
for l in lines:
    if l.startswith('      char *malloc();'): continue
    if l.startswith('#include <stdio.h>'): fchtak.write('#include <stdlib.h>\n')
    fchtak.write(l)
fchtak.close()

# Use the Gaudi random number generator.
rlu = file('../src/rlu.F', 'w')
rlu.write('      FUNCTION RLU(IDUMMY)\n      REAL*8 CPYR\n'
          '      RLU = CPYR(IDUMMY)\n      RETURN\n      END\n')
rlu.close()

# Generate the C-interface.
#src = Fortran.Source('../src/SuperChic.f')
#cin = file('../SuperChic/SuperChic.h', 'w')
#cin.write('// Generated automatically with Gen/SuperChic/python/SuperChic.py.'
#          + '\n// Do not manually modify. Based on ' + ver + '.\n')
#cin.write('extern "C" {\n\n')
#cin.write(src.procedures['superchic'].cexpr() + '\n\n')
#for k, c in src.commons.iteritems():
#    if k != 'hepevt': cin.write(c.cexpr() + '\n\n')
#cin.write('}\n')
