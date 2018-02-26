# This example prints our the magnetic field

from Gaudi.Configuration import *

# Set up the magnetic field service
from Configurables import InterpolatedBFieldSvc
bFieldSvc = InterpolatedBFieldSvc("BFieldSvc", fieldMap = '/eos/experiment/fcc/hh/simulation/MagneticField/FCChhBField_rz.root', treeName= 'bField', lengthScalor=1., bFieldScalor= 1., cylinderCoordinates = TRUE, firstOctant=TRUE, OutputLevel = VERBOSE)

# Set up the magnetic field writer
from Configurables import BFieldSymmetryCheck
bFieldSymmetryCheck = BFieldSymmetryCheck("BFieldSymmetryCheck", bFieldSvc=bFieldSvc, rMax = 20000., rBins=100, zMax = 30000., zBins = 150, phiBins = 50, OutputLevel = VERBOSE)

# set up the event loop
from Configurables import ApplicationMgr
ApplicationMgr( TopAlg = [],
               EvtSel = 'NONE',
               EvtMax   = 1,
               # order is important, as GeoSvc is needed by SimG4Svc
               ExtSvc = [bFieldSvc,bFieldSymmetryCheck],
               OutputLevel=VERBOSE
               )
