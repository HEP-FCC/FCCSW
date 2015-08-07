# $Id: GaussMonitoringBdJpsiKSCPVDecProdCut.py,v 1.1 2009-05-15 08:47:03 robbep Exp $ 

from Gauss.Configuration import *

LHCbApp().DDDBtag = "head-20090330"
LHCbApp().CondDBtag = "sim-20090402-vc-mu100"

importOptions('$GENTESTSROOT/options/GaussMonitoringBdJpsiKSCPVDecProdCut.opts')
