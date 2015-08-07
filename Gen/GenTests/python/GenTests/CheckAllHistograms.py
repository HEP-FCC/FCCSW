import os
import datetime

GaussVersion = 'v37r0'

listofhistos = {
#    '30000000': 'MinBias',
#    '11164011': 'BdD0KstKpiDecProdCut',
#    '13264001': 'BsDspiDecProdCut',
#    '12463010': 'BuD0hKpicocktail',
#    '10000000': 'InclB',
#    '20000000': 'InclD'#,
    '24142001': 'InclJpsi'
    }

refCastor = '/castor/cern.ch/user/r/robbep/GaussREF/'
pathCastor = '/castor/cern.ch/user/r/robbep/GaussGen_' + GaussVersion + '/'

# copy the index page from the Web area
webarea = os.environ['LHCBDOC'] + '/gauss/generator/validation/'
os.system( 'cp -f '+webarea+'index.html .')
toDay = datetime.date.today()
os.system( 'cp -f '+webarea+'index.html '+webarea+'index.html.'+toDay.isoformat() ) 

for eventtype, eventname in listofhistos.iteritems():
    # Copy CASTOR files locally
    refNameHisto = refCastor + 'GaussHistos_' + eventtype + '.root'
    copyRefHisto = 'rfcp ' + refNameHisto + ' /tmp/robbep'
    refNameLog = refCastor + 'GaussOutput_' + eventtype + '.txt'
    copyRefLog = 'rfcp ' + refNameLog + ' /tmp/robbep'

    os.system( copyRefHisto )
    os.system( copyRefLog )

    nameHisto = pathCastor + 'GaussHistos_' + GaussVersion + '_' + eventtype + '.root'
    copyHisto = 'rfcp ' + nameHisto + ' /tmp/robbep'
    nameLog = pathCastor + 'GaussOutput_' + GaussVersion + '_' + eventtype + '.txt'
    copyLog = 'rfcp ' + nameLog + ' /tmp/robbep'

    os.system( copyHisto )
    os.system( copyLog )

    # then do the tests
    os.system( 'python GeneratorReferencePlots.py --refhisto /tmp/robbep/GaussHistos_'+eventtype+'.root --reflog /tmp/robbep/GaussOutput_'+eventtype+'.txt --histo /tmp/robbep/GaussHistos_' + GaussVersion + '_' + eventtype + '.root --log /tmp/robbep/GaussOutput_' + GaussVersion + '_' + eventtype + '.txt --add-to-index --install' )

    # move to web area
    os.system( 'cp -rf Gauss_' + eventtype + '_*' + ' ' + webarea )   

    # Do some cleanup
    os.system( 'rm *.eps' )
    os.system( 'rm -rf Gauss_' + eventtype + '_*' )
    os.system( 'rm *.jpg' )
    os.system( 'rm page.html' )

os.system( 'cp -f index.html ' + webarea )
os.system( 'rm index.html' )

