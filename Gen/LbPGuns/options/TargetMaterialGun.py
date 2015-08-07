##############################################################################
# Example options to generate a fixed number of particle guns per event with 
# flat random distribution or in a regular grid in either an x-y 
# rectangular plane at a given z, or in eta-phi.
# Only one type of particle is generated.
# This is useful for generating non interacting particles (neutrinos
# can be used) to evaluate material in the detector at given positions. 
##############################################################################
# 
# This example will generate one nu_e per event originating at
# (0,0,0) in the LHCb reference system, in a regular grid in eta-phi
# extending over the whole LHCb acceptance.
# To cover the whole eta-phi grid with the default values for its size and
# number of steps it is necessary to have "ApplicationMgr.EvtMax = 8792;",
# for the whole x-y grid "ApplicationMgr.ExtMax = 8320;" 
#   
# Include this file instead of the EVENTTYPE.opts after all you other 
# options.
#
#  @author : G.Corti
#  @date   : last modified on 2009-11-04
##############################################################################

#from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *

# Set up ParticleGun
ParticleGun = ParticleGun("ParticleGun")

# Control of the ParticleGun
# Event type is set as described in LHCb-2005-034 : G=5! (Def = 0)
ParticleGun.EventType = 53210205

# Pick the type of particle gun generator
from Configurables import MaterialEval
ParticleGun.addTool(MaterialEval, name="MaterialEval")
ParticleGun.ParticleGunTool = "MaterialEval"

# Set fixed number of particles to produce in an event (default = 10) and
# their PDG code (default = 2221111)
from Configurables import FlatNParticles
ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
ParticleGun.FlatNParticles.MinNParticles = 100
ParticleGun.FlatNParticles.MaxNParticles = 100
ParticleGun.MaterialEval.PdgCode = -2212

# The vertex of each particle gun is produced in a single point of given
# coordinates. Default is 0.0*mm for all. 
# distribution within the given limits, when min=max the vertex is in the
# given point. 
#ParticleGun.MaterialEval.Xorig = 0*mm
#ParticleGun.MaterialEval.Yorig = 0*mm
ParticleGun.MaterialEval.Zorig = 500*mm

# The abs(P) of the particle if fixed at the given value (default = 500 GeV)
ParticleGun.MaterialEval.ModP = 1 * GeV

# The particle can be generated uniformly (randomly or in a grid) in eta-phi
# rather than x-y (default is false, i.e. x-y)
ParticleGun.MaterialEval.EtaPhi = True

# The boundary limits of the x-y or eta-phi planes have to given.
# 
# The following options are for the x-y plane, to specify the position
# and size of the "target rectangle". The default plane is at z = 10*m, 
# with -3.2*m < x < 3.2*m and -2.6*m < y < -2.6*m, i.e. a little more
# than the LHCb acceptance
ParticleGun.MaterialEval.ZPlane = 600*mm
#ParticleGun.MaterialEval.Xmin   = -15*m
#ParticleGun.MaterialEval.Ymin   = -15*m
#ParticleGun.MaterialEval.Xmax   =  15*m
#ParticleGun.MaterialEval.Ymax   =  15*m

# The follwing control the parameters for the generation in eta-phi.
# The defaults are 2.1 < eta < 4.9 and phi over 2pi, i.e. a little more
# than scanning the LHCb acceptance
ParticleGun.MaterialEval.MinEta = 10.
ParticleGun.MaterialEval.MaxEta = 10.
ParticleGun.MaterialEval.MinPhi = 0.0*rad
ParticleGun.MaterialEval.MaxPhi = 0.0*rad 

# A regular grid of given steps can be used instead of a randomly uniform
# distribution (default = false)
ParticleGun.MaterialEval.UseGrid = False

# For the grid the number of steps needs to be specified differently for
# x/y or eta/phi
#
# For x/y the default number of steps is 320 in x and 260 in y , i.e. each
# step will be 2 cm with the default property for the plane
#ParticleGun.MaterialEval.NStepsInX = 320
#ParticleGun.MaterialEval.NStepsInY = 260

# For eta/phi the default number of steps is 280 in eta and 314 in phi, i.e.
# the step will be 0.1 units in eta and 0.2 rad in phi
#ParticleGun.MaterialEval.NStepsInEta = 280
#ParticleGun.MaterialEval.NStepsInPhi = 314