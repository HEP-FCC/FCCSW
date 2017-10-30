## Configuratin for Papas Gaudi components
## Runs papas using as a sequence of tools

from Gaudi.Configuration import *

from Configurables import PapasAlg, PapasImportParticlesTool
from Configurables import PapasSimulatorTool, PapasMergeClustersTool, PapasBuildBlocksTool
from Configurables import PapasSimplifyBlocksTool, PapasPFReconstructorTool, PapasExportParticlesTool

#Notes:
#
# The PapasAlg allows a configurable set of tools to be run (see below).
# The Papas::Event is passed to each of the tools and new collections are then added into the papas::Event.
# Thus subsequent tools can access previously made collections.
# The papas::Event uses a short hand to label the collections that it contains:
# see https://github.com/alicerobson/FCCSW/blob/papas0.8.1/Sim/doc/FastSimulationUsingPapas.md
# TODO update this link when pull request has gone through
#read in pythia generated particles, write out reconstructed particles (fcc EDM format)
papasalg = PapasAlg("papasalg",
                         tools=[
                                "PapasSimulatorTool/papassim", #simulates smeared clusters and tracks
                                "PapasMergeClustersTool/ecalmerge", #merges clusters ECAL
                                "PapasMergeClustersTool/hcalmerge", #merged clusters HCAL
                                "PapasBuildBlocksTool/blockbuilder", #build blocks of linked clusters and tracks
                                "PapasSimplifyBlocksTool/blocksimplifier", #simplifies the blocks
                                "PapasPFReconstructorTool/reconstructor"], #reconstructs particles based on the blocks
                         importTool ="PapasImportParticlesTool/importer", #reads in gen_particles and creates papas particles.
                         exportTool ="PapasExportParticlesTool/exporter", #export papas reconstructed particles to fcc particles
                         seed = 0xdeadbeef,#seed random generator
                         physicsDebugFile = "", #no phyics debug output
                         detService = "PapasDetSvc", #name of detector service
                         )

papasalg_pdebug = PapasAlg("papasalg_pdebug",
                    tools=[
                           "PapasSimulatorTool/papassim", #simulates smeared clusters and tracks
                           "PapasMergeClustersTool/ecalmerge", #merges clusters ECAL
                           "PapasMergeClustersTool/hcalmerge", #merged clusters HCAL
                           "PapasBuildBlocksTool/blockbuilder", #build blocks of linked clusters and tracks
                           "PapasSimplifyBlocksTool/blocksimplifier", #simplifies the blocks
                           "PapasPFReconstructorTool/reconstructor"], #reconstructs particles based on the blocks
                    importTool ="PapasImportParticlesTool/importer", #reads in gen_particles and creates papas particles.
                    exportTool ="PapasExportParticlesTool/exporter", #export papas reconstructed particles to fcc particles
                    seed = 0xdeadbeef,#seed random generator
                    physicsDebugFile = 'papasPhysics.txt', #write out papas physics to file
                    detService = "PapasDetSvc", #name of detector service
                    )

#Papas importer
importer = PapasImportParticlesTool("importer")
importer.genparticles.Path='GenParticle' #name of the input pythia particles collection

#Papas simulation
#reads in papas particles and simulated true and smeared cluster and tracks
papassimtool = PapasSimulatorTool("papassim", particleSubtype="s")
#Papas Merge Clusters
#takes smeared ecal and hcal clusters and merges overlapping clusters
papasmergeecaltool = PapasMergeClustersTool("ecalmerge", TypeAndSubtype="es") #run merge on clusters of type "es" = ecal smeared

papasmergehcaltool = PapasMergeClustersTool("hcalmerge", TypeAndSubtype="hs") #run merge on clusters of type "hs" = hcal smeared

#Papas Construct Blocks of connected clusters and tracks
papasblockbuildertool = PapasBuildBlocksTool("blockbuilder",
                                             ecalSubtype="m", #use merged ecal clusters collection
                                             hcalSubtype="m", #use merged ecal clusters collection
                                             trackSubtype="s") #use smeared tracks collection
#Papas simplify the blocks structures
papasblocksimplifiertool = PapasSimplifyBlocksTool("blocksimplifier", blockSubtype="r") #use the reconstucted blocks from previous step

#Papas Reconstruct particles from blocks
papaspfreconstructortool = PapasPFReconstructorTool("reconstructor", blockSubtype ="s") #use blocks collection from block simplifier

#export reconstructed particles to root file
papasexportparticlestool = PapasExportParticlesTool("exporter", particleSubtype="r") #use the reconstructed particles as input
papasexportparticlestool.recparticles.Path='RecParticle' #name of output reconstructed particles in fcc format
papasexportparticlestool.particleMCparticleAssociations.Path='ParticleLinks' #link from gen to reconstructed particles
papasexportparticlestool.genparticles.Path='GenParticle'


