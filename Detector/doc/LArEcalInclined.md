# Noble liquid calorimeter

### Table of contents:
* [Introduction](#introduction)
* [Geometry of the noble liquid calorimeter](#ecal-geometry)
* [Calorimeter SW organisation in FCCSW](#code-where)
* [Full simulations with noble liquid calorimeter](#full-simulations)
  * [Simulations](#simulations)
  * [Reconstruction](#reconstruction)
* [Optimisation of the calorimeter](#optimisation)
* [HOWTOs](#howtos)
  * [How to recalculate sampling fraction](#sampling-fraction)
  * [How to calculate upstream correction](#upstream-correction)
  * [How to change noise values](#change-noise)

## Introduction

The purpose of this documentation is to introduce the Geant4 simulations and reconstructions of the noble liquid barrel calorimeter. The emphasis is given on the implementation of the calorimeter geometry in FCCSW and the steps to be followed for the optimisation of the calorimeter.

The full simulations and reconstruction in the FCCSW are described in [this tutorial](https://hep-fcc.github.io/fcc-tutorials/full-detector-simulations/README.html).

The design of the calorimeter and the studies for the FCChh calorimetry system are documented on [arXiv](https://arxiv.org/abs/1912.09962).

## Geometry of the noble liquid calorimeter

Highly granular noble liquid calorimeter is composed of absorber plates (lead-steel) inclined with respect to the radial direction. The active material between the plates is liquid argon (LAr). Readout is performed with straight multilayer electrodes realised by printed circuit boards (PCBs). The readout planes are placed in between the absorber plates. The calorimeter is placed in the cryostat to cool down the noble liquid.

The detector collects the signal in individual cells that divide the detector in azimuthal angle, depth and pseudorapidity. The division of the calorimeter in the azimuthal angle is defined by the number of absorbers. The elementary cell, which is the smallest area from which the signal can be collected is the gap in between the absorber and the readout. The elementary cells can be combined, creating a cell of a desired size.

Special layers
- first layer (**presampler**): The absorbers do not contain lead plates in this layer. This layer is 3-4 times smaller compared to the other layers.
- second layer (**strip layer**): The granularity in pseudorapidity is 4x higher compared to the rest of the layers. This layer is important for the pi0/gamma identification.

Geometry description in FCCSW
- Source code in [Detector/DetFCChhECalInclined/src/](../DetFCChhECalInclined/src)
- Configuration files
  - FCCee: [Detector/DetFCCeeECalInclined/compact/](../DetFCCeeECalInclined/compact)
  - FCChh: [Detector/DetFCChhECalInclined/compact/](../DetFCChhECalInclined/compact)
  
Please note that all tunable parameters are in the xml files. You're not expected to touch the geometry source code unless you are 100\% sure what you're doing.

Examples from the configuration file [Detector/DetFCCeeECalInclined/compact/FCCee_ECalBarrel.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel.xml):
- Setting of the inclination angle and the LAr gap thickness
~~~{.xml}
    <!-- Inclination angle of the plates -->
    <constant name="InclinationAngle" value="50*degree"/>
    <!-- thickness of active volume between two absorber plates at barrel Rmin, measured perpendicular to the readout plate -->
    <constant name="LArGapThickness" value="1.806*mm"/>
~~~
- Readout defines the segmentation of the calorimeter. Please note there are two readouts defined - one for simulations (no phi segmentation) and one for reconstruction (with phi segmentation, no module ID). To learn more about the readout in DD4HEP have a look [here](DD4hepInFCCSW.md).
~~~{.xml}
  <readouts>
    <!-- readout for the simulation -->
    <!-- offset in eta is eta max value, better more than less -->
    <readout name="ECalBarrelEta">
      <segmentation type="GridEta" grid_size_eta="0.01" offset_eta="-1.0"/>
      <id>system:4,cryo:1,type:3,subtype:3,layer:8,module:11,eta:9</id>
    </readout>
    <!-- readout for the reconstruction -->
    <!-- phi position is calculated based on the centre of volume (hence it cannot be done in the simulation from energy deposits position) -->
    <readout name="ECalBarrelPhiEta">
      <segmentation type="FCCSWGridPhiEta" grid_size_eta="0.01" phi_bins="704" offset_eta="-1.0" offset_phi="-pi+(pi/704.)"/>
      <id>system:4,cryo:1,type:3,subtype:3,layer:8,eta:9,phi:10</id>
    </readout>
  </readouts>
~~~

## Full simulations with noble liquid calorimeter

The full simulations of the calorimeter consists of three steps - simulation, digitisation and reconstruction. What exactly is done in these steps is described below. It is recommended to run the simulation and digitisation in one go. The simulation is the most CPU consuming part. By performing the digitisation step we reduce the size of the output file a lot. The output from this first step is used for the reconstruction. This allows to perform the optimisation of the reconstruction algorithms on the prepared simulated samples.

### Geant4 simulation & digitisation
 - Generation of particle (e.g. particle gun, event generator)
 - Passage of particles through detector (depends on the detector description)
 - Merge Geant4 hits into cells (cell sizes described by the field *readout*) (*)
 - Calibrate deposited energy to electromagnetic scale (application of the sampling fraction)
 - Output: truth particles & vertices, calorimeter cells
 - Example script [Reconstruction/RecFCCeeCalorimeter/option/runCaloSim.py](../../Reconstruction/RecFCCeeCalorimeter/option/runCaloSim.py)

(*) Merging hits into cells\n
This is done in a couple of steps in the code. The size of the cell is given by the field called *readout* in the configuration xml file.


### Reconstruction
 - Input: calorimeter cells
 - Add Gaussian noise to the cells (*)
 - Merge cells into clusters (sliding window algorithm, topoclusters) (**)
 - Output: calorimeter clusters
 - Example script [Reconstruction/RecFCCeeCalorimeter/option/runFullCaloSystem_ReconstructionSW_noiseFromFile.py](../../Reconstruction/RecFCCeeCalorimeter/option/runFullCaloSystem_ReconstructionSW_noiseFromFile.py)

(*) Gaussian noise

(**) Clustering algorithms\n
Sliding window algorithm is used for electrons/photons, topoclusters for hadrons/jets. Please look in [arXiv](https://arxiv.org/abs/1912.09962) for details about the algorithms and the implementation in the FCCSW.
- Sliding window merge cells in a fixed eta x phi window across all longitudinal layers. One cluster should correspond to one object (electron, photon). The size of the window was optimised for the FCChh geometry.
- Topoclusters are of variable size. It merges cells and their neighbours if the energy in the cell is higher than a certain threshold above noise.   

## Optimisation of the calorimeter

Parameters to be tunned (in configuration xml files)
- Material in the gap between the lead plates (default: LAr)
- Gap thickness (double the gap size lead - readout - lead at minimum radial distance, the gap size is measured perpendicular to the readout)
- Inclination angle of the lead plates

Please note that
- The number of lead plates is calculated from the the thickness of the gap and from the inclination angle. It should be a multiple of 32 or 64 following the requirements for the construction of the calorimeter.
- The segmentation in azimuth (phi) should be a multiple of the elementary cell size in phi.

Once you change the parameters in the geometry description (xml file), you need to
- Recalculate the sampling fractions per layer
- Change the calculated sampling fractions in the nominal xml file
- Estimate the electronic noise per cell

## HOWTOs

### How to recalculate sampling fraction values

Details about the algorithm and usage are given [here](DetectorStudies.md).

Use [fcc_ee_samplingFraction_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_samplingFraction_inclinedEcal.py) with [FCCee_ECalBarrel_calibration.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_calibration.xml) configuration file for FCCee. Change the configuration file to match the geometry you are interested in. The output file histSF_fccee_inclined.root contains histograms with the sampling fraction values per layer (*ecal_sf_layerN*).

Important: It is recommended to run the simulations for the sampling fraction calculation at different energies and take the average values as the sampling fractions.

### How to calculate upstream correction

The energy losses in the upstream material are investigated using the algorithm [UpstreamMaterial](../DetStudies/src/components/UpstreamMaterial.h). The details about the algorithm are given [here](DetectorStudies.md).
Use [fcc_ee_samplingFraction_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_samplingFraction_inclinedEcal.py) with [FCCee_ECalBarrel_upstream.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_upstream.xml) cofiguration file for FCCee. Change the configuration file to match the geometry you are interested in. 

### How to change noise values

