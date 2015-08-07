##############################################################################
# Example options to generate particle guns with flat random distribution in
# momentum, theta and phi originating from a flat vertx distribution.
##############################################################################
#
#  This example will generate one pi+ or pi- per event originating at
#  (0,0,0) in the LHCb reference system, with 1 GeV < abs(P) < 100 GeV,
#  0.15 mrad < theta < 0.300 mrad and 0 < phi < 2pi
#    
#  Include this file instead of the EVENTTYPE.opts after all you other 
#  options.
#
#  @author : G.Corti
#  @date   : last modified on 2009-11-04
##############################################################################
 
from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *

# Set up ParticleGun
ParticleGun = ParticleGun("ParticleGun")

# Control of the ParticleGun
# Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
ParticleGun.EventType = 53210205;

# GunMode flag:
#   if = MomentumRange uses parameters below within random values generated
#   if = FixedMomentum need to set fix values of px, py, pz via specific properties
from Configurables import MomentumRange
ParticleGun.addTool(MomentumRange, name="MomentumRange")
ParticleGun.ParticleGunTool = "MomentumRange"

#from Configurables import FixedMomentum
#ParticleGun.addTool(FixedMomentum, name="FixedMomentum")
#ParticleGun.ParticleGunTool = "FixedMomentum"

# Set min and max number of particles to produce in an event
# defaults are = 1, i.e. always one particle
from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
#ParticleGun.FlatNParticles.MinNParticles = 1
#ParticleGun.FlatNParticles.MaxNParticles = 1

# PDG code of a list of possible particles to be generated, default = {-211})
#ParticleGun.MomentumRange.PdgCodes = [-211, 211]
ParticleGun.MomentumRange.PdgCodes = [-13, 13]
#ParticleGun.FixedMomentum.PdgCodes = [-211, 211]

# The vertex of each particle gun is randomly generated in a flat
# distributionwithin the given limits, when min=max the vertex is in the
# given point. Default is 0.0*mm for all.
#from Configurables import FlatSmearVertex
#ParticleGun.addTool(FlatSmearVertex, name="FlatSmearVertex")
#ParticleGun.VertexSmearingTool = "FlatSmearVertex"
#ParticleGun.FlatSmearVertex.xVertexMin = 0.0*mm;  
#ParticleGun.FlatSmearVertex.xVertexMax = 0.0*mm
#ParticleGun.FlatSmearVertex.yVertexMin = 0.0*mm
#ParticleGun.FlatSmearVertex.yVertexMax = 0.0*mm
#ParticleGun.FlatSmearVertex.zVertexMin = 0.0*mm
#ParticleGun.FlatSmearVertex.zVertexMax = 0.0*mm

# The momentum of the particle is derived from randomly generated value
# of abs(P) (default for both min and max = 100 GeV), theta (default 
# min = 0.1*rad and max = 0.4*rad) and phi (default min = 0, max = twopi*rad)
ParticleGun.MomentumRange.MomentumMin = 200.0*GeV
ParticleGun.MomentumRange.MomentumMax = 300.0*GeV
ParticleGun.MomentumRange.ThetaMin    = 0.015*rad
ParticleGun.MomentumRange.ThetaMax    = 0.300*rad
#ParticleGun.MomentumRange.PhiMin     =   0.0*rad
#ParticleGun.MomentumRange.PhiMax     =  6.28*rad

# When GunMode = FixedMomentum, the specific options for setting px, py and pz are
# the following with default values listed
#ParticleGun.FixedMomentum.px = 1.0*GeV
#ParticleGun.FixedMomentum.py = 1.0*GeV
#ParticleGun.FixedMomentum.pz = 1.0*GeV

