import os
from random import Random

#--------------------------------
# Put Gauss version here
#--------------------------------
GaussVersion = 'v37r0'
#--------------------------------

listoftests = {
    '30000000': 'MinBias',
    '11164011': 'BdD0KstKpiDecProdCut',
    '13264001': 'BsDspiDecProdCut',
    '12463010': 'BuD0hKpicocktail',
    '10000000': 'InclB',
    '20000000': 'InclD',
    '24142001': 'InclJpsi'
    }

#----------------------------------------------
# Random for event number and run number
#---------------------------------------------
random = Random()
random.seed(GaussVersion)

#-----------------------------------------------
# Creates the CASTOR dir if it does not exists
#---------------------------------------------
castorcmd = "rfmkdir /castor/cern.ch/user/r/robbep/GaussGen_"+GaussVersion
os.system(castorcmd)

GENTestsPath = os.environ['GENTESTSROOT']
test_backend = LSF(queue='1nd')


for eventtype, eventname in listoftests.iteritems():

    testName = "GaussTest_"+eventtype+"_"+GaussVersion
    FirstEvent = random.randint( 1 , 1500 )
    RunNumber = random.randint( 1 , 9599 )

    test_application = Gauss()
    test_application.version = GaussVersion

    test_application.optsfile = File( GENTestsPath + '/options/GaussMonitoring'+eventname+'.py' )
    
    test_application.extraopts = """
HistogramPersistencySvc().OutputFile="/castor/cern.ch/user/r/robbep/GaussGen_"""+GaussVersion+"""/GaussHistos_"""+GaussVersion+"""_"""+eventtype+""".root"
GaussGen=GenInit("GaussGen")
GaussGen.FirstEventNumber="""+str(FirstEvent)+"""
GaussGen.RunNumber="""+str(RunNumber)+"""
    """
    
    j = Job(
        name        = testName ,
        application = test_application ,
        backend     = test_backend
        )
    
    j.submit()
    
