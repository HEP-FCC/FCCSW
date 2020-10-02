# Noble liquid calorimeter

### Table of contents:
* [Introduction](#introduction)
* [Geometry of the noble liquid calorimeter](#ecal-geometry)
* [Calorimeter SW organisation in FCCSW](#code-where)
* [How to run Geant4 full simulations](#run-simulations)
* [Optimisation of the geometry](#optimisation)
  * [Which parameters are tunable](#parameters)
  * [Sampling fraction calculation](#sampling-fraction)
* [Reconstruction in the calorimeters](#reconstruction)

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
- Source code in *Detector/DetFCChhECalInclined/src/*
- Definition of parameters
  - FCCee: *Detector/DetFCCeeECalInclined/compact/* (common settings in *FCCee_ECalBarrel.xml*)
  - FCChh: *Detector/DetFCChhECalInclined/compact/* (common settings in *FCChh_ECalBarrel_Common.xml*)
  
Please note that all tunable parameters are in the xml files. You're not expected to touch the geometry source code unless you are 100\% sure what you're doing.


Examples of algorithms, scripts to run
- Simulation and reconstruction
   - FCCee: *Reconstruction/RecFCCeeCalorimeter/*
   - FCChh: *Reconstruction/RecCalorimeter/*
- Energy calibration (sampling fraction, upstream material correction) in *Detector/DetStudies/*
- Scripts are under *test/options* in the relevant directory, algorithms in *src/components*

## Full simulations with noble liquid calorimeter

The full simulations of the calorimeter consists of three steps - simulation, digitisation and reconstruction. What exactly is done in these steps is described below. It is recommended to run the simulation and digitisation in one go. The simulation is the most CPU consuming part. By performing the digitisation step we reduce the size of the output file a lot. The output from this first step is used for the reconstruction. This allows to perform the optimisation of the reconstruction algorithms on the prepared simulated samples.

Geant4 simulation & digitisation
 - Generation of particle (e.g. particle gun, event generator)
 - Passage of particles through detector (depends on the detector description)
 - Merge Geant4 hits into cells (cell sizes described by the field *readout*) (*)
 - Calibrate deposited energy to electromagnetic scale (application of the sampling fraction)
 - Output: truth particles & vertices, calorimeter cells
 - Example script: *Reconstruction/RecFCCeeCalorimeter/option/runCaloSim.py*

Reconstruction
 - Input: calorimeter cells
 - Add Gaussian noise to the cells (optional)
 - Merge cells into clusters (sliding window algorithm, topoclusters) (**)
 - Output: calorimeter clusters
 - Example script: *Reconstruction/RecFCCeeCalorimeter/option/runFullCaloSystem_ReconstructionSW_noiseFromFile.py*

(*) Merging hits into cells\n
This is done in a couple of steps in the code. The size of the cell is given by the field called *readout* in the configuration xml file. 

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

## How to

### How to run simulations and reconstructions

### How to recalculate the sampling fraction

Details are given [here](DetectorStudies.md)

Use [fcc_ee_samplingFraction_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_samplingFraction_inclinedEcal.py) and [FCCee_ECalBarrel_calibration.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_calibration.xml) for FCCee.

Change the FCCee_ECalBarrel_calibration.xml configuration file to match the geometry you are interested in. The output file histSF_fccee_inclined_e50GeV_theta90.root contains histograms with the sampling fraction values per layer (*ecal_sf_layerN*).

Important: It is recommended to run the simulations for the sampling fraction calculation at different energies and take the average value as the sampling fraction.

### How to calculate and apply upstream correction

Details are given [here](DetectorStudies.md).
Use [fcc_ee_samplingFraction_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_samplingFraction_inclinedEcal.py) and [FCCee_ECalBarrel_calibration.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_calibration.xml) for FCCee.


### How to change the noise values


