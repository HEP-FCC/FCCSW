## Configuration for CLIC detector for papas

from Gaudi.Configuration import *

from Configurables import ClicFieldSvc, ClicTrackerSvc, ClicEcalSvc, ClicHcalSvc
fieldsvc = ClicFieldSvc("ClicFieldSvc"); #todo add in more parameters
ecalsvc = ClicEcalSvc("ClicEcalSvc", emin = [.5, .5]);
hcalsvc = ClicHcalSvc("ClicHcalsvc");
trackersvc = ClicTrackerSvc("ClicTrackerSvc");

from Configurables import ClicDetSvc
detservice = ClicDetSvc("PapasDetSvc",
                        ecalService = "ClicEcalSvc",
                        hcalService = "ClicHcalSvc",
                        trackerService = "ClicTrackerSvc",
                        fieldService = "ClicFieldSvc");

