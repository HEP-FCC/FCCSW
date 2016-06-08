Full Simulation with Geant4 in FCCSW
====

Instruction how to use Geant4 within FCCSW (GAUDI framework).

For additional information on fast simulation in Geant4 [see](Geant4fastsim.md).

> All relative paths refer to the main FCCSW directory.

## Contents
1. [Overview](#1-overview)
   * [Geant](#11-geant-components)
   * [Sim in FCCSW](#12-simulation-package-in-fccsw)
2. [Example of full sim configuration](#2-example)
3. [Geant configuration via GAUDI service `G4SimSvc`](#3-geant-configuration-via-gaudi-service-g4simsvc)
  * [Geometry construction](#31-geometry-construction)
  * [Physics list](#32-physics-list)
  * [User actions](#33-user-actions)
4. [Simulation in GAUDI algorithm `G4SimAlg`](#4-simulation-in-gaudi-algorithm-g4simalg)
  * [Events](#41-event-processing)
  * [Output](#42-output)
5. [Units](#5-units)


## How to
* [change the geometry](#how-to-use-different-geometry)
* [use sensitive detectors](#sensitive-detectors)
* [change the physics list](#how-to-use-different-physics-list)
* [add user action](#how-to-add-user-action)
* [use particle gun (single particle events)](#how-to-use-particle-gun)

[DD4hep]: http://aidasoft.web.cern.ch/DD4hep "DD4hep user manuals"
[DD4hep manual]: http://www.cern.ch/frankm/DD4hep/DD4hepManual.pdf "DD4hep manual"

## 1. Overview

### 1.1. Geant components

Geant main manager class is `G4RunManager`. It has own implementation in FCCSW `sim::RunManager` as the event flow is governed by GAUDI and not by Geant.

#### Main service `G4SimSvc` ([see more](#3-geant-configuration-via-gaudi-service-g4simsvc))

The main simulation service `G4SimSvc` owns `sim::RunManager` and controls the communication between GAUDI and Geant (`sim::RunManager`).

Necessary information about the simulation that needs to be given:
* detector geometry (`IG4DetectorConstruction` tool)
* physics list describing all the particles that can be created in the simulation and all the processes they may encounter (`IG4PhysicsList` tool)
* additional requirements (so-called user actions) (`IG4ActionTool`)

This service is also passing events (`G4Event`) to and from Geant.

#### Main algorithm `G4SimAlg` ([see more](#4-simulation-in-gaudi-algorithm-g4simalg))

The main simulation algorithm communicates with `G4SimSvc` in each event loop execution.
It is responsible for the translation of the EDM event (`MCParticleCollection`) to `G4Event`, passing it to be simulated and retrieving it afterwards.

Retrieved `G4Event` contains the very same primary particles and vertices, though it also contains hits collections and [various information](#4-simulation-in-gaudi-algorithm-g4simalg). To enable a flexible setting of what should be saved from an event, it may be specified in a tool derived from `IG4SaveOutputTool`.
A property **outputs** of `G4SimAlg` takes a list of strings with those tool names.
Those tools should declare the output that is supposed to be further stored by the algorithm `PodioOutput`.

### 1.2. Simulation package in FCCSW

Simulation package contains following directories:

* *SimG4Interfaces*
  * interfaces to tools, algorithms and services;
* *SimG4Components*
  * all main components (e.g. `SimG4Svc`, `G4DD4hepDetector`)
  * tests, further examples (e.g. `Sim/SimG4Components/tests/geant_fullsim_hcal.py`)
* *SimG4Common*
  * Geant classes' implementations, common for both the full and fast simuliation (e.g. `sim::RunManager`)
* *SimG4Full*
  * components and Geant classes' implementations used in the full simulation (e.g. `G4FullSimActions`)
* *SimG4Fast*
  * components and Geant classes' implementations used in the fast simulation (e.g. `sim::FastSimPhysics`)


## 2. Example

~~~{.sh}
./run gaudirun.py options/geant_fullsim.py
~~~

The configuration file (`options/geant_fullsim.py`) contains:
  * reading an event from a HepMC file

    ~~~{.py}
    from Configurables import HepMCReader
    reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
    reader.DataOutputs.hepmc.Path = "hepmc"
    ~~~

  * translating a HepMC event to the EDM

    ~~~{.py}
    from Configurables import HepMCConverter
    hepmc_converter = HepMCConverter("Converter")
    hepmc_converter.DataInputs.hepmc.Path="hepmc"
    hepmc_converter.DataOutputs.genparticles.Path="all_genparticles"
    hepmc_converter.DataOutputs.genvertices.Path="all_genvertices"
    ~~~

  * construction of the geometry using DD4hep
    - `detector` - path to the XML file with geometry

    ~~~{.py}
    from Configurables import GeoSvc
    geoservice = GeoSvc("GeoSvc",
                         detector='file:DetectorDescription/Detectors/compact/TestTracker.xml',
                         OutputLevel = VERBOSE)
    ~~~

  * Geant configuration ([see more](#3-geant-configuration-via-gaudi-service-g4simsvc))
    - **detector** - tool providing the [geometry](#31-geometry-construction), possible: G4DD4hepDetector and [G4GdmlDetector](#gdml-example)
    - **physicslist** - tool providing the [physics list](#32-physics-list), possible: [G4FtfpBert](#)
    - **actions** - tool providing the [user actions initialisation list](#33-user-actions), possible: G4FullSimActions
    ~~~{.py}
    from Configurables import G4SimSvc
    geantservice = G4SimSvc("G4SimSvc",
                             detector='G4DD4hepDetector',
                             physicslist="G4FtfpBert",
                             actions="G4FullSimActions" )
    ~~~


  * simulation ([see more](#4-simulation-in-gaudi-algorithm-g4simalg))
    - `outputs` - names of the tools saving the [output](#4-2-output) from a simulated event,
      possible: G4SaveTrackerHits, G4SaveCalHits

    ~~~{.py}
    from Configurables import G4SimAlg, G4SaveTrackerHits
    savetrackertool = G4SaveTrackerHits("G4SaveTrackerHits")
    savetrackertool.DataOutputs.trackClusters.Path = "clusters"
    savetrackertool.DataOutputs.trackHits.Path = "hits"
    savetrackertool.DataOutputs.trackHitsClusters.Path = "hitClusterAssociation"
    geantsim = G4SimAlg("G4SimAlg",
                         outputs= ["G4SaveTrackerHits/G4SaveTrackerHits"])
    geantsim.DataInputs.genParticles.Path="allGenParticles"
    ~~~

  * saving the output to ROOT file

    ~~~{.py}
    from Configurables import PodioOutput
    out = PodioOutput("out",
                       OutputLevel=DEBUG)
    out.outputCommands = ["keep *"]
    ~~~

  * GAUDI's main component
      - stating which algorithms (`TopAlg`) should be initialised, executed and finalised in the event loop
      - specifying how many events from the input file should be processed (`EvtMax`)
      - stating which services (`ExtSvc`) should be created at initialisation
      - output level (`OutputLevel`), possible: VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL, ALWAYS

    ~~~{.py}
    from Configurables import ApplicationMgr
    ApplicationMgr( TopAlg = [reader, hepmc_converter, geantsim, out],
    EvtSel = 'NONE',
    EvtMax   = 1,
    ExtSvc = [podioevent, geoservice, geantservice], # order! geo needed by geant
    OutputLevel = DEBUG
    )
    ~~~

#### HCal example
Another example: user can create a hadronic calorimeter using DD4hep and save the hits:

~~~{.sh}
./run gaudirun.py Sim/SimG4Components/tests/geant_fullsim_hcal.py
~~~

#### GDML example
or create a detector using GDML. In this case, however, no sensitive detectors are predefined and user is responsible for their implementation. Preferable hit format is the one used by DD4hep (e.g. `DD4hep::Simulation::Geant4CalorimeterHit`), this way user may use the saving output utilities that are already provided.

~~~{.sh}
./run gaudirun.py Sim/SimG4Components/tests/geant_fullsim_gdml.py
~~~


3. Geant configuration: via GAUDI service G4SimSvc
----

Main service for simulation with Geant4 `G4SimSvc` owns `sim::RunManager` which derives from `G4RunManager`. Own implementation of G4RunManager was necessary in order to leave the event flow to GAUDI. All the details that does not concern the particle generator remain the same. Consult Geant4 [User's Guide](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForToolkitDeveloper/html/index.html) or [Physics Manual](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/PhysicsReferenceManual/fo/PhysicsReferenceManual.pdf) for more details.

In particular, there are two basic ingredients of the simulation: geometry that describes what is the material in which particle is propagated, and a so-called physics list that contains all the particles that may be created in the simulation together with an associated list of physics processes they may encounter.

The general concept of the simulation in Geant is to divide each simulation 'stage' into smaller pieces until the smallest (and simplest) is reached. The most overall is **run** which is a set of **events**. Each event consists of primary particles that are represented by **tracks**. Each track in a simulation is a **step**-by-step analysis of a particle passage through matter. Geant toolkit leaves a user the possibility to monitor the simulation at any of those stages. Hence, the so-called user actions may be created that undertake user-specified action before/at/after certain stage.

Any communication with the Geant Run Manager is handled by this service:
- configuration:
  - [geometry construction](#31-geometry-construction)
  - [physics list initialisation](#32-physics-list)
  - [user actions initialisation](#33-user-actions)
- simulation:
  - [passing an event to `G4EventManager`](#41-event-processing)
  - [retrieving a simulated event (with hits collections and other information)](#42-output)


### 3.1. Geometry construction

FCCSW is using Detector Description for HEP (DD4hep) toolkit. Consult [user guides][DD4hep] for more details.

Geometry description requires the C++ factory methods of sub-detectors, see:
~~~(.sh)
DetectorDescription/Detectors/src/
~~~
as well as the XML configuration of the detector setup, including the dimensions, names, materials etc., see:
~~~(.sh)
DetectorDescription/Detectors/compact
~~~

Main service responsible for handling the DD4hep setup is `GeoSvc`. It has a property **detector** that takes a string with a path to the detector description XML file.

DD4hep is able to parse automatically the geometry and convert it to Geant4 format. It can be retrieved and passed to the Geant configuration service via tool `G4DD4hepDetector`. User needs to set the geometry tool in `G4SimSvc` (property **detector**) to `G4DD4hepDetector` (TODO: set it as a default case).

In FCCSW there is an alternative way to create the geometry, via GDML description (and tool `G4GdmlDetector` with property **gdml** taking a path to the GDML file). It is meant only for the test purposes as it does not support sensitive detectors. User would need to create them on his own.

### Sensitive detectors

Sensitive detectors are responsible for creating the hits whenever a particle traverses the active material. The segmentation of each detector can be described in DD4hep. If a user builds a detector containing only tracker(s) or calorimeter(s), a proper description of the detector in DD4hep is sufficient and there is no need to specify any sensitive detector in Geant4.

##### DD4hep (see more in sec. 2.11 and 2.12 of [DD4hep manual])
* XML file should contain:
  * readout structure, a structure of a sensitive volume `<readout>`
  * information on how each sensitive element is segmented `<segmentation>`
  * unique identification of the location of each response `<id>`

  ~~~{.xml}
  <readouts>
    <readout name="BarHCal_Readout">
      <segmentation type="CartesianGridXY" grid_size_x="0.05*mm" grid_size_y="0.05*mm"/>
      <id>system:3,layer:10,wedge:7,sub_module:2,row:9,x:32:-16,y:-16</id>
    </readout>
  </readouts>
  ~~~
  > #### Note:
  > Names of the readouts structures are used further in the implementations if IG4SaveOutputTool, hence they should contain the name of the detector type they represent: "Tracker", "ECal" (or "EMCal"), "HCal".

* C++ factory method should:
  * actually construct all the modules of the detector (extracting detail information e.g. on number of modules in z direction or in phi, their material, etc. from the XML file)
  * set the sensitive detector type to either **Geant4Tracker** or **Geant4Calorimeter**.
    This way hits can be stored in either `DD4hep::Simulation::Geant4TrackerHit` or `DD4hep::Simulation::Geant4CalorimeterHit`

    ~~~{.cpp}
    DD4hep::Geometry::SensitiveDetector::setType("Geant4Calorimeter");
    ~~~

    Furthermore, that will automatically create the hits collections that are stored in `G4HCofThisEvent` and can be accessed from `G4Event` after the simulation is done (how to access the event is described [further](#42-output)). Names of the collections are determined by the name of readout, set in XML file.


### How to use different geometry

In order to describe a detector, one needs an XML file and relevant C++ factory methods.
To describe a sensitive detector consult the [short description](#sensitive-detectors) and sections 2.11 and 2.12 in [DD4hep manual].
Further information may be also found on [DD4hep documentation webpage][DD4hep].

Once XML file is done, it can be passed as a property **detector** to `GeoSvc` which will parse it and translate the geometry to Geant format.
___


### 3.2. Physics List

Physics list describes all the particles and physics processes used in the simulation.

Physics list tool needs to be set as **physicslist** property of `G4SimSvc`.

The currently used physics list is FTFP_BERT, which is recommended by Geant4 for HEP. The tool creating this list is called `G4FtfpBert`. It simply creates `G4VModularPhysicsList`.

List of other reference physics list may be found [here](http://geant4.cern.ch/geant4/support/proc_mod_catalog/physics_lists/referencePL.shtml)

### How to use different physics list
In order to use a different physics list, one need to construct a physics list tool basing on `G4FtfpBert` tool.

Any new implementation of a physics list (or any other component) should be presented in a pull request to HEP-FCC/FCCSW for any other person to use it.
___


### 3.3. User Actions

User actions tool needs to be added as a property **actions** to `G4SimSvc`.

Geant allows users to specify what should be performed at any stage of simulation.
User actions are created in the implementation of `GVUserActionInitialization` class, e.g. `FullSimActions::Build()`.
Any implementation of action initialisation list should have a relevant GAUDI component (tool) that creates it. Tool creating `FullSimActions` is called `G4FullSimActions`.

Currently `FullSimActions` is empty and no user actions are created.

User actions may derive from the following G4 interfaces:
* G4UserRunAction.hh
* G4UserEventAction.hh
* G4UserStackingAction.hh
* G4UserTrackingAction.hh
* G4UserSteppingAction.hh
* G4UserTimeStepAction.hh


### How to add user action

Any user action that derives from Geant4 interface can be implemented in Sim/SimG4Full.

In order to invoke this action in the simulation, it should be created in the `FullSimActions::Build()` method.

However, if one uses interchangeably different 'sets' of user actions and do not want to recompile FCCSW with every change (addition/deletion) of user action in `FullSimActions`, it is possible to create another implementation(s) of `G4VUserActionInitialization`.
In that case, a relevant GAUDI tool should be created, basing on `G4FullSimActions`. Its name should follow the convention of adding a prefix "G4" to the name of the class (implementation of `G4VUserActionInitialization` that it creates.
___

## 4. Simulation in GAUDI algorithm G4SimAlg

Simulation algorithm handles all the communication between other algorithms and `G4SimSvc`.

It takes as input **genParticles** the EDM `MCParticleCollection`.

Also, a list of names to the output-saving tools can be specified in **outputs**.


### 4.1. Event Processing

For each execution of the algorithm an EDM `MCParticleCollection` is translated into `G4Event` using the method `G4SimAlg::EDM2G4()`. At the translation time, for each `G4PrimaryParticle` `sim::ParticleInformation` is created with a reference to the EDM's `MCParticle`. This can be used further e.g. in the saving output tool.

A translated `G4Event` is passed to `G4SimSvc` and after the simulation is done, it is retrieved. Here all (if any) saving tools are called. Finally, an event is terminated.


### 4.2. Output

Saving the output from a simulated `G4Event` is performed by tools deriving from an interface `IG4SaveOutputTool`.
Tools may have the data outputs specified.
A method `IG4SaveOutputTool::SaveOutput(const G4Event &aEvent)` is meant to retrieve any useful information and save it to EDM.
Useful information means e.g. hits collections (`G4HCofThisEvent`) or anything stored in an implementation of `G4VUserEventInformation`, `G4VUserEventInformation`, `G4VUserTrackInformation`, `G4VUserPrimaryParticleInformation` etc.

Existing tools store hits collections from the tracker detectors (`G4SaveTrackerHits`) or calorimeters (`G4SaveCalHits`).

`G4SaveTrackerHits` stores **trackClusters** (EDM `TrackClusterCollection`), **trackHits** (EDM `TrackHitCollection`) and **trackHitsClusters** (EDM `TrackClusterHitsAssociationCollection`) for any hit collection with "Tracker" in its name. Collection name is specified by the readout name at the XML file. For instance, the collection below contains hits in the tracker ("CentralTracker_Readout") ([see more](#sensitive-detectors)):

~~~{.xml}
<readouts>
  <readout name="CentralTracker_Readout">
  ...
  </readout>
</readouts>
~~~

`G4SaveCalHits` tool is more general in a sense that it should describe both hits collections in the electromagetic and hadronic calorimeters. Hence, it takes a property **caloType** that can be either "ECal" (or "EMCal") or "HCal". It stores **caloClusters** (EDM `CaloClusterCollection`) and **caloHits** (EDM `CaloHitCollection`) for any hit collection with name that contains the string defined in **caloType** property.

5. Units
-----

Important aspect of the translations between HepMC, EDM and Geant4 are the units. Since each framework uses by default different units, every translation should take that into account.

Conversions between EDM and Geant4 are specified in `sim::Units`.

Conversions between EDM and HepMC depend on the units used by HepMC input file (changes from file to file) and is therefore handled separately in HepMCConverter class.
