# This example prints our the magnetic field

from Gaudi.Configuration import *

# Set up the magnetic field service
from Configurables import InterpolatedBFieldSvc
bFieldSvc = InterpolatedBFieldSvc("BFieldSvc", fieldMap = '/eos/experiment/fcc/hh/simulation/MagneticField/FCChhBField_rz.root', treeName= 'bField', lengthScalor=1., bFieldScalor= 1., cylinderCoordinates = TRUE, firstOctant=TRUE, OutputLevel = VERBOSE)

# The geant4 magnetic field tool
from Configurables import SimG4BFieldTool
simG4BFieldTool = SimG4BFieldTool("SimG4BFieldTool", FieldOn=True, IntegratorStepper="ClassicalRK4", bFieldSvc=bFieldSvc)

# Set up the magnetic field writer
from Configurables import BFieldG4CrossCheck
bFieldG4CrossCheck = BFieldG4CrossCheck("BFieldG4CrossCheck", bFieldSvc=bFieldSvc, g4magneticField=simG4BFieldTool, OutputLevel = VERBOSE)

# set up the event loop
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
               EvtSel = 'NONE',
               EvtMax   = 1,
               # order is important, as GeoSvc is needed by SimG4Svc
               ExtSvc = [bFieldSvc,bFieldG4CrossCheck],
               OutputLevel=VERBOSE
               )
