##############################################################################
# Example options to generate cosmic rays with their distribution in
# momentum, theta and phi; the xyz vertex options define the region
# of the detector which one wants to be populated; the 
# vertex is extrapolated from there to a plane above the 
# detector.
##############################################################################
#
# This example will generate one mu+ or mu- per event originating at
# (x,y,z) as explained before, with 1 GeV < abs(P) < 100 GeV,
# 0 degrees < theta < 85 degrees and 0 < phi < 2pi
#   
# Include this file instead of the EVENTTYPE.opts after all you other 
# options.
#
#  @author : G.Manca
#  @date   : last modified on 2009-11-04
##############################################################################

from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *

# Set up ParticleGun
ParticleGun = ParticleGun("ParticleGun")

# Control of the ParticleGun
# Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
# gsdctnxu:
# g=type of process (5=particle gun)
# s=which particles are present and pass 400mrad w.r.t.beam cut; 2=mu
# d=decay; particle or antipart; 2=several particles (??)
# c=eta and p distribution : 2 (0=fix p, 1=flat eta, 2=flat xy, 3=mom range)
# tnxu = track parameters; if c=2 or 3, tn=00;
ParticleGun.EventType = 52220000;

#Pick the type of particle gun generator
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

# GM: The region (a surface) of the detector you want populated. The 
# real vertex will be extrapolated from this point at the point
# y=+6m
# This is an option as someone might want to generate only in 
# one particular region of the detector.
# default is the all detector.
# the surface will be at the z fixed by z
ParticleGun.Cosmics.xMin = -5000.0*mm
ParticleGun.Cosmics.xMax =  5000.0*mm
ParticleGun.Cosmics.yMin = -5000.0*mm
ParticleGun.Cosmics.yMax =  5000.0*mm
ParticleGun.Cosmics.z    =     0.0*mm


# The momentum of the particle is derived from randomly generated value
# of abs(P) (default min = 1 and max = 100 GeV), theta (default 
# min = 0.1*rad and max = pi/2) 
# and phi (default min = 0, max = twopi*rad)
ParticleGun.Cosmics.MomentumMin =   1000.0*MeV
ParticleGun.Cosmics.MomentumMax = 100000.0*MeV
ParticleGun.Cosmics.ThetaMin    =      0.0*rad #0 degrees
ParticleGun.Cosmics.ThetaMax    =    1.483*rad #85 degrees
ParticleGun.Cosmics.PhiMin      =      0.0*rad
ParticleGun.Cosmics.PhiMax      =     6.28*rad

# Max Distance M1=10.85   m,  Td=32.55   ns  Min Distance M1=9.999 m,  Td=29.997ns
# Max Distance M2=16.4936 m,  Td=49.4809 ns  Min Distance M2=15.2  m,  Td=45.6  ns : 55 ns
# Max Distance M3=17.7958 m,  Td=53.3873 ns  Min Distance M3=16.4  m,  Td=49.2  ns : 58 ns
# Max Distance M4=19.0979 m,  Td=57.2937 ns  Min Distance M4=17.6  m,  Td=52.8  ns : 62 ns
# Max Distance M5=20.4    m,  Td=61.2001 ns  Min Distance M5=18.8  m,  Td=56.4  ns : 66 ns 

ParticleGun.Cosmics.TimeOfInteraction  = 0.0*ns #the time the hit fires the chosen surface

# GM 02.09.09: 
# this is needed in order to get the right Cosmic angular distribution
from Configurables import GiGaGeo
GiGaGeo().XsizeOfWorldVolume = 500.0*m
GiGaGeo().YsizeOfWorldVolume = 500.0*m
GiGaGeo().ZsizeOfWorldVolume = 500.0*m 

from Configurables import TrCutsRunAction
TrCutsRunAction("TrCuts").MinX = -50*m
TrCutsRunAction("TrCuts").MaxX =  50*m
TrCutsRunAction("TrCuts").MinY = -50*m
TrCutsRunAction("TrCuts").MaxY =  50*m
TrCutsRunAction("TrCuts").MinZ = -50*m
TrCutsRunAction("TrCuts").MaxZ =  50*m

