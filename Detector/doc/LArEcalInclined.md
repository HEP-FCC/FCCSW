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
  
Please note that all tunable parameters are in the configuration xml files. You're not expected to touch the geometry source code unless you are 100\% sure what you're doing.

Examples from the configuration file [Detector/DetFCCeeECalInclined/compact/FCCee_ECalBarrel.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel.xml):
- Settings of the inclination angle and the size of the LAr gap
~~~{.xml}
    <!-- Inclination angle of the plates -->
    <constant name="InclinationAngle" value="50*degree"/>
    <!-- thickness of active volume between two absorber plates at barrel Rmin, measured perpendicular to the readout plate -->
    <constant name="LArGapThickness" value="1.806*mm"/>
~~~
- Readout defines the segmentation of the calorimeter. Please note there are two readouts defined - one for simulations (no phi segmentation) and one for reconstruction (with phi segmentation, no module ID). Other fields should be consistent among these two readouts. To learn more about readouts in DD4HEP have a look [here](DD4hepInFCCSW.md).
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

Full simulations of the calorimeter consist of simulation, digitisation and reconstruction. What exactly is done in these steps is described below. It is recommended to run the simulation and digitisation in one go. The simulation is the most CPU consuming part. By performing the digitisation step we reduce the size of the output file a lot (the Geant4 hits are merged into cells). The output from this first step is used for the reconstruction. This allows to perform the optimisation of the reconstruction algorithms on the prepared simulated samples.

You can find more details about the implemented algorithms [here](../../Reconstruction/doc/RecCalorimeter.md).

### Geant4 simulation & digitisation
 - Generation of particle (e.g. particle gun, event generator)
 - Passage of particles through detector (depends on the detector description)
 - Merging Geant4 hits into cells
   - The size of the cell is given by the fields called *readout* in the configuration xml file
   - The hits are merged into cells with default Eta segmentation and positions in xyz are calculated
   - The positions are used to rewrite the cellId using the Phi-Eta segmentation
 - Calibration of deposited energy to electromagnetic scale (application of the sampling fraction)
 - Gaussian noise is not added at this point to reduce the size of the output file
 - Output: truth particles & vertices, calorimeter cells
 - Example script [Reconstruction/RecFCCeeCalorimeter/option/runCaloSim.py](../../Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py)

Configuration of create cells algorithm in [Reconstruction/RecFCCeeCalorimeter/option/runCaloSim.py](../../Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py) 
~~~[.py]
from Configurables import CreateCaloCells
createEcalBarrelCellsStep1 = CreateCaloCells("CreateECalBarrelCellsStep1",
			       doCellCalibration=True,	
                               calibTool = calibEcalBarrel,
                               addCellNoise=False, filterCellNoise=False,
                               OutputLevel=INFO,
                               hits="ECalBarrelHits",
                               cells="ECalBarrelCellsStep1")
~~~
- *doCellCalibration* - calibration to electromagnetic scale
- *calibTool* - different sampling fractions per layer (CalibrateInLayersTool) or a single sampling fraction factor (CalibrateCaloHitsTool)
- *addCellNoise* - add noise to cells (including cells without signal)
- *hits* - input hit collection
- *cells* - output hit collection

### Reconstruction
 - Input: calorimeter cells
 - Addition of Gaussian noise to the cells using CreateCaloCells algorithm
   - The noise tool expects eta dependent noise values for each longitudinal layer
   - If you expect different noise pattern, you can implement your own noise tool
 - Merge cells into clusters (sliding window algorithm, topoclusters)
   - Sliding window algorithm is used for electrons/photons
   - Topoclustering algorithm for hadrons/jets.
   - Details are described [here](../../Reconstruction/doc/RecCalorimeter.md).
 - Output: calorimeter clusters
 - Example script [Reconstruction/RecFCCeeCalorimeter/option/runFullCaloSystem_ReconstructionSW_noiseFromFile.py](../../Reconstruction/RecFCCeeCalorimeter/options/runFullCaloSystem_ReconstructionSW_noiseFromFile.py)
 
Configuration of sliding window algorithm configuration from the example script
~~~[.py]
from Configurables import CaloTowerTool
towers = CaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               ecalBarrelReadoutName = ecalBarrelReadoutName,
                               ecalEndcapReadoutName = "",
                               ecalFwdReadoutName = "",
                               hcalBarrelReadoutName = "",
                               hcalExtBarrelReadoutName = "",
                               hcalEndcapReadoutName = "",
                               hcalFwdReadoutName = "",
                               OutputLevel = INFO)
towers.ecalBarrelCells.Path = ecalBarrelCellsName + "Noise"
towers.ecalEndcapCells.Path = "emptyCaloCells" #ecalEndcapCellsName
towers.ecalFwdCells.Path = "emptyCaloCells" #ecalFwdCellsName
towers.hcalBarrelCells.Path = "emptyCaloCells"
towers.hcalExtBarrelCells.Path = "emptyCaloCells" # "newHCalExtBarrelCells"
towers.hcalEndcapCells.Path = "emptyCaloCells" #hcalEndcapCellsName
towers.hcalFwdCells.Path = "emptyCaloCells" #hcalFwdCellsName

from Configurables import CreateCaloClustersSlidingWindow
createClusters = CreateCaloClustersSlidingWindow("CreateClusters",
                                                 towerTool = towers,
                                                 nEtaWindow = windE, nPhiWindow = windP,
                                                 nEtaPosition = posE, nPhiPosition = posP,
                                                 nEtaDuplicates = dupE, nPhiDuplicates = dupP,
                                                 nEtaFinal = finE, nPhiFinal = finP,
                                                 energyThreshold = threshold)
createClusters.clusters.Path = "CaloClusters"
~~~
- You have to build the towers in the calorimeter first
- *deltaEtaTower* and *deltaPhiTower* should correspond to the segmentation of the calorimeter to make use of the fine segmentation
- the algorithm requires inputs from all calorimeter system as implemented for FCChh. However, the endcaps and forward calorimeters are not implemented for the FCCee
     - that's why the fields e.g. *ecalFwdReadoutName* are empty
     - that's why the inputs e.g. *towers.ecalFwdCells.Path* are emptyCaloCells (created by CreateEmptyCaloCellsCollection)
- The towers are added in the sliding window algorithm
- Different window sizes in eta x phi and energyThreshold are configurables of the sliding window algorithm
  - These were optimised for FCChh, optimisation for FCCee is needed

## Optimisation of the calorimeter

Parameters to be tunned (in configuration xml files)
- Material in the gap between the lead plates (default: LAr)
- Gap thickness (double the gap size lead - readout - lead at minimum radial distance, the gap size is measured perpendicular to the readout)
- Inclination angle of the lead plates

Please note that
- The number of lead plates is calculated from the the thickness of the gap and from the inclination angle. It should be a multiple of 32 or 64 following the requirements for the construction of the calorimeter.
- The segmentation in azimuth (phi) should be a multiple of the elementary cell size depending on the inclination angle.

Once you change the parameters in the geometry description (xml file), you need to
- Recalculate the sampling fractions per layer
- Change the sampling fractions in the nominal xml file
- Estimate the electronic noise per cell

## HOWTOs

### How to recalculate sampling fraction values

Details about the algorithm and usage are given [here](DetectorStudies.md).

Use [fcc_ee_samplingFraction_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_samplingFraction_inclinedEcal.py) with [FCCee_ECalBarrel_calibration.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_calibration.xml) configuration file for FCCee. Change the configuration file to match the geometry you are interested in. The output file histSF_fccee_inclined.root contains histograms with the sampling fraction values per layer (*ecal_sf_layerN*).

Important: It is recommended to run the simulations for the sampling fraction calculation at different energies and take the average values as the sampling fractions.

### How to calculate upstream correction

The energy losses in the upstream material are investigated using the algorithm [UpstreamMaterial](../DetStudies/src/components/UpstreamMaterial.h). The details about the algorithm are given [here](DetectorStudies.md).

Use [fcc_ee_samplingFraction_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_samplingFraction_inclinedEcal.py) with [FCCee_ECalBarrel_upstream.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_upstream.xml) cofiguration file for FCCee studies. Change the configuration file to match the geometry you are interested in. 

### How to change noise values

The Gaussian noise to each calorimeter cell is added using [NoiseCaloCellsFromFileTool](../../Reconstruction/RecCalorimeter/src/components/NoiseCaloCellsFromFileTool.h). It uses Root file with histograms showing noise values per cell in individual layers as a function of |eta|. An example Root file with electronic noise estimate for LAr calorimeter can be downloaded from [here](http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_ecalBarrelFCCee_50Ohm_traces1_4shieldWidth.root).

Example of the configuration of the noise tool in the script [Reconstruction/RecFCCeeCalorimeter/option/runFullCaloSystem_ReconstructionSW_noiseFromFile.py](../../Reconstruction/RecFCCeeCalorimeter/options/runFullCaloSystem_ReconstructionSW_noiseFromFile.py)
~~~[.py]
from Configurables import NoiseCaloCellsFromFileTool
noiseBarrel = NoiseCaloCellsFromFileTool("NoiseBarrel",
                                         readoutName = ecalBarrelReadoutName,
                                         noiseFileName = ecalBarrelNoisePath,
                                         elecNoiseHistoName = ecalBarrelNoiseHistName,
                                         activeFieldName = "layer",
                                         addPileup = False,
                                         numRadialLayers = 8)
~~~
- *readoutName* is the name of the readout of the calorimeter
- *noiseFileName* is the path to the Root file with histograms with the noise values per cell in individual layers as a function of |eta|
- *elecNoiseHistoName* is the name of the histograms in the Root file. The names are expected in the form of *elecNoiseHistoName*+str(indexOfLayer). *indexOfLayer* goes from 1 to numRadialLayers
- *activeFieldName* stands for the name of the readout for the longitudinal layers
- *numRadialLayers* is the numbers of the longitudinal layers
- *addPileup* is expected to be set to False for FCCee

If you want to change the noise values
- Prepare your own version of the Root file with histograms of noise values per cell in individual layers as a function of |eta| with the naming convension described above.
- Change the path in *noiseFileName*
- Change the *elecNoiseHistoName* to match the names of the histograms used in your noise file
