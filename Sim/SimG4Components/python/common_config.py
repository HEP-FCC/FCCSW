
from Gaudi.Configuration import *
from Configurables import GenAlg
## Particle Gun using MomentumRangeParticleGun tool and FlatSmearVertex
# MomentumRangeParticleGun generates particles of given type(s) within given momentum, phi and theta range
# FlatSmearVertex smears the vertex with uniform distribution
gen = GenAlg("ParticleGun", SignalProvider="MomentumRangeParticleGun", VertexSmearingTool="FlatSmearVertex")
gen.hepmc.Path = "hepmc"

from Configurables import Gaudi__ParticlePropertySvc
## Particle service
# list of possible particles is defined in ParticlePropertiesFile
ppservice = Gaudi__ParticlePropertySvc("ParticlePropertySvc", ParticlePropertiesFile="Generation/data/ParticleTable.txt")

from Configurables import HepMCToEDMConverter
## Reads an HepMC::GenEvent from the data service and writes a collection of EDM Particles
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

mc_particle_algs = [gen, hepmc_converter]
mc_particle_svcs = [ppservice]
