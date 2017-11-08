Full Simulation with Geant4 in FCCSW
==

Instruction how to use Geant4 within FCCSW (GAUDI framework).

For additional information on fast simulation in Geant4 [see](FastSimulationUsingGeant.md).

> All relative paths refer to the main FCCSW directory.

## Contents
[Overview](#overview)
  * [Geant](#geant-components)
  * [Sim package in FCCSW](#simulation-package-in-fccsw)
[Example of full sim configuration](#example)
[Geant configuration via GAUDI service `SimG4Svc`](#geant-configuration-via-gaudi-service-simg4svc)
  * [Geometry construction](#geometry-construction)
  * [Physics list](#physics-list)
  * [User actions](#user-actions)
[Simulation in GAUDI algorithm `SimG4Alg`](#simulation-in-gaudi-algorithm-simg4alg)
  * [Events](#event-processing)
  * [Output](#output)
[Units](#units)


## How to
* [use sensitive detectors](#sensitive-detectors)
* [change the physics list](#how-to-use-different-physics-list)
* [add user action](#how-to-add-a-user-action)
* [use fast simulation](FastSimulationUsingGeant.md)

[DD4hep]: http://aidasoft.web.cern.ch/DD4hep "DD4hep user manuals"
[DD4hep manual]: http://www.cern.ch/frankm/DD4hep/DD4hepManual.pdf "DD4hep manual"

## Overview

### Geant components

Geant main manager class is `G4RunManager`. It has own implementation in FCCSW `sim::RunManager` as the event flow is governed by GAUDI and not by Geant.

#### Main service `SimG4Svc` ([see more](#geant-configuration-via-gaudi-service-simg4svc))

The main simulation service `SimG4Svc` owns `sim::RunManager` and controls the communication between GAUDI and Geant (`sim::RunManager`).

Necessary information about the simulation that needs to be given:
* detector geometry (`ISimG4DetectorConstruction` tool)
* physics list describing all the particles that can be created in the simulation and all the processes they may encounter (`ISimG4PhysicsList` tool)
* additional requirements (so-called user actions) (`ISimG4ActionTool`)

This service is also passing events (`G4Event`) to and from Geant.

#### Main algorithm `SimG4Alg` ([see more](#simulation-in-gaudi-algorithm-simg4alg))

The main simulation algorithm communicates with `SimG4Svc` in each event loop execution.
It is responsible for creation of a `G4Event` (by default: translation from the EDM event), passing it to the service for the simulation and retrieving it afterwards.

Retrieved `G4Event` contains the very same primary particles and vertices, though it also contains hits collections and [various information](#simulation-in-gaudi-algorithm-simg4alg). To enable a flexible setting of what should be saved from an event, it may be specified in a tool derived from `ISimG4SaveOutputTool`.
A property **outputs** of `SimG4Alg` takes a list of strings with those tool names.
Those tools should declare the output that can be further stored by the algorithm `PodioOutput`.

### Simulation package in FCCSW

Simulation package contains following directories:

* *SimG4Interfaces*
  * interfaces to tools, algorithms and services;
* *SimG4Components*
  * all main components (e.g. `SimG4Svc`, `SimG4DD4hepDetector`)
  * tests, further examples (e.g. `Sim/SimG4Components/tests/geant_fullsim_hcal.py`)
* *SimG4Common*
  * Geant classes' implementations, common for both the full and fast simulation (e.g. `sim::RunManager`)
* *SimG4Full*
  * components and Geant classes' implementations used in the full simulation (e.g. `SimG4FullSimActions`)
* *SimG4Fast*
  * components and Geant classes' implementations used in the fast simulation (e.g. `sim::FastSimPhysics`)


## Example

~~~{.sh}
./run fccrun.py Examples/options/geant_fullsim.py
~~~

The configuration file (`Examples/options/geant_fullsim.py`) contains:
  * reading an event from a HepMC file

    ~~~{.py}
    from Configurables import HepMCFileReader, GenAlg
    readertool = HepMCFileReader("ReaderTool", Filename="/eos/project/f/fccsw-web/testsamples/FCC_minbias_100TeV.dat")
    reader = GenAlg("Reader", SignalProvider=readertool)
    reader.hepmc.Path = "hepmc"
    ~~~

  * translating a HepMC event to the EDM

    ~~~{.py}
    from Configurables import HepMCConverter
    hepmc_converter = HepMCConverter("Converter")
    hepmc_converter.hepmc.Path="hepmc"
    hepmc_converter.genparticles.Path="allGenParticles"
    hepmc_converter.genvertices.Path="allGenVertices"
    ~~~

  * construction of the geometry using DD4hep
    - `detectors` - paths to the XML files with geometry (master file with dimensions and an example: tracker)

    ~~~{.py}
    from Configurables import GeoSvc
    geoservice = GeoSvc("GeoSvc",
                         detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                         'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'])
    ~~~

  * Geant configuration ([see more](#geant-configuration-via-gaudi-service-simg4svc))
    - **detector** - tool providing the [geometry](#geometry-construction), possible: SimG4DD4hepDetector (default) and [SimG4GdmlDetector](#gdml-example)
    - **physicslist** - tool providing the [physics list](#physics-list), possible: SimG4FtfpBert(default)
    - **actions** - tool providing the [user actions initialisation list](#user-actions), possible: SimG4FullSimActions(default)
    ~~~{.py}
    from Configurables import SimG4Svc
    geantservice = SimG4Svc("SimG4Svc",
                             detector='SimG4DD4hepDetector',
                             physicslist="SimG4FtfpBert",
                             actions="SimG4FullSimActions" )
    ~~~


  * simulation ([see more](#simulation-in-gaudi-algorithm-simg4alg))
    - `outputs` - names of the tools saving the [output](#output) from a simulated event,
      possible: `SimG4SaveTrackerHits`, `SimG4SaveCalHits`
    - `eventProvider` - tool that provides `G4Event` to the simulation, possible `SimG4PrimariesFromEdmTool`, `SimG4SingleParticleGeneratorTool`

    ~~~{.py}
    from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
    savetrackertool = SimG4SaveTrackerHits("SimG4SaveTrackerHits", readoutNames = ["TrackerBarrelReadout", "TrackerEndcapReadout"])
    savetrackertool.positionedTrackHits.Path = "positionedHits"
    savetrackertool.trackHits.Path = "hits"
    particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
    particle_converter.genParticles.Path = "allGenParticles"
    geantsim = SimG4Alg("SimG4Alg",
                         outputs= ["SimG4SaveTrackerHits/SimG4SaveTrackerHits"],
                         eventProvider=particle_converter)
    ~~~

  * saving the output to ROOT file
    - `reaodouts` property of saving tool (in the example above `SimG4SaveTrackerHits`) defines for which readoutNames the hits collections should be translated to EDM. Readout name is defined in DD4hep XML file as the attribute `readout` of `detector` tag.

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

#### Other examples
Another examples of the simulation (different detectors, event generators) can be found in:

~~~{.sh}
Examples/options/
Sim/SimG4Components/tests/options/
~~~

#### GDML example
There is a possibility to create the detector with GDML instead of DD4hep. In this case, however, no sensitive detectors are predefined and user is responsible for their implementation. Preferable hit format is the one used by DD4hep (e.g. `DD4hep::Simulation::Geant4CalorimeterHit`), this way user may use the saving output utilities that are already provided.
Simple example:

~~~{.sh}
./run fccrun.py Sim/SimG4Components/tests/options/geant_fullsim_gdml.py
~~~


## Geant configuration: via GAUDI service SimG4Svc

Main service for simulation with Geant4 `SimG4Svc` owns `sim::RunManager` which derives from `G4RunManager`. Own implementation of G4RunManager was necessary in order to leave the event flow to GAUDI. All the details that does not concern the particle generator remain the same. Consult Geant4 [User's Guide](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForToolkitDeveloper/html/index.html) or [Physics Manual](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/PhysicsReferenceManual/fo/PhysicsReferenceManual.pdf) for more details.

In particular, there are two basic ingredients of the simulation: geometry that describes what is the material in which particle is propagated, and a so-called physics list that contains all the particles that may be created in the simulation together with an associated list of physics processes they may encounter.

The general concept of the simulation in Geant is to divide each simulation 'stage' into smaller pieces until the smallest (and simplest) is reached. The most overall is **run** which is a set of **events**. Each event consists of primary particles that are represented by **tracks**. Each track in a simulation is a **step**-by-step analysis of a particle passage through matter. Geant toolkit leaves a user the possibility to monitor the simulation at any of those stages. Hence, the so-called user actions may be created that undertake user-specified action before/at/after certain stage.

Any communication with the Geant Run Manager is handled by this service:
- configuration:
  - [geometry construction](#geometry-construction)
  - [physics list initialisation](#physics-list)
  - [user actions initialisation](#user-actions)
- simulation:
  - [passing an event to `G4EventManager`](#event-processing)
  - [retrieving a simulated event (with hits collections and other information)](#output)


### Geometry construction

> Consult [detector documentation in FCCSW](../../Detector/doc/DD4hepInFCCSW.md) and [DD4hep user guides][DD4hep] for more details.

FCCSW is using Detector Description for HEP (DD4hep) toolkit.

DD4hep is able to parse automatically the geometry and convert it to Geant4 format. It can be retrieved and passed to the Geant configuration service via tool `SimG4DD4hepDetector`. User does not need to set the geometry tool in `SimG4Svc` as it is by default set to `SimG4DD4hepDetector`. Only `GeoSvc` needs to be configured.

FCCSW provides an alternative way to create the geometry, via GDML description (and tool `SimG4GdmlDetector` with property **gdml** taking a path to the GDML file). It is meant only for the test purposes as it does not support sensitive detectors. User would need to create them on his own. See more in the [example](#gdml-example).

### Sensitive detectors

Sensitive detectors are responsible for creating the hits whenever a particle traverses the active material. The readout of each detector can be described in DD4hep.
(see more in [Detector documentation](../../Detector/doc/DD4hepInFCCSW.md#sensitive-detectors))

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
  > Names of the readouts are also names of the collections of hits from that readout. They are used further in the implementations if `ISimG4SaveOutputTool` that retrieve the hits from Geant and store them in the EDM.

* C++ factory method should:
  * construct all the sub-volumes of the detector (extracting detail information e.g. on number of layers, modules in z direction or in phi, their material, etc. from the XML file)
  * set the sensitive detector type

    ~~~{.cpp}
    DD4hep::Geometry::SensitiveDetector::setType("SimpleCalorimeterSD");
    ~~~

    This way the hits collection are created automatically and are filled whenever a particle traverses a sensitive material. Hits are be stored in either `DD4hep::Simulation::Geant4TrackerHit` or `DD4hep::Simulation::Geant4CalorimeterHit`. See more on the current implementations of the sensitive detector types in the [Detector documentation](../../Detector/doc/DD4hepInFCCSW.md#using-an-existing-sensitive-detector-definition).

### Physics List

Physics list describes all the particles and physics processes used in the simulation.

Physics list tool can be set as **physicslist** property of `SimG4Svc`. If none is set, the default list is used (FTFP_BERT).

Currently used physics list is FTFP_BERT, which is recommended by Geant4 for HEP studies. The tool that creates this list is called `SimG4FtfpBert` (it simply creates `G4VModularPhysicsList` that can be set in `sim::RunManager` by `SimG4Svc`).

List of other reference physics list may be found [here](http://geant4.cern.ch/geant4/support/proc_mod_catalog/physics_lists/referencePL.shtml)

### How to use different physics list
In order to use a different physics list, one needs to construct a physics list tool basing on `SimG4FtfpBert` tool.

Any new implementation of a physics list (or any other component) should be presented in a pull request to HEP-FCC/FCCSW.



### User Actions

User actions tool can be added as a property **actions** to `SimG4Svc`. If none is set, the default action initialization is used (currently empty - no default actions are specified).

Geant allows users to specify what should be performed at any stage of simulation.
User actions are created in the implementation of `GVUserActionInitialization` class, e.g. `sim::FullSimActions::Build()`.
Any implementation of action initialisation list should have a relevant GAUDI component (tool) that creates it. Tool creating `sim::FullSimActions` is called `SimG4FullSimActions`.

Currently `sim::FullSimActions` is empty and no user actions are created.

User actions may derive from the following G4 interfaces:
* G4UserRunAction
* G4UserEventAction
* G4UserStackingAction
* G4UserTrackingAction
* G4UserSteppingAction
* G4UserTimeStepAction


### How to add a user action

Any user action that derives from Geant4 interface can be implemented in `Sim/SimG4Full/` subpackage.

In order to invoke this action in the simulation, it should be created in the `sim::FullSimActions::Build()` method.

Different 'sets' of user actions can be created in other implementations of `G4VUserActionInitialization`.
In that case, a relevant GAUDI tool should be created, basing on `SimG4FullSimActions`. Its name should follow the convention of adding a prefix "SimG4" to the name of the class (implementation of `G4VUserActionInitialization` that it creates).


## Simulation in GAUDI algorithm SimG4Alg

Simulation algorithm handles all the communication between other algorithms and `SimG4Svc`.

It takes as input **eventProvider** tool that passes `G4Event`, either translated from EDM and `MCParticleCollection` (`SimG4PrimariesFromEdmTool`), or from the Geant4 particle gun (`SimG4SingleParticleGeneratorTool`).

Also, a list of names to the output-saving tools can be specified in **outputs**.


### Event Processing

For each execution of the algorithm an event `G4Event` is retrieved from the **eventProvider** tool. `G4Event` is passed to `SimG4Svc` and after the simulation is done, it is retrieved. Here all (if any) saving tools are called. Finally, an event is terminated.


### Output

Saving the output from a simulated `G4Event` is performed by tools deriving from an interface `ISimG4SaveOutputTool`.
Tools should have the data outputs specified.
A method `ISimG4SaveOutputTool::SaveOutput(...)` is meant to retrieve any useful information and save it to EDM.
Useful information means e.g. hits collections (`G4HCofThisEvent`) or anything stored in an implementation of `G4VUserEventInformation`, `G4VUserEventInformation`, `G4VUserTrackInformation`, `G4VUserPrimaryParticleInformation` etc.

Existing tools store hits collections from the tracker detectors (`SimG4SaveTrackerHits`) or calorimeters (`SimG4SaveCalHits`). The names of the hits collections are passed to the saving tool in property **readoutNames**. The name of the readout is defined in DD4hep XML file as the attribute `readout` of `<detector>` tag and also under the `<readout>` tag. For instance, the collection below contains hits in the tracker ("CentralTracker_Readout") ([see more](#sensitive-detectors)). If vector **readoutNames** contains no elements or any name that does not correspond to the hit collection, the tool will fail at initialization.

~~~{.xml}
<readouts>
  <readout name="CentralTracker_Readout">
  ...
  </readout>
</readouts>
~~~

`SimG4SaveTrackerHits` stores **trackHits** (EDM `TrackHitCollection`) and **positionedTrackHits** (EDM `PositionedTrackHitCollection`).
`SimG4SaveCalHits` tool can be used for the hit collections from both the electromagetic and hadronic calorimeters. It stores **caloHits** (EDM `CaloHitCollection`) and **positionedCaloHits** (EDM `PositionedCaloHitCollection`).

Positioned hits contain not only the information about the hit, but also the exact position of each energy deposit. If that information is not required by the study, it can be dropped before saving to the output file (by setting in the algorithm `PodioOutput` the property **outputCommands** to e.g. ['keep *', 'drop positionedHits']).


### Units

Important aspect of the translations between HepMC, EDM and Geant4 are the units. Since each framework uses by default different units, every translation should take that into account.

Conversions between EDM and Geant4 are specified in `sim::Units`.

Conversions between EDM and HepMC depend on the units used by HepMC input file (changes from file to file) and is therefore handled separately in HepMCConverter class.
