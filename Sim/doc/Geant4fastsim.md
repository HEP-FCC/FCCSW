Fast Simulation with Geant4 in FCCSW
==

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
      * [Trackers](#trackers)
      * [Calorimetry](#calorimetry)
  * [Physics list](#32-physics-list)
* [Simulation in GAUDI algorithm `SimG4Alg`](#4-simulation-in-gaudi-algorithm-simg4alg)
  * [Output](#41-output)

[DD4hep]: http://aidasoft.web.cern.ch/DD4hep "DD4hep user manuals"
[GFlash]: http://inspirehep.net/record/352388 "GFlash"
[tkLayout]: http://fcc-tklayout.web.cern.ch/fcc-tklayout/ "tkLayout"

## 1. Overview

For details on GAUDI and Geant read [full simulation overview](Geant4fullsim.md#1-overview)

### 1.1. Fast simulation

Full simulation uses the detailed detector description and simulates the particles passage through it, taking into account all the physics processes they may encounter. It is very time and CPU consuming.

Therefore for many tasks, especially where huge number of simulated events is needed, the fast simulation is used. It takes a less detailed description of the detector and does not simulate every particle step-by-step. Instead, it simulates the overall response of the (particular) detector in a parametric way.

The parametrisation depends on the detector type. For trackers it may create instantly track (that may be treated as if it was reconstructed) and propagate the particle to the next detector. In case of the calorimeters, for each particle the energy deposits may be created instantly, which are later passed to the reconstruction procedure.

Both full and fast simulation can be performed in FCCSW using Geant4. Since the same tools are used for both of them, each simulation may be an interplay of both with the full simulation performed in some volumes and fast simulation in others.

#### 1.1.1. Tracking detectors

Generated particles are transported inside the tracking detectors and their 4-momentum and/or position are smeared taking into account the resolutions and efficiency. Those smeared particles may be analysed and treated as the reconstructed particles, even though no hits were produced and no reconstruction was performed. All the detector effects (both physics processes that may encounter and detector resolutions) come from the smearing process (or rather the resolutions that were used for smearing).

The resolutions used in the smearing may come arbitrary from our knowledge of the detectors. In that case one applies a Gaussian smearing with a given standard deviation. That approach may be also used by the physicists to test how the detector resolution affects the results.  The resolutions used for the smearing may also come from external tools (for instance those used in the tracker performance studies) and be read in FCCSW from ROOT file. One of such tools is [tkLayout], which provides the momentum and pseudorapidity dependent resolutions. tkLayout is already in use by the FCC community, [see presentation](https://indico.cern.ch/event/446599/contribution/5/attachments/1202368/1750485/OccupancyStudies_ZDrasal.pdf). Tools for those smearing methods are implemented in FCCSW.

More complex approach involves the construction of the tables with the detector resolutions (pseudorapidity/momentum/particle dependent). They are calculated from a small (relatively) sample of full simulations of single-particle events. Single-particle events simplify the reconstruction process (they don't involve the pattern recognition etc.). Such resolutions are unique for tested detectors hence they may be used for smearing the particles with a better accuracy. Implementation of this approach is still in progress.

#### 1.1.2. Calorimeters

When a particle enters the calorimeter, instead of long and detail shower development, the energy deposits may be created instantly, based on the initial energy and type of the particle. The shape of such shower may either come from a parametrisation, or from a library of the so-called frozen showers. First approach, implemented already in Geant4 originates from [hep-ex/0001020][GFlash] and is used in e.g. CMS. It describes the shower profiles (longitudinal and lateral) with a set of parameters, either constant or material dependent. The second approach is used by e..g ATLAS. The library of the showers may be created only for small momentum particles and be applied only for the 'tails' of the showers, leaving the 'core' of the shower to the detailed simulation.

## 2. Example

To run the fast simulation:

~~~{.sh}
./run gaudirun.py Examples/options/geant_fastsim.py
~~~

The differences between the configuration file of the fast simulation (Examples/options/geant_fastsim.py) with respect to the full simulation:

  * Detector - detailed geometry of tracker is not needed, envelope is sufficient
    ~~~{.py}
    from Configurables import GeoSvc
    geoservice = GeoSvc("GeoSvc", detectors=['file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
    'file:Detector/DetCommon/compact/TrackerAir.xml'])

    ~~~

  * Geant configuration ([see more](#3-geant-configuration-via-gaudi-service-simg4svc))
    - **physicslist** - tool providing the [physics list](#32-physics-list),

      `SimG4FastSimPhysicsList` adds fast sim process on top of the standard  ('full') physics list included by a property **fullphysics**, e.g. on top of `SimG4FtfpBert` (default)

    ~~~{.py}
    from Configurables import SimG4FastSimPhysicsList
    physicslisttool = SimG4FastSimPhysicsList("Physics", fullphysics="SimG4FtfpBert")
    ~~~

    - **regions** - create [regions where parametrisation may take place](#regions), based on the attached to it fast simulation model

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
    from Configurables import SimG4Alg,SimG4SaveSmearedParticles, SimG4PrimariesFromEdmTool
    saveparticlestool = SimG4SaveSmearedParticles("SimG4SaveSmearedParticles")
    saveparticlestool.DataOutputs.particles.Path = "smearedParticles"
    saveparticlestool.DataOutputs.particlesMCparticles.Path = "particleMCparticleAssociation"
    particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
    particle_converter.DataInputs.genParticles.Path = "allGenParticles"
    geantsim = SimG4Alg("SimG4Alg",
                         outputs = ["SimG4SaveSmearedParticles/SimG4SaveSmearedParticles"],
                         eventProvider=particle_converter)
    ~~~


3. Geant configuration: via GAUDI service SimG4Svc
----

Described in details in the [instruction on Geant4 simulation in FCCSW](Geant4fullsim.md#3-geant-configuration-via-gaudi-service-simg4svc)


### 3.1. Geometry construction

Geometry is provided by DD4hep via GAUDI's service `GeoSvc`.

### Regions

In the fast simulation user wants to define a specific behaviour in certain geometry regions. That specific behaviour is described in classes derived from `G4VFastSimulationModel`. Geant will not perform normal transportation inside regions with fast simulation models attached (providing that particle triggers that model).

  The fast simulation model needs to be attached to a `G4Region` object. That `G4Region` can contain one or many logical volumes (parts of the detector). Logical volumes are created by DD4hep. `G4Region` is created together with an appropriate fast simulation model and its configuration.

`G4Region` can contain one or many logical volumes (parts of the detector). Regions are created for all the volumes which names as passed in property **volumeNames** of the region tool.  Logical volumes are created by DD4hep. Name of the detector is specified in DD4hep xml file (see more in [short description](Geant4fullsim.md#31-geometry-construction) or [DD4hep user guides][DD4hep]):

~~~{.xml}
<detector name ="CentralTracker">
~~~

Parametrisation may happen only in the specified region (`G4Region`) with the fast simulation model attached. In order to create any region, user should use a tool with an interface `ISimG4RegionTool`. Those tools may be passed to `SimG4Svc` as a vector **regions**. Current implementation contains the tool `SimG4FastSimTrackerRegion` for the tracker parametrisation and `SimG4FastSimCalorimeterRegion` for the calorimeter parametrisation.

### How to define regions
Tools `SimG4FastSimTrackerRegion` and `SimG4FastSimCalorimeterRegion` expect the name of the volume created by DD4hep. Even if this volume is just a simple shape, filled with air (e.g. as in the [example](#2-Example): Detector/DetCommon/compact/TrackerAir.xml). It is recommended that for any other shape of the region (containing more than one logical volume, or for part of some volume), an appropriate volume is created first in DD4hep.


### Trackers

`SimG4FastSimTrackerRegion` handles the creation of the `sim::FastSimModelTracker` model and its configuration:
- **smearing** - (required) tool describing how particles should be smeared (`ISimG4SmearingTool`)
- **minMomentum** - (optional) minimum momentum that triggers the fast sim model
- **maxMomentum** - (optional) maximum momentum that triggers the fast sim model
- **maxEta** - (optional) maximum pseudorapidity that triggers the fast sim model
There is also ongoing work to implement the PDG of the particle that triggers the model (currently it is triggered for all charged particles).

Generally, once the model is triggered, the particle is transported to the exit of the volume (currently using Geant transportation hence only 10 times decrease in the simulation speed). The momentum of such particle is also smeared (and saved), as implemented in the smearing tool.

A default smearing tool, `SimG4ParticleSmearFormula`, uses [TFormula](https://root.cern.ch/doc/master/classTFormula.html) to parse the resolution formula that is momentum dependent and is given as parameter **resolutionMomentum** in a job configuration file (as string). This string must be a valid formula expression, e.g. `"sin(x)/x"` or `"0.01*x^2"`, where `x` refers to the momentum. All parameters should be defined directly in the expression. For more information please check [TFormula documentation](https://root.cern.ch/doc/master/classTFormula.html). The resolution of tracker may be constant (as in the above-mentioned example) and in that case, for the performance reasons only, `SimG4ParticleSmearSimple` may be a more suitable tool.

The third available tool uses the momentum and pseudorapidity dependent resolutions read from ROOT file. Such a file may be obtained with the [tkLayout]. The tool `SimG4ParticleSmearRootFile` reads ROOT file defined in a property **filename** in a job configuration file (in the example `/afs/cern.ch/exp/fcc/sw/0.7/testsamples/tkLayout_example_resolutions.root`). The resolutions are defined for the narrow pseudorapidity bins, and they are evaluated for the particle momentum based on the linear interpolation between two closest momenta for which the resolutions were computed by tkLayout.
File has a following structure. It contains two trees: 'info' and 'resolutions'. Tree 'info' contains two branches, each with `TArrayD`: 'eta' and 'p'. Array 'eta' contains upper edge of the pseudorapidity bin (lower edge of first bin is 0). Array 'p' informs for which momenta the resolutions were created. The minimum and maximum momentum (and pseudorapidity) of a particle that can be smeared is described by the minimal and maximal values in those arrays. Tree 'resolutions' contains `TArrayD` of resolutions for each momentum (and there are as many arrays as eta bins).

### How to use different smearing resolutions

Smearing is performed using the GAUDI tool derived from `ISimG4SmearingTool`. Currently there are three tools that may be used for this purpose.

The main concept of smearing for all tools is the same. The momentum is smeared by multiplying it by the randomly generated number from a Gaussian distribution with the mean $\mu=1$ and the standard deviation $\sigma$ (resolution). The difference comes from the way the resolutions are obtained.

The simple smearing tool, `SimG4ParticleSmearSimple`, smears particles (its momenta) with a non-particle and non-momentum dependent constant resolution. It can be set as a parameter **sigma** in a job configuration file (default: 0.01= 1%).


### Calorimetry

Region... TODO TOD TODO

### 3.2. Physics List

Geant simulation requires the description of all the particles and processes, the so-called physics list. An example of such a list is a predefined `FTFP_BERT` physics list.

Fast simulation requires some additions to the standard physics list, hence there is an overlay, created by `SimG4FastSimPhysicsList` tool. It takes, as a property **fullphysics**, the name of the tool containing the particles' and standard processes' definitions (in case of FTFP_BERT list it is `SimG4FtfpBert` tool).

Additionally:
* "Coupled Transportation" is used to allow invoking `G4PathFinder` that propagates the particle in the magnetic field.
   It is used within the fast simulation model for tracker to compute the exit position of a particle from the volume (taking the momentum from the entrance to the tracker volume).

* `sim::FastSimPhysics` is registered as an additional process. It attaches the fast simulation manager process to ALL the particles. That means that along with the standard processes such as transportation, multiple scattering etc. the particle can encounter 'fast simulation' process. That happens if a particle enters a region with fast simulation model attached and if that particle fulfils trigger conditions (is charged in case of `sim::FastSimModelTracker`).

   Details may be found at [Geant4 user guide](http://geant4.web.cern.ch/geant4/UserDocumentation/UsersGuides/ForApplicationDeveloper/html/ch05s02.html#sect.PhysProc.Param)


## 4. Simulation in GAUDI algorithm SimG4Alg

There is one, common algorithm handling fast and full simulation in Geant4. Indeed, full simulation is performed for all the particles and all the detectors that do not have fast simulation models attached and for particles that did not trigger those models.

However, in order to be able to save the smeared particles in the tracking detectors, user needs to create `SimG4SaveSmearedParticles` tool. To save hits from the calorimeters user specifies the same tools that are used to save hits from the full simulation. Also the digitisation procedure and the reconstruction are shared between fast and full simulation of calorimeters.

## 4. Simulation in GAUDI algorithm SimG4Alg

There is one, common algorithm handling fast and full simulation in Geant4. In particular, full simulation is performed for all the particles that do not trigger any fast simulation model ([see more](#31-geometry-construction)).

However, in order to be able to save the smeared particles in the tracker (that may be treated as 'reconstructed' tracks), user need to create `SimG4SaveSmearedParticles` tool.

### 4.1. Output

`SimG4SaveSmearedParticles` tool stores all the particles (EDM `ParticleCollection`) and particlesMCparticles (EDM `ParticleMCParticleAssociationCollection`). They can be treated as 'reconstructed' particles as the detector effects (both resolution and reconstruction efficiency) are imitated by the smearing and the resulting changes to the momentum are taken into account.
In the current implementation only the primary particles may be saved as they contain the particle information created in the translation of the event. This needs to be reimplemented so that the information is attached to the track rather then to the particle.
