from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCDumper

# # reads HepMC text file and write the HepMC::GenEvent to the data service
# reader = HepMCReader("Reader", Filename="/afs/cern.ch/exp/fcc/sw/0.7/testsamples/testHepMCrandom.dat")
# reader.DataOutputs.hepmc.Path = "hepmc"

from Configurables import HepMCConverter
hepmc_converter = HepMCConverter("Converter")
hepmc_converter.DataInputs.hepmc.Path="hepmc"
hepmc_converter.DataOutputs.genparticles.Path="allGenParticles"
hepmc_converter.DataOutputs.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[# 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                         # 'file:Detector/DetFCChhECalSimple/compact/FCChh_ECalBarrel_Mockup.xml'
    'Test/TestGeometry/data/TestBoxCaloSD_segmentation.xml'
], OutputLevel = DEBUG)


from Configurables import TestCellCounting
cells = TestCellCounting("cells", readoutName="ECalHits", # BarECal_Readout",
                         fieldNames=["system"], #,"active","EM_barrel","layer"],
                         volumeMatchName="BoxECal",
                         fieldValues=[0 # 5,1,1,0
                         ], OutputLevel = DEBUG)

from Configurables import FCCDataSvc, PodioOutput
podiosvc = FCCDataSvc("EventDataSvc")
out = PodioOutput("out", filename="testCellCounting.root")
out.outputCommands = ["keep *"]

# ApplicationMgr
ApplicationMgr(EvtSel='NONE',
               EvtMax=5,
               TopAlg=[cells, out],
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podiosvc, geoservice],
                OutputLevel=DEBUG)
