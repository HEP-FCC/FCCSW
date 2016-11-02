Fast Simulation with Geant4 in FCCSW
====

Instruction how to use the fast simulation in Geant4 within FCCSW (GAUDI).

It is an addition to the [instruction on Geant4 in FCCSW](Geant4fullsim.md).

> All relative paths refer to the main FCCSW directory.

## Contents
* [Overview](#1-overview)
  * [Fast simulation](#11-fast-simulation)
    * [Tracking detectors](#111-tracking-detectors)
    * [Calorimeters](#112-calorimeters)
* [Example of fast sim configuration](#2-example)
* [Geant configuration via GAUDI service `SimG4Svc`](#3-geant-configuration-via-gaudi-service-simg4svc)
  * [Geometry construction](#31-geometry-construction)
    * [Regions](#regions)
  * [Physics list](#32-physics-list)
  * [User actions](#33-user-actions)
* [Simulation in GAUDI algorithm `SimG4Alg`](#4-simulation-in-gaudi-algorithm-simg4alg)
  * [Events](#41-event-processing)
  * [Output](#42-output)

[DD4hep]: http://aidasoft.web.cern.ch/DD4hep "DD4hep user manuals"
[GFlash]: http://inspirehep.net/record/352388 "GFlash"
[tkLayout]: http://fcc-tklayout.web.cern.ch/fcc-tklayout/ "tkLayout"

## 1. Overview

For details on GAUDI and Geant read [full simulation overview](#Geant4fullsim.md#overview)

### 1.1. Fast simulation

Full simulation uses the detailed detector description and simulates the particles passage through it, taking into account all the physics processes they may encounter. It is very time and CPU consuming.

Therefore for many tasks, especially in the early stage of detector design, the fast simulation is used. It takes a less detailed description of the detector and does not simulate every particle step-by-step. Instead, it simulates the overall response of the (particular) detector in a parametric way.

Both full and fast simulation can be performed in FCCSW using Geant4. Since the same tools are used for both of them, each simulation may be an interplay of both with the full simulation performed in some volumes and fast simulation in others.

#### 1.1.1. Tracking detectors

Generated particles are transported inside the tracking detectors and their 4-momentum and/or position are smeared taking into account the resolutions and efficiency. Those smeared particles may be analysed and treated as the reconstructed particles, even though no hits were produced and no reconstruction was performed. All the detector effects (both physics processes that may encounter and detector resolutions) come from the smearing process (or rather the resolutions that were used for smearing).

The resolutions used in the smearing may come arbitrary from our knowledge of the detectors. In that case one applies a Gaussian smearing with a given standard deviation. That approach may be also used by the physicists to test how the detector resolution affects the results.  The resolutions used for the smearing may also come from external tools (for instance those used in the tracker performance studies) and be read in FCCSW from ROOT file. One of such tools is [tkLayout], which provides the momentum and pseudorapidity dependent resolutions. tkLayout is already in use by the FCC community, [see presentation](https://indico.cern.ch/event/446599/contribution/5/attachments/1202368/1750485/OccupancyStudies_ZDrasal.pdf). Tools for those smearing methods are implemented in FCCSW.

More complex approach involves the construction of the tables with the detector resolutions (pseudorapidity/momentum/particle dependent). They are calculated from a small (relatively) sample of full simulations of single-particle events. Single-particle events simplify the reconstruction process (they don't involve the pattern recognition etc.). Such resolutions are unique for tested detectors hence they may be used for smearing the particles with a better accuracy. Implementation of this approach is still in progress.

#### 1.1.2. Calorimeters

Fast simulation in the calorimeters may involve either an analytical parametrisation of the shower profiles, or the library of the so-called frozen showers.

Taking into account the calorimeter material and the energy of the particle (e+,e-) one may describe the resulting electromagnetic shower with the equations, hence instead of a detailed simulation, hits in the calorimeter may be created instantly based on the set of formulas. Such an approach in Geant4 originates from [hep-ex/0001020][GFlash].

Another approach, the library of frozen showers involves creation of a large number of pre-simulated showers. This approach will be addressed in future.


## 2. Example

To run the fast simulation:

~~~{.sh}
./run gaudirun.py Examples/options/geant_fastsim.py
~~~

The differences between the configuration file of the fast simulation (options/geant_fastsim.py) with respect to the full simulation:

  * Geant configuration ([see more](#3-geant-configuration-via-gaudi-service-simg4svc))
    - **physicslist** - tool providing the [physics list](#32-physics-list),

      `SimG4FastSimPhysicsList` adds fast sim process on top of the standard  ('full') physics list included by a property **fullphysics**, e.g. on top of `SimG4FtfpBert` (default)

    ~~~{.py}
    from Configurables import SimG4FastSimPhysicsList
    physicslisttool = SimG4FastSimPhysicsList("Physics", fullphysics="SimG4FtfpBert")
    ~~~

    - **regions** - create regions where parametrisation may take place, based on the attached to it fast simulation model

      `SimG4FastSimTrackerRegion` creates model `sim::FastSimModelTracker` that configures the parametrisation (trigger conditions, what happens to the particles).
      The parametrisation may be triggered inside the volumes which names are passed in property **volumeNames**.
      Also a simple smearing tool `SimG4ParticleSmearFormula` is created that describes how the particle momentum is altered (once the parametrisation is triggered), property **smearing**.

    ~~~{.py}
    from Configurables import SimG4ParticleSmearFormula, SimG4FastSimTrackerRegion
    smeartool = SimG4ParticleSmearFormula("smear", resolutionMomentum = "0.013")
    regiontool = SimG4FastSimTrackerRegion("model", volumeNames=["TrackerEnvelopeBarrel"], smearing=smeartool)
    geantservice = SimG4Svc("SimG4Svc", physicslist=physicslisttool, regions=["SimG4FastSimTrackerRegion/model"])
    ~~~

    - `SimG4Svc` configuration:

    ~~~{.py}
    from Configurables import SimG4Svc
    geantservice = SimG4Svc("SimG4Svc", physicslist=physicslisttool, regions=["SimG4FastSimTrackerRegion/model"])
    ~~~


  * simulation ([see more](#4-simulation-in-gaudi-algorithm-simg4alg))
    - as for the full simulation, but using different tools to store the output of the simulation (`SimG4SaveSmearedParticles`)

    ~~~{.py}
    from Configurables import SimG4Alg,SimG4SaveSmearedParticles
    saveparticlestool = SimG4SaveSmearedParticles("SimG4SaveSmearedParticles")
    saveparticlestool.DataOutputs.particles.Path = "smearedParticles"
    saveparticlestool.DataOutputs.particlesMCparticles.Path = "particleMCparticleAssociation"
    geantsim = SimG4Alg("SimG4Alg",
                         outputs = ["SimG4SaveSmearedParticles/SimG4SaveSmearedParticles"])
    geantsim.DataInputs.genParticles.Path="allGenParticles"
    ~~~


3. Geant configuration: via GAUDI service SimG4Svc
----

Described in details in the [instruction on Geant4 simulation in FCCSW](Geant4fullsim.md#3-geant-configuration-via-gaudi-service-simg4svc)


### 3.1. Geometry construction

Geometry is provided by DD4hep via GAUDI's service `GeoSvc`.

### Regions

In the fast simulation user wants to set a specific behaviour in certain geometry volumes. Those regions are created by tools deriving from `ISimG4RegionTool`. Each implementation not only creates a region, but also attaches a fast simulation model that describes what happens to the parametrised particle. Any trigger conditions that are set by user should be also passed via region tool to the model. Geant will not perform normal transportation inside volumes with fast simulation models attached (providing that particle triggers that model).

`G4Region` can contain one or many logical volumes (parts of the detector). Regions are created for all the volumes which names as passed in property **volumeNames** of the region tool.  Logical volumes are created by DD4hep. Name of the detector is specified in DD4hep xml file (see more in [short description](#Geant4fullsim.md#31-geometry-construction) or [DD4hep user guides][DD4hep]):

~~~{.xml}
<detector name ="CentralTracker">
~~~

There is currently one implementation of the tool creating regions: `SimG4FastSimTrackerRegion`, creating `sim::FastSimModelTracker`. Once triggered, it transports particle to the exit of the volume (currently using Geant transportation hence only 10 times decrease in the simulation speed). The momentum of such particle is also smeared, as implemented in the smearing tool `ISimG4ParticleSmearTool`.
 User may specify the momentum range and maximum pseudorapidity for which the fast simulation will be performed by setting them for the  SimG4FastSimTrackerRegion tool(**minMomentum**, **maxMomentum** and **maxEta**). There is also ongoing work to implement the setting of the PDG of the particle that triggers the model (currently: all charged particles).


### How to use different smearing resolutions

Smearing is performed using the GAUDI tool derived from `ISimG4ParticleSmearTool`. There are three tools that may be used.

The main concept of smearing for all tools is the same. The momentum is smeared by multiplying it by the randomly generated number from a Gaussian distribution with the mean $\mu=1$ and the standard deviation $\sigma$ (resolution). The difference comes from the way the resolutions are obtained.

The simple smearing tool, `SimG4ParticleSmearSimple`, smears particles (its momenta) with a non-particle and non-momentum dependent constant resolution. It can be set as a parameter **sigma** in a job configuration file (default: 0.01= 1%).

A default smearing tool, `SimG4ParticleSmearFormula`, uses [TFormula](https://root.cern.ch/doc/master/classTFormula.html) to parse the resolution formula that is momentum dependent and is given as parameter **resolutionMomentum** in a job configuration file (as string). This string must be a valid formula expression, e.g. `"sin(x)/x"` or `"0.01*x^2"`, where `x` refers to the momentum. All parameters should be defined directly in the expression. For more information please check [TFormula documentation](https://root.cern.ch/doc/master/classTFormula.html). The resolution of tracker may be constant (as in the above-mentioned example) and in that case, for the performance reasons only, `SimG4ParticleSmearSimple` may be a more suitable tool.

The third available tool uses the momentum and pseudorapidity dependent resolutions read from ROOT file. Such a file may be obtained with the [tkLayout]. The tool `SimG4ParticleSmearRootFile` reads ROOT file defined in a property **filename** in a job configuration file (in the example `/afs/cern.ch/exp/fcc/sw/0.7/testsamples/tkLayout_example_resolutions.root`). The resolutions are defined for the narrow pseudorapidity bins, and they are evaluated for the particle momentum based on the linear interpolation between two closest momenta for which the resolutions were computed by tkLayout.
File has a following structure. It contains two trees: 'info' and 'resolutions'. Tree 'info' contains two branches, each with `TArrayD`: 'eta' and 'p'. Array 'eta' contains upper edge of the pseudorapidity bin (lower edge of first bin is 0). Array 'p' informs for which momenta the resolutions were created. The minimum and maximum momentum (and pseudorapidity) of a particle that can be smeared is described by the minimal and maximal values in those arrays. Tree 'resolutions' contains `TArrayD` of resolutions for each momentum (and there are as many arrays as eta bins).

Regarding the calorimeter parametrisation, there is an ongoing work on the implementation of the parametrised showers approach. It is based on the [GFlash library][GFlash], already existing in Geant4. It will be shortly pushed into master branch.


### 3.2. Physics List

Geant simulation requires the description of all the particles and processes, the so-called physics list. An example of such a list is a predefined `FTFP_BERT` physics list.

Fast simulation requires some additions to the standard physics list, hence there is an overlay, created by `SimG4FastSimPhysicsList` tool. It takes, as a property **fullphysics**, the name of the tool containing the particles' and standard processes' definitions (in case of FTFP_BERT list it is `SimG4FtfpBert` tool).

Additionally:
* "Coupled Transportation" is used to allow invoking `G4PathFinder` that propagates the particle in the magnetic field.
   It is used within the fast simulation model for tracker to compute the exit position of a particle from the volume (taking the momentum from the entrance to the tracker volume).

* `sim::FastSimPhysics` is registered as an additional process. It attaches the fast simulation manager process to ALL the particles. That means that along with the standard processes such as transportation, multiple scattering etc. the particle can encounter 'fast simulation' process. That happens if a particle enters a volume with fast simulation model attached and if that particle fulfils trigger conditions (is charged in case of `sim::FastSimModelTracker`).

   Details may be found at [Geant4 user guide](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s02.html#sect.PhysProc.Param)


## 4. Simulation in GAUDI algorithm SimG4Alg

There is one, common algorithm handling fast and full simulation in Geant4. In particular, full simulation is performed for all the particles that do not trigger any fast simulation model ([see more](#31-geometry-construction)).

However, in order to be able to save the smeared particles in the tracker (that may be treated as 'reconstructed' tracks), user need to create `SimG4SaveSmearedParticles` tool.

### 4.2. Output

`SimG4SaveSmearedParticles` tool stores all the particles (EDM `ParticleCollection`) and particlesMCparticles (EDM `ParticleMCParticleAssociationCollection`). They can be treated as 'reconstructed' particles as the detector effects (both resolution and reconstruction efficiency) are imitated by the smearing and the resulting changes to the momentum are taken into account.
In the current implementation only the primary particles may be saved as they contain the particle information created in the translation of the event. This needs to be reimplemented so that the information is attached to the track rather then to the particle.