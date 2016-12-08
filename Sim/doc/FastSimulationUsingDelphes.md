Fast Simulation Using Delphes
==================================

Contents

-   [Overview](#overview)
-   [Introduction](#introduction)
-   [Installation Procedure](#installation-procedure)
-   [What is Delphes?](#what-is-delphes)
-   [Running Delphes in FCCSW](#running-delphes-in-fccsw)
    - [FCCSW configuration file](#fccsw-configuration-file)
    - [Delphes card](#delphes-card)
-   [FCC-EDM output](#fcc-edm-output)
-   [Other documentation](#other-documentation)

Overview
--------

This is a technical description of the Delphes interface in FCCSW. It describes the configuration file of FCCSW and properties
that allow customisation of the Delphes job. We also have dedicated tutorials to
[get started with Delphes](https://github.com/HEP-FCC/fcc-tutorials/blob/master/FccSoftwareGettingStartedFastSim.md#getting-started-with-delphes-fcc-hh) and
[run a full analysis using Delphes fast simulation](https://github.com/HEP-FCC/fcc-tutorials/blob/master/FccFullAnalysis.md).

Introduction
------------------

**A small tutorial on how to study FCC-hh benchmark channels** within
the **[FCCSW framework](https://github.com/HEP-FCC/FCCSW)** with
**[Pythia](http://home.thep.lu.se/~torbjorn/pythia81html/Welcome.html)**
generator, **[Delphes](https://cp3.irmp.ucl.ac.be/projects/delphes)** simulation, and
**[Heppy](https://github.com/cbernet/heppy)** analysis framework.

The event generation (Pythia8) and the detector simulation (Delphes) are handled by the FCCSW.
Heppy is then configured used to apply an event selection, and produce a flat ROOT tree containing
the observables that are relevant for the physics channel under study.

Installation Procedure
---------------------------

The fast simulation is run within the **FCCSW framework**, based on **Gaudi**. For installation instructions of FCCSW see
[FCCSW installation instructions](https://github.com/HEP-FCC/FCCSW/blob/master/README.md).


What is Delphes
---------------

To get familiar with the **Delphes software** , follow a [quick tour](https://cp3.irmp.ucl.ac.be/projects/delphes/wiki/WorkBook/QuickTour)
on the Delphes official web page. Let us stress that the Delphes card needs to be compatible with the Delphes software
version. Only in such case, the support for parametrization and physics object output of all Delphes modules can be
guaranteed!


Description of the Workflow with Delphes in FCCSW
-------------------------------------------------

-   The Pythia input is defined through [LHE](https://en.wikipedia.org/wiki/Les_Houches_Accords) (Les Houches Event)
    data file. Additionally Pythia is configured through a config file (details see below).

-   In FCCSW the Pythia output is propagated to Delphes with the [HepMC](http://lcgapp.cern.ch/project/simu/HepMC/)
    event data format.

-   We can either use Pythia and Delphes in the same job or alternatively read a
    [HepMC](http://lcgapp.cern.ch/project/simu/HepMC/) data file.

-   Each event is passed to Delphes which takes care of the simulation and outputs collections and relations of various
    physics objects.

-   The Delphes output objects (collections, relations) are translated into the **[FCC-EDM](https://github.com/HEP-FCC/fcc-edm/blob/master/README.md)**
    (FCC event-data model) and written into a ROOT file. Thus, can be then easily processed by ROOT macros.

-   In addition, for backwards compatibility and/or testing purposes the Delphes output is also supported. The output is
    a ROOT tree, using standard Delphes ROOT tree writer.

Program workflow:

![PythiaDelphes\_ZDrasal\_Scheme.jpg](https://twiki.cern.ch/twiki/pub/FCC/FccPythiaDelphes/PythiaDelphes_ZDrasal_Scheme.jpg "PythiaDelphes_ZDrasal_Scheme.jpg")


Running Delphes in FCCSW
------------------------

The example can be run with:

```
./run gaudirun.py Sim/SimDelphesInterface/options/PythiaDelphes_config.py
```

After the job finishes you should see a file `FCCDelphesOutput.root`. Details about the contents of this file may be found
[below](#fcc-edm-output). Two configurations govern what actually happens in the generation and fast simulation:

1. FCCSW python configuration (that schedules Pythia and Delphes)
2. The Delphes card

> Details about the Pythia configuration, can be found [here](../../Generation/doc/RunningPythia.md).


### FCCSW configuration file

The FCCSW configuration file (python script) to run Pythia & Delphes in one job:

**[PythiaDelphes\_config.py](https://github.com/HEP-FCC/FCCSW/blob/master/Sim/SimDelphesInterface/options/PythiaDelphes_config.py)** -
Parameters of interest:

-   `nEvents` --&gt; events to be simulated
-   `messageLevel` --&gt; GAUDI messaging
    verbosity: ERROR, WARNING, INFO, DEBUG, ... --&gt; use WARNING
    or INFO
-   `pythiaconfFile` --&gt; Pythia
    configuration file (use either Pythia\_LHEinput.cmd
    or Pythia\_standard.cmd)
-   `delphesCard` --&gt; Delphes
    configuration file (use official Delphes card)
-   `delphesHepMCInFile` --&gt; Delphes
    input file (use "" to read <span class="twikiNewLink"> HepMC
    </span> directly from Pythia, i.e. from transient data store)
-   `delphesRootOutFile` --&gt; Delphes
    output file (use "" to avoid extra output with Delphes objects
    to ROOT file, FCC-EDM objects automatically written out!)
-   `delphesXXXOutArray` --&gt; Define
    Delphes output arrays to be processed as FCC-EDM XXX particles
    (muons, electrons, etc.) --&gt; various Delphes modules provide
    the same type of particle with different level of processing.
    (For better understanding, watch the content of the Delphes card
    and search e.g. for muons to see how individual Delphes modules
    work out muon particles. Focus on
    `OutputArray` variable.)

### Delphes card

Some cards are located in the
[Sim/SimDelphesInterface/data](https://github.com/HEP-FCC/FCCSW/tree/master/Sim/SimDelphesInterface/data)
directory:

-   **[FCChh\_DelphesCard\_Baseline\_v01.tcl](https://github.com/HEP-FCC/FCCSW/blob/master/Sim/SimDelphesInterface/data/FCChh_DelphesCard_Baseline_v01.tcl)**:
    Official FCChh Delphes configuration card, defining sequence of Delphes modules to be executed and detector characterization.

(more to come)

FCC-EDM output
--------------

The following collections and relations will be saved in the output
file. Each collection (relation) is handled in the FCCSW through a
dedicated handle. To get an idea on the EDM, have a look at the [yaml](https://github.com/HEP-FCC/fcc-edm/blob/master/edm.yaml) file.
For reference, the handle name (specified in a
[DelphesPythia\_config.py](https://github.com/HEP-FCC/FCCSW/blob/master/Sim/SimDelphesInterface/options/PythiaDelphes_config.py)
file) is given in the brackets \[\] below:

-   **Collections:**
    -   `fcc::MCParticleCollection`:  generated particles `[genParticles]`
    -   `fcc::GenVertexCollection`: generated vertices `[genVertices]`
    -   `fcc::GenJetCollection`: generated jets `[genJets]`
    -   `fcc::ParticleCollection`: reconstructed muons `[muons]`, electrons `[electrons]`, charged particles `[charged]`,
        neutral particles `[neutral]`, photons `[photons]` and jet constituents `[jetParts]`
    -   `fcc::JetCollection`: reconstructed jets `[jets]`
    -   `fcc::METCollection`: reconstructed missing Et `[met]`
    -   `fcc::TaggedJetCollection`: collection of reconstructed tagged jets - b-tags, c-tags and tau-tags, holds a relation to the original jet
        `[bTags, cTags, tauTags]`
    -   `fcc::TaggedParticleCollection`: collection reconstructed isolation tag info for electrons, muons and
        photons, holds a relation to the original particle `[muonITags, electronITags, photonITags]`

-   **Relations:**
    -   `fcc::ParticleMCParticleAssociationCollection`: relations of reconstructed object to MC particle for
        muons `[muonsToMC]` , electrons `[electronsToMC]`, charged particles `[chargedToMC]` , neutral particles
        `[neutralToMC]` and photons `[photonsToMC]`


***

Other documentation
-------------------

Tutorials on using Delphes for FCC-hh:

- [Getting started with Delphes (FCC-hh)](https://github.com/HEP-FCC/fcc-tutorials/blob/master/FccSoftwareGettingStartedFastSim.md#getting-started-with-delphes-fcc-hh)
- [A FCC Pythia + Delphes + Heppy Analysis](https://github.com/HEP-FCC/fcc-tutorials/blob/master/FccFullAnalysis.md)


Some presentations on this topic may be found here:

-   [Talk](https://indico.cern.ch/event/446602/contribution/6/attachments/1238094/1818990/PythiaDelphes_ZDrasal.pdf)
    (FCC-hh group) on Mar 3rd 2016
-   [Talk](https://indico.cern.ch/event/443015/contribution/2/attachments/1155286/1660371/PythiaDelphes_ZDrasal.pdf)
    (FCC-hh group) on Sep 15th 2015
-   [Talk](https://indico.cern.ch/event/399484/contribution/0/attachments/800169/1096662/FCCSW_MDG_040615.pdf)
    (FCC software group) on Jun 4th 2015
-   [Talk](https://indico.cern.ch/event/370378/contribution/2/attachments/736634/1010596/FCCSW_MDG_050215.pdf)
    (FCC sofware group) on Feb 5th 2015

***

