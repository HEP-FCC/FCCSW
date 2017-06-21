Fast Simulation Using Papas
==================================

Contents

-   [Overview](#overview)
-   [Introduction](#introduction)
-   [Installation Procedure](#installation-procedure)
-   [What is Papas?](#what-is-papas?)
-   [Running Papas in FCCSW](#running-papas-in-fccsw)

Overview
--------

This is a  description of the Papas interface in FCCSW. It describes the configuration file of FCCSW and properties
that allow customisation of Papas.

The event generation (Pythia8) and the Papas simulation are handled by the FCCSW.
Heppy analyses may then be run on the output Root file.

Installation Procedure
---------------------------

Papas fast simulation is run within the **FCCSW framework**, based on **Gaudi**. For installation instructions of FCCSW see
[FCCSW installation instructions](https://github.com/HEP-FCC/FCCSW/blob/master/README.md).


What is Papas?
---------------

PAPAS (PArametrized PArticle Simulation) provides a simulation and particle reconstruction algorithm which is designed to allow users to test the performance of detector design. The approach uses fast simulation/ reconstruction. An implementation of the CMS detector is provided as a starting point. For other detectors, users must provide simple C++ classes to define key detector parameters eg tracker acceptance and calorimeter radius and length.

PAPAS propagates stable generated particles through a simple detector model.

In the tracker, charged particles may be detected as tracks, taking into account the acceptance, efficiency, and momentum resolution of this detector.

In the calorimeters, particles are detected as energy deposits. The energy deposits are modelled by taking into account the following detector properties: energy resolution, acceptance, energy thresholds and characteristic size. The latter is defined as the distance between two clusters below which the two clusters cannot be resolved and are considered as a single cluster.

A particle flow algorithm then runs over the simulated tracks and clusters to identify and reconstruct charged hadrons, photons, and neutral hadrons. These particles can then be used in Heppy as an input to higher-level algorithms like jet clustering, or directly in the analysis.

TIP Electrons and muons are passed through PAPAS without any modification, and the user is responsible for applying their own efficiency and resolution models. The hadronic decay products of tau leptons are simulated just like other hadrons and photons.

A CMS-like detector model is provided as an example.


Running Papas in FCCSW
-------------------------------------------------

The input is a file of fcc generated particles eg using Pythia see [tutorial](https://github.com/HEP-FCC/fcc-tutorials/blob/master/FccSoftwareGettingStartedFastSim.md#generate-events-with-pythia8)

The Gaudi interface to Papas, PapasAlg provides following configurable set tools
 * PapasImportParticlesTool - reads in gen particles
 * PapasSimulatorTool  - simulates smeared clusters and tracks
 * PapasMergeClustersTool - merges clusters
 * PapasBuildBlocksTool - builds blocks of linked clusters and tracks
 * PapasSimplifyBlocksTool - simplifies the blocks
 * PapasPFReconstructorTool - reconstructs particles based on the blocks
 * PapasExportParticlesTool - exports papas reconstructed particles to fcc particles

Papas uses a Papas::Event to store the collections of particles, clusters, tracks, blocks that it produces. The Papas::Event is passed to each of the tools and new collections are then added into the papas::Event. Thus subsequent tools can access previously made collections.
  Once a collection has been added to the event it can no longer be changed.

 The papas::Event uses a short hand to label the collections that it contains:
   the code is a two letter string eg "es"

the first letter is the type of object contained in the collection

```
         e = ecal
         h = hcal
         t = track
         p = particle
         b = block
```

the second letter indicates a subtype eg

```
         s = simulated (for particles), smeared (for clusters/tracks) or simplified (for blocks)         m = merged
         r = reconstructed (particles) or raw (blocks)
```

 tool arguments are either a two letter TypeAndSubtype, eg "es" ecal smeared
  or a single letter type specific subtype, eg particleSubtype="s"

for example:

```
papasmergeecaltool = PapasMergeClustersTool("ecalmerge", TypeAndSubtype="es")
```

would merge clusters of type 'es' = ecal smeared (and produce an output collection of type "em")



Running PAPAS in FCCSW
------------------------

An annotated example of a Gaudi options file to run Papas can be found here:-
 https://github.com/HEP-FCC/FCCSW/tree/master/Sim/doc
(link to be refined once pull request is merged)
The example can be run as follows:

Generate some particles

```
wget https://raw.githubusercontent.com/HEP-FCC/fcc-physics/master/pythia8/ee_ZH_Zmumu_Hbb.txt
```


Generate ee to ZH events with Z to mumu and H to b bbar:

```
fcc-pythia8-generate ee_ZH_Zmumu_Hbb.txt
```

Run

```
./run gaudirun.py $FCCSWPATH/Sim/SimPapas/options/simple_papastool.py
```

Verify that you have produced some particles in ROOT:

```
root output.root
events->Print()
events->Draw("papasreconstructed.core.p4.px", "papasreconstructed.core.status==1")
```



