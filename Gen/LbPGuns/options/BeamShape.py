# Set up particle gun
from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *

partgun = ParticleGun("ParticleGun")

# Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
partgun.EventType = 53210205;

# Add the beamshape gun
from Configurables import BeamShape
partgun.addTool(BeamShape, name="BeamShape")
partgun.ParticleGunTool = "BeamShape"
guntool = partgun.BeamShape

# Beam centroid
guntool.xCenter = 0*mm          # Beam position
guntool.yCenter = 0*mm          # Beam position
guntool.zCenter = -1000*mm      # Generation point
guntool.BeamDirection = 1       # Beam 1: 1, Beam 2: -1 
guntool.ThetaCenter = 0*rad     # Inclination angle (angle to z)
guntool.PhiCenter = 0*rad       # Azimuth angle 

# Beam parameters
guntool.Momentum = 450*GeV      # Beam particle momentum
guntool.PdgCode = 2212          # Particle PDG ID
guntool.xEmmitance = 7.82e-9*m  # Transverse emmitance
guntool.yEmmitance = 7.82e-9*m  # Transverse emmitance
guntool.xBeta = 10.0*m          # Optical beta function
guntool.yBeta = 10.0*m          # Optical beta function
guntool.xAlpha = 4.18e-7        # Correlation function
guntool.yAlpha = 1.18e-7        # Correlation function

# Set min and max number of particles to produce in an event
# defaults are = 1, i.e. always one particle
from Configurables import FlatNParticles
partgun.addTool(FlatNParticles, name="FlatNParticles")
partgun.NumberOfParticlesTool = "FlatNParticles"
numparttool = partgun.FlatNParticles
numparttool.MinNParticles = 1
numparttool.MaxNParticles = 1

