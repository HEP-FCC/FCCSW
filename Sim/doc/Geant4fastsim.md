Fast Simulation with Geant4 in FCCSW
====

Instruction how to use the fast simulation in Geant4 within FCCSW (GAUDI).

It is an addition to the [instruction on Geant4 in FCCSW](Geant4fullsim.md).

> All relative paths refer to the main FCCSW directory.

## Contents
* [Overview](#1-overview)
  * [Fast simulation](#11-fast-simulation)
* [Example of fast sim configuration](#2-example)
* [Geant configuration via GAUDI service `G4SimSvc`](#3-geant-configuration-via-gaudi-service-g4simsvc)
  * [Geometry construction](#31-geometry-construction)
  * [Physics list](#32-physics-list)
  * [User actions](#33-user-actions)
* [Simulation in GAUDI algorithm `G4SimAlg`](#4-simulation-in-gaudi-algorithm-g4simalg)
  * [Events](#41-event-processing)
  * [Output](#42-output)

[DD4hep]: http://aidasoft.web.cern.ch/DD4hep "DD4hep user manuals"
[GFlash]: http://inspirehep.net/record/352388 "GFlash"

## How to



## 1. Overview

For details on GAUDI and Geant read [full simulation overview](#Geant4fullsim.md#overview)

### 1.1. Fast simulation

Full simulation uses the detailed detector description and simulates the particles passage through it, taking into account all the physics processes they may encounter. It is very time and CPU consuming.

Therefore for many tasks, especially in the early stage of detector design, the fast simulation is used. It takes a less detailed description of the detector and does not simulate every particle step-by-step. Instead, it simulates the overall response of the (particular) detector in a parametric way.

Generated particles are transported inside the detector and their 4-momentum and/or position are smeared taking into account the resolutions and efficiency. Those smeared particles may be analysed and treated as the reconstructed particles, even though no hits were produced and no reconstruction was performed. All the detector effects (both physics processes that may encounter and detector resolutions) come from the smearing process (or rather the resolutions that were used for smearing).

The resolutions used in the smearing may come arbitrary from our knowledge of the detectors. In that case one applies a Gaussian smearing with a given standard deviation. That approach may be also used by the physicists to test how the detector resolution affects the results. That smearing is currently implemented in FCCSW.

More complex approach involves the construction of the tables with the detector resolutions (pseudorapidity/momentum/particle dependent). They are calculated from a small (relatively) sample of full simulations of single-particle events. Single-particle events simplify the reconstruction process (they don't involve the pattern recognition etc.). Such resolutions are unique for tested detectors hence they may be used for smearing the particles with a better accuracy. Implementation of this approach is still in progress.

Fast simulation in the calorimeters may involve using the so-called frozen showers. Taking into account the calorimeter material and the energy of the particle (e+,e-) one may describe the resulting electromagnetic shower with the equations, hence instead of a detailed simulation, hits in the calorimeter may be created instantly. Such an approach in Geant4 originates from [hep-ex/0001020][GFlash]

Both full and fast simulation can be performed in FCCSW using Geant4. Since the same tools are used for both of them, each simulation may be an interplay of both with the full simulation performed in some volumes and fast simulation in others.


## 2. Example

To run the fast simulation:

~~~{.sh}
./run gaudirun.py options/geant_fastsim.py
~~~

The differences between the configuration file of the fast simulation (options/geant_fastsim.py) with respect to the full simulation:

  * Geant configuration ([see more](#3-geant-configuration-via-gaudi-service-g4simsvc))
    - **physicslist** - tool providing the [physics list](#32-physics-list),

      [`G4FastSimPhysicsList`](#) adds fast sim process on top of the standard  ('full') physics list, eg. `G4FtfpBert`, included by a property **fullphysics**

    ~~~{.py}
    from Configurables import G4FastSimPhysicsList
    physicslisttool = G4FastSimPhysicsList("Physics", fullphysics="G4FtfpBert")
    ~~~

    - **actions** - tool providing the [user actions initialisation list](#33-user-actions)

      `G4FastSimActions` includes in particular `sim::InitializeModelsRunAction` essential for the fast sim models registration to the geometry.
      Also a simple smearing tool `G4ParticleSmearSimple` is created (**smearing** defined in a property **fullphysics**).

    ~~~{.py}
    from Configurables import G4FastSimActions, G4ParticleSmearSimple
    smeartool = G4ParticleSmearSimple("Smear", sigma = 0.02)
    actionstool = G4FastSimActions("Actions", smearing=smeartool)
    ~~~

    - `G4SimSvc` configuration:

    ~~~{.py}
    from Configurables import G4SimSvc
    geantservice = G4SimSvc("G4SimSvc",
                             detector='G4DD4hepDetector',
                             physicslist=physicslisttool,
                             actions=actionstool)
    ~~~


  * simulation ([see more](#4-simulation-in-gaudi-algorithm-g4simalg))
    - as for the full simulation, but using different tools to store the output of the simulation (`G4SaveSmearedParticles`)

    ~~~{.py}
    from Configurables import G4SimAlg,G4SaveSmearedParticles
    saveparticlestool = G4SaveSmearedParticles("G4SaveSmearedParticles")
    saveparticlestool.DataOutputs.particles.Path = "smearedParticles"
    saveparticlestool.DataOutputs.particlesMCparticles.Path = "particleMCparticleAssociation"
    geantsim = G4SimAlg("G4SimAlg",
                         outputs = ["G4SaveSmearedParticles/G4SaveSmearedParticles"])
    geantsim.DataInputs.genParticles.Path="allGenParticles"
    ~~~



3. Geant configuration: via GAUDI service G4SimSvc
----

Described in details in the [instruction on Geant4 simulation in FCCSW](Geant4fullsim.md#3-geant-configuration-via-gaudi-service-g4simsvc)


### 3.1. Geometry construction

Geometry is provided by DD4hep via GAUDI's service `GeoSvc`.

In the fast simulation user wants a specific behaviour in certain geometry volumes. That specific behaviour is described in classes derived from `G4VFastSimulationModel`. Geant will not perform normal transportation inside volumes with fast simulation models attached (providing that particle triggers that model).

  The fast simulation model needs to be attached to a `G4Region` object. That `G4Region` can contain one or many logical volumes (parts of the detector). Logical volumes are created by DD4hep. `G4Region` object is created automatically in `sim::InitializeModelsRunAction` for any detector that has in its name 'Tracker', 'ECal', 'EMCal' or 'HCal'. Name of the detector is specified in DD4hep xml file (see more in [short description](#Geant4fullsim.md#31-geometry-construction) or [DD4hep user guides][DD4hep]):

    ~~~{.xml}
    <detector name ="CentralTracker">
    ~~~

  Currently a fast simulation model `sim::FastSimModelTracker` will be created and attached automatically for every `G4LogicalVolume` that contains name 'tracker'.  Attaching the models is also done in `sim::InitializeModelsRunAction`.
<!--- and can be controlled (switched on/off for tracker/ECal/HCal detectors) using flags set in a constructor of `sim::InitializeModelsRunAction::InitializeModelsRunAction(` (all set to true by default).
-->

  There is also an ongoing work on the implementation of the GFlash parametrisation that could be attached to the calorimeters.


### How to use different smearing resolutions

Smearing is performed using the GAUDI tool derived from `ISmearingTool`.

Currently only smearing for tracker is supported. Simple smearing tool, `G4ParticleSmearSimple`, smears particles (its momenta) with a non-particle and non-momentum dependent resolution. The momentum is smeared by multiplying Gaussian distribution with the mean $\mu=1$ and the standard deviation $\sigma$ (resolution). It can be set as a parameter **sigma** in a job configuration file (default: 0.01= 1%).

[WIP] Ongoing work involves implementation of a tool that would smear according to the resolutions read from a ROOT file. Those resolutions can be particle, momentum an pseudorapidity dependent.
For instance, user would be able to generate resolution files with a simplistic geometry layout using [TkLayout](https://indico.cern.ch/event/446599/contribution/5/attachments/1202368/1750485/OccupancyStudies_ZDrasal.pdf).

[WIP] Regarding the calorimeter parametrisation, there is an ongoing work on the implementation of the frozen showers approach. It is based on the [GFlash library][GFlash], already existing in Geant4.
___


### 3.2. Physics List

Geant simulation requires the description of all the particles and processes, the so-called physics list. An example of such a list is a predefined `FTFP_BERT` physics list.

Fast simulation requires some additions to the standard physics list, hence there is an overlay, created by `G4FastSimPhysicsList` tool. It takes, as a property **fullphysics**, the name of the tool containing the particles' and standard processes' definitions (in case of FTFP_BERT list it is `G4FtfpBert` tool).

Additionally:
* "Coupled Transportation" is used to allow invoking `G4PathFinder` that propagates the particle in the magnetic field.
   It is used within the fast simulation model for tracker to compute the exit position of a particle from the volume (taking the momentum from the entrance to the tracker volume).

* `sim::FastSimPhysics` is registered as an additional process. It attaches the fast simulation manager process to ALL the particles. That means that along with the standard processes such as transportation, multiple scattering etc. the particle can encounter 'fast simulation' process. That happens if a particle enters a volume with fast simulation model attached and if that particle fulfils trigger conditions (is charged in case of `sim::FastSimModelTracker`).

   Details may be found at [Geant4 user guide](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s02.html#sect.PhysProc.Param)


### 3.4. User Actions

For the fast simulation purposes a run user action `sim::InitializeModelsRunAction` is created. It checks the names of the logical volume in the world volume for the key-words: 'Tracker', 'ECal', 'EMCal' and 'HCal'. Currently only 'Tracker' name is valid (other detectors do not have yet the parametrisation).

First, for any name containing 'Tracker', `G4Region`is created. That region becomes an envelope for the `sim::FastSimModelTracker`.
Each model will use the smearing tool for the momentum/energy smearing, so the name of the smearing tool needs to be passed to the `sim::InitializeModelsRunAction` through the property of `G4FastSimActions` (**smearing**).

Furthermore, there is a tracking user action `SaveParticlesUserAction` that sets information about the momentum, vertex position and status of each particle at the end of tracking. The information `sim::ParticleInformation` is associated with any `G4PrimaryParticle` and can be retrieved after the simulation from an event ([see](#42-output)).


## 4. Simulation in GAUDI algorithm G4SimAlg

There is one, common algorithm handling fast and full simulation in Geant4. Indeed, full simulation is performed for all the particles and all the detectors that do not have fast simulation models attached (in `sim::InitializeModelsRunAction`, [see](#31-geometry-construction)).

However, in order to be able to save the smeared particles, user need to create `G4SaveSmearedParticles` tool.

### 4.1. Event Processing

As for [full simulation](#Geant4fullsim.md#41-event-processing).

### 4.2. Output

`G4SaveSmearedParticles` tool stores all the particles (EDM `ParticleCollection`) and particlesMCparticles (EDM `ParticleMCParticleAssociationCollection`). They can be treated as 'reconstructed' particles as the detector effects (both resolution and reconstruction efficiency) are imitated by the smearing and the resulting changes to the momentum are taken into account.

At the end of tracking (`SaveParticlesUserAction::PostUserTrackingAction()`), information about the particles momenta, status and vertex position is stored in `sim::ParticleInformation`. Particle information contains also the reference to the EDM `MCParticle`. Therefore, for each particle in the event, based on `sim::ParticleInformation`, an EDM output is created (including the association between `MCParticle` and `Particle`).

