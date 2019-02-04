from Gaudi.Configuration import *

###### Parser ################

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--input', type=str, default=None, help='specify an input File')
my_args, _ = parser.parse_known_args()

from FWCore.joboptions import parse_standard_job_options
args = parse_standard_job_options()

"""
my_input = "simu_test.root"
outfilename = "reco_test.root"
"""

if args.outputfile is not None:
    outfilename = args.outputfile

if my_args.input != None:
    my_input = my_args.input

################################

#my_input = "simu_test.root"
#outfilename = "reco_100eV_DCA8mm.root"
"""
my_input ="/eos/user/n/nali/Hadrons/HadZ.root"
outfilename = "/eos/user/n/nali/Hadrons/Analysis_HadZ.root"

"""
my_input ="/eos/user/n/nali/SR/SR.root"
outfilename = "/eos/user/n/nali/SR/Analysis_SR.root"

from GaudiKernel.SystemOfUnits import eV
from Configurables import ApplicationMgr, FCCDataSvc, PodioOutput

podioevent   = FCCDataSvc("EventDataSvc", input=my_input)



from Configurables import PodioInput, ReadTestConsumer
podioinput = PodioInput("PodioReader", collections=["positionedHits_DCH"], OutputLevel=DEBUG)
#checker = ReadTestConsumer()


# DD4hep geometry service
# Parses the given xml file
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['Detector/DetFCCeeIDEA/compact/FCCee_DectEmptyMaster.xml',
                                         'Detector/DetFCCeeIDEA/compact/DriftChamber.xml'], 
                    OutputLevel = DEBUG)

from Configurables import CreateDCHHits
createhits = CreateDCHHits("CreateDCHHits",
                           readoutName = "DriftChamberCollection",
                           outFileName=outfilename,
                           EdepCut = 100*1e-9,
                           DCACut = 0.8,
                           OutputLevel=DEBUG)

createhits.positionedHits.Path = "positionedHits_DCH"
createhits.mergedHits.Path = "merged_DCH"


ApplicationMgr( TopAlg = [podioinput, 
                          createhits, 
                          ],
                EvtSel = 'NONE',
                EvtMax   = -1,
                # order is important, as GeoSvc is needed by SimG4Svc
                ExtSvc = [podioevent, 
                          geoservice
                          ],
                OutputLevel = DEBUG
 )


