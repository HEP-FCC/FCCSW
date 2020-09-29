Noble liquid calorimeter
==

### Table of contents:
* [Introduction](#introduction)
* [Geometry of the noble liquid calorimeter](#ecal-geometry)
* [Where is relevant code](#code-where)
* [How to run Geant4 full simulations](#run-simulations)
* [Optimisation of the geometry](#optimisation)
  * [Which parameters are tunable](#parameters)
  * [Sampling fraction calculation](#sampling-fraction)
* [Reconstruction in the calorimeters](#reconstruction)

## Introduction

The purpose of this documentation is to introduce the Geant4 simulations and reconstructions of the noble liquid barrel calorimeter. The emphasis is given on the implementation of the calorimeter geometry in FCCSW and the steps to be followed for the optimisation of the calorimeter.

The full simulations and reconstruction of the calorimeter in the FCCSW are described in this [tutorial](https://hep-fcc.github.io/fcc-tutorials/full-detector-simulations/README.html).

The design of the calorimeter and the studies for the FCChh calorimetry system are documented on [arXiv](https://arxiv.org/abs/1912.09962).

## Geometry of the noble liquid calorimeter

Highly granular noble liquid calorimeter is composed of absorber plates (lead-steel) inclined with respect to the radial direction. The active material between the plates is liquid argon (LAr). Readout is performed with straight multilayer electrodes realised by printed circuit boards (PCBs). The readout planes are placed in between the absorber plates. The calorimeter is placed in the cryostat to cool down the noble liquid.

The detector collects the signal in individual cells that divide the detector in azimuthal angle, depth and pseudorapidity. The division of the calorimeter in the azimuthal angle is defined by the number of absorbers. The elementary cell, which is the smallest area from which the signal can be collected is the gap in between the absorber and the readout. The elementary cells can be combined, creating a cell of a desired size.

Special layers
- first layer (presample): The absorbers do not contain lead plates in this layer. This layer is 3-4 times smaller compared to the other layers
- second layer (strip layer): The granularity in pseudorapidity is 4x higher compared to the rest of the layers. This layer is important for the pi0/gamma identification.

Details about the design of the calorimeter are [here](https://arxiv.org/abs/1912.09962)

## Description in the FCCSW
 
Geometry description (source code) can be found [here](https://github.com/HEP-FCC/FCCSW/blob/master/Detector/DetFCChhECalInclined/src/ECalBarrelInclined_geo.cpp) 

Settings of the parameters (xml files) can be found
- [here](https://github.com/HEP-FCC/FCCSW/tree/master/Detector/DetFCCeeECalInclined/compact/FCCee_ECalBarrel.xml) for FCCee
- [here](https://github.com/HEP-FCC/FCCSW/blob/master/Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_Common.xml) for FCChh

All tunable parameters are in the xml files. You're not expected to touch the source code unless you are 100\% sure what you're doing.

### Full simulations with noble liquid calorimeter

Simulation, digitisation ()
- Geant4 full simulations
- Digitisation: Creation of cells
  - Readout

Reconstruction ()
- Adding noise to the cells (optional)
- Sliding window reconstruction

## Optimisation of the geometry

### Which parameters could be tunned
- Material in the gap between the lead plates (default: LAr)
- Gap thickness (double the gap size lead - readout - lead at minimum radial distance, the gap size is measured perpendicular to the readout)
- Inclination angle of the lead plates

The number of lead plates is calculated from the the thickness of the gap and from the inclination angle. It should be a multiple of 32 or 64 following the requirements for the construction of the calorimeter.

The segmentation in azimuth (phi) should be a multiple of the elementary cell size in phi.

### Optimisation circle
Once changing parameters in the xml file, you need to recalculate
- Sampling fractions per layer (xml file for calibration with the desired parameters, python script which does the calculation)
- Change the calculated sampling fractions in the nominal xml file

## Reconstruction in the calorimeters
