##############################################################################
# Example options to generate cosmic rays with their distribution in
# momentum, theta and phi; the xyz vertex options define the region
# of the detector which one wants to be populated; the 
# vertex is extrapolated from there to a plane above the 
# detector.
##############################################################################
#
# This example will generate one mu+ or mu- per event passing
# through the RICH1 scintillator trigger
#   
# Include this file instead of the EVENTTYPE.opts after all you other 
# options.
#
# @author : C.Jones
# @date   : 15/07/2009
##############################################################################

from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *

# Set up ParticleGun
ParticleGun = ParticleGun("ParticleGun")

# Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
# gsdctnxu:
# g=type of process (5=particle gun)
# s=which particles are present and pass 400mrad w.r.t.beam cut; 2=mu
# d=decay; particle or antipart; 2=several particles (??)
# c=eta and p distribution : 2 (0=fix p, 1=flat eta, 2=flat xy, 3=mom range)
# tnxu = track parameters; if c=2 or 3, tn=00;
ParticleGun.EventType = 52220000

# Pick the type of particle gun generator
from Configurables import Cosmics
ParticleGun.addTool(Cosmics, name="Cosmics")
ParticleGun.ParticleGunTool = "Cosmics"

# Set min and max number of particles to produce in an event
# defaults are = 1, i.e. always one particle
from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
#ParticleGun.FlatNParticles.MinNParticles = 1
#ParticleGun.FlatNParticles.MaxNParticles = 1

# PDG code of a list of possible particles to be generated, default = {13,-13)
# GM: This should not be changed
ParticleGun.Cosmics.PdgCodes = [-13, 13]

# Generate position and angle
ParticleGun.Cosmics.xMin = -1000.0*mm
ParticleGun.Cosmics.xMax =  1000.0*mm
ParticleGun.Cosmics.yMin =   200.0*mm
ParticleGun.Cosmics.yMax =  1200.0*mm
ParticleGun.Cosmics.z    = -1500.0*mm
ParticleGun.Cosmics.ThetaMin = 0.105*rad
ParticleGun.Cosmics.ThetaMax = 0.563*rad
ParticleGun.Cosmics.PhiMin   =  3.14*rad
ParticleGun.Cosmics.PhiMax   =  6.28*rad

# The momentum of the particle is derived from randomly generated value
# of abs(P) (default min = 1 and max = 100 GeV)
ParticleGun.Cosmics.MomentumMin  =     1000.0*MeV
ParticleGun.Cosmics.MomentumMax  =   100000.0*MeV
ParticleGun.Cosmics.TimeOfInteraction  = 10.2*ns # want ~ 11.9 in RICH1 

#ParticleGun.OutputLevel = 1
#from Configurables import GetMCRichHitsAlg
#GetMCRichHitsAlg("GetRichHits").OutputLevel = 1
