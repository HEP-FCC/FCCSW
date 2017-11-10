## CMS detector configuration for papas

from Gaudi.Configuration import *

from Configurables import CMSFieldSvc, CMSTrackerSvc, CMSEcalSvc, CMSHcalSvc
fieldsvc = CMSFieldSvc("CMSFieldSvc"); #todo add in remaining parameters
ecalsvc = CMSEcalSvc("CMSEcalSvc");
hcalsvc = CMSHcalSvc("CMSHcalsvc");
trackersvc = CMSTrackerSvc("CMSTrackerSvc");

from Configurables import CMSDetSvc
detservice = CMSDetSvc("PapasDetSvc",
                        ecalService = "CMSEcalSvc",
                        hcalService = "CMSHcalSvc",
                        trackerService = "CMSTrackerSvc",
                        fieldService = "CMSFieldSvc");


