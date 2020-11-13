# Noble liquid calorimeter

### Table of contents:
* [Introduction](#introduction)
* [Geometry of the noble liquid calorimeter](#geometry-of-the-noble-liquid-calorimeter)
  * [Geometry description in FCCSW](#geometry-description-in-FCCSW)
* [Full simulations with noble liquid calorimeter](#full-simulations-with-noble-liquid-calorimeter)
  * [Geant4 simulation and digitisation](#geant4-simulation-and-digitisation)
  * [Reconstruction](#reconstruction)
* [Optimisation of the calorimeter](#optimisation-of-the-calorimeter)
  * [Geometry of the calorimeter](#geometry-of-the-calorimeter)
  * [Cell sizes](#cell-sizes)
* [HOWTOs](#howtos)
  * [How to calculate sampling fraction](#how-to-calculate-sampling-fraction)
  * [How to calculate upstream correction](#how-to-calculate-upstream-correction)
  * [How to change noise values](#how-to-change-noise-values)
  * [How to limit number of layers in cluster reconstruction](#how-to-limit-number-of-layers-in-cluster-reconstruction)
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

### Geometry description in FCCSW
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
- Readout defines the segmentation of the calorimeter. Please note there are **two readouts defined - one for simulation and one for reconstruction**. The only difference between the readouts is the *phi* segmentation and *module* ID. For the simulations we use the physical volumes of the modules to obtain the *module* ID. In the next step we merge hits belonging to the virtual cells defined by module, *eta* and longitudinal *layer*. We calculate positions in *xyz* of these cells. These positions are used for the final cell creation - the cell defined by *phi*, *eta* and longitudinal *layer*. For that the *phi* ID has to be calculated. **Please make sure that all fields except of module and phi are consistent among these two readouts.** To learn more about readouts in DD4HEP have a look [here](DD4hepInFCCSW.md).
~~~{.xml}
  <readouts>
    <!-- readout for the simulation -->
    <!-- offset in eta is eta max value including cryostat -->
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

### Geant4 simulation and digitisation
 - Generation of particle (e.g. particle gun, event generator)
 - Passage of particles through detector (depends on the detector description)
 - Merging Geant4 hits into cells
   - The size of the cell is given by the fields called *readout* in the configuration xml file
   - The hits are merged into cells with default Eta segmentation and positions in xyz are calculated
   - The positions are used to rewrite the cellId using the Phi-Eta segmentation
 - Calibration of deposited energy to electromagnetic scale (application of the sampling fraction) (*)
 - Gaussian noise is not added at this point to reduce the size of the output file
 - Output: truth particles & vertices, calorimeter cells
 - Example script [Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py](../../Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py)

(*) It could be a bit a surprising that we apply the sampling fraction correction even before adding noise to cells. By doing this you can immediately see that the energy corresponds to what is expected right after the simulation is finished. The noise values added during the reconstruction step are expected to be calibrated to the EM scale. On the other hand, this is just an example script, you can just switch on and off the calibration, noise etc. as you like. This is very flexible and up to you.

You can run the example script with the following command
~~~[.sh]
fccrun Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py  \
       --filename fccee_LAr_idea_pgun.root \
       -n 10
~~~
The name of the output file is fccee_idea_LAr_pgun.root. The ROOT file contains the information about MC particles and cells with energy deposits of 10 single particle events (100 GeV e-) in the FCC event data model.

Configuration of create cells algorithm in [Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py](../../Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py) 
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
- *addCellNoise* - add noise to cells (including cells without signal; set to True during the reconstruction step - see below)
- *filterCellNoise* - save only cells above threshold (recommended to set to False; if you would like to run the reconstruction on cells with noise, you need to save all cells)
- *hits* - input hit collection
- *cells* - output hit collection



### Reconstruction
 - Input: calorimeter cells
 - Addition of Gaussian noise to the cells using CreateCaloCells algorithm
   - Noise is added to all cells of the calorimeter (also the ones without signal) 
   - The noise tool expects eta dependent noise values for each longitudinal layer
   - If you expect different noise pattern, you can implement your own noise tool
 - Merge cells into clusters (sliding window algorithm, topoclusters)
   - Sliding window algorithm is used for electrons/photons
   - Topoclustering algorithm for hadrons/jets.
   - Details are described [here](../../Reconstruction/doc/RecCalorimeter.md).
 - Output: calorimeter clusters
 - Example script [Reconstruction/RecFCCeeCalorimeter/options/runFullCaloSystem_ReconstructionSW_noiseFromFile.py](../../Reconstruction/RecFCCeeCalorimeter/options/runFullCaloSystem_ReconstructionSW_noiseFromFile.py)

You can run the example script with the following command
~~~[.sh]
fccrun Reconstruction/RecFCCeeCalorimeter/options/runFullCaloSystem_ReconstructionSW_noiseFromFile.py  \
       --input fccee_idea_LAr_pgun.root  \
       --filename output_allCalo_reco_noise.root
~~~
It takes the input file with calorimeter cells `fccee_idea_LAr_pgun.root`, adds noise to cells and runs the sliding window clustering algorithm. The output file `output_allCalo_reco_noise.root` contains the information about calorimeter cells and the reconstructed clusters.
 
Configuration of the sliding window algorithm in the example script. You have to build calorimeter towers (merge cells across the radius) first and create the clusters afterwards.
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
- Building of the towers
  - *deltaEtaTower* and *deltaPhiTower* describes the size of the tower in the *eta* and *phi*
  - the algorithm requires inputs from all calorimeter system as implemented for FCChh. However, the endcaps and forward calorimeters are not implemented for the FCCee
     - that's why the fields e.g. *ecalFwdReadoutName* are empty
     - that's why the inputs e.g. *towers.ecalFwdCells.Path* are emptyCaloCells (created by CreateEmptyCaloCellsCollection)
- Sliding window algorithm
  - The towers are added in the sliding window algorithm
  - Different window sizes in *eta* x *phi* and *energyThreshold* are configurables of the sliding window algorithm
     - These were optimised for FCChh, optimisation for FCCee is needed

## Optimisation of the calorimeter

### Geometry description

Parameters to be tunned (in configuration xml files)
- Material in the gap between the lead plates (default: LAr)
- Gap thickness (a distance between two nearby absorbers, it is measured perpendicular to the readout at the minimum radial distance of the calorimeter) 
- Inclination angle of the lead plates
- Thicknesses of the absorber plates (Pb, steel and glue) and readout are extrapolated from ATLAS. If you would like to change these values, make sure the numbers are reasonable also from the construction point of view

Please note that
- The number of lead plates is calculated from the the thickness of the gap and from the inclination angle. The number of plates need to be divisible to get smaller construction modules (e.g. 16, 32 or 64 modules) to fit the full *phi* angle.
- The segmentation in azimuth (phi) should be a multiple of the elementary cell size depending on the inclination angle.

Once you change the parameters in the geometry description (xml file), you need to
- Recalculate the sampling fractions per layer
- Change the sampling fractions in the nominal xml file
- Estimate the electronic noise per cell

### Cell sizes

The definition of the cell sizes is done in the configuration xml files. The division in longitudinal *layers* and in *eta* is virtual as there are no physical volumes corresponding to the division in *R* and *eta*. The size of the cell in *phi* should correspond to a multiple of the angle between two absorbers. The segmentation in *phi* is calculated from the position of the centre of the elementary cell defined by *layer*, *eta* and *module* IDs. The segmentation in *eta* is set using the field *grid_size_eta* (for simulation and reconstruction readouts). To change the cell size in *phi*, change the fields *phi_bins* and *offset_phi* (reconstruction readout).

Definition of readout
~~~{.xml}
  <readouts>
    <!-- readout for the simulation -->
    <!-- offset in eta is eta max value including cryostat -->
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

Definition of the thickness of the longitudinal layers is done using field *layer*
~~~{.xml}
<layers>
     <layer thickness="2*cm" repeat="1"/>
     <layer thickness="6.15*cm" repeat="7"/>
</layers>
~~~
The thickness is defined in the radial direction (NOT along the inclined plates!). It allows to define layers of different thickness easily. There is a first layer with thickness of 2 cm and 7 layers of 6.15 cm in this example.

While optimising the layer thickness, you can set the thickness of all layers to a small value (e.g. 0.5 cm). Run the simulations with many layers and merge the layers into cells in the next step. This will give flexibility for the optimisation studies without the need to re-run the simulations over and over. There is a tool which merges layers [MergeLayers](../../Reconstruction/RecCalorimeter/src/components/MergeLayers.h):
~~~[.py]
from Configurables import MergeLayers
mergelayers = MergeLayers("MyMergeLayers",
                   # take the bitfield description from the geometry service
                   readout = ecalReadoutName,
                   # cells in which field should be merged
                   identifier = "layer",
                   volumeName = "layer",
                   # how many cells to merge
                   merge =  [7]*5+[8],
                   OutputLevel = INFO)
mergelayers.inhits.Path = "ECalCells"
mergelayers.outhits.Path = "mergedECalCells"
~~~
In this example 6 layers are created from 43 layers (7*5+8).

## HOWTOs

### How to calculate sampling fraction values

Details about the algorithm and usage are given [here](DetectorStudies.md).

Use [fcc_ee_samplingFraction_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_samplingFraction_inclinedEcal.py) with [FCCee_ECalBarrel_calibration.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_calibration.xml) configuration file for FCCee. Change the configuration file to match the geometry you are interested in. The output file *histSF_fccee_inclined.root* contains histograms with the sampling fraction values per layer (*ecal_sf_layerN*).

Important: It is recommended to run the simulations for the sampling fraction calculation at different energies and take the average values as the sampling fractions.

Once you derive the new sampling fraction values, you can apply them by changing the field name *samplingFraction* in the CalibrateInLayersTool, e.g. in [Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py](../../Reconstruction/RecFCCeeCalorimeter/options/runCaloSim.py) l. 106.

### How to calculate upstream correction

The energy losses in the upstream material are investigated using the algorithm [UpstreamMaterial](../DetStudies/src/components/UpstreamMaterial.h). The details about the algorithm are given [here](DetectorStudies.md).

Use [fcc_ee_upstreamMaterial_inclinedEcal.py](../DetStudies/tests/options/fcc_ee_upstreamMaterial_inclinedEcal.py) with [FCCee_ECalBarrel_upstream.xml](../DetFCCeeECalInclined/compact/FCCee_ECalBarrel_upstream.xml) cofiguration file for FCCee studies. Change the configuration file to match the geometry you are interested in. 

The upstream correction depends on the thickness of the cryostat in front of the calorimeter and on the geometry of the first layer of the calorimeter. There is a linear correlation between the energy detected in the first layer and the energy deposited in the upstream material. Moreover, the constants of the linear dependence changes with energy and pseudorapidity of the incoming particle. More details about the upstream correction can be found in Section 4.1.1.1 of [this paper](https://arxiv.org/abs/1912.09962).

The application of the upstream correction is missing in the FCCSW, it is on our todo list. The correction is applied on the reconstructed objects - clusters from sliding window algorithm. An example can be found [here](https://github.com/faltovaj/FCC_calo_analysis_cpp/blob/master/scripts/plot_recoMonitor.py#L13).

The upstream correction depends on the thickness of the cryostat in front of the calorimeter and on the geometry of the first layer of the calorimeter. There is a linear correlation between the energy detected in the first layer and the energy deposited in the upstream material. Moreover, the constants of the linear dependence changes with energy and pseudorapidity of the incoming particle. More details about the upstream correction can be found in Section 4.1.1.1 of [this paper](https://arxiv.org/abs/1912.09962).

The application of the upstream correction is missing in the FCCSW, it is on our todo list. The correction is applied on the reconstructed objects - clusters from sliding window algorithm. An example can be found [here](https://github.com/faltovaj/FCC_calo_analysis_cpp/blob/master/scripts/plot_recoMonitor.py#L13).

### How to change noise values

The Gaussian noise to each calorimeter cell is added using [NoiseCaloCellsFromFileTool](../../Reconstruction/RecCalorimeter/src/components/NoiseCaloCellsFromFileTool.h). It uses Root file with histograms showing noise values per cell in individual layers as a function of |*eta*|. An example Root file with electronic noise estimate for LAr calorimeter can be downloaded from [here](http://fccsw.web.cern.ch/fccsw/testsamples/elecNoise_ecalBarrelFCCee_50Ohm_traces1_4shieldWidth.root).

Example of the configuration of the noise tool in the script [Reconstruction/RecFCCeeCalorimeter/options/runFullCaloSystem_ReconstructionSW_noiseFromFile.py](../../Reconstruction/RecFCCeeCalorimeter/options/runFullCaloSystem_ReconstructionSW_noiseFromFile.py)
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
- *noiseFileName* is the path to the Root file with histograms with the noise values per cell in individual layers as a function of |*eta*|
- *elecNoiseHistoName* is the name of the histograms in the Root file. The names are expected in the form of *elecNoiseHistoName*+str(indexOfLayer). *indexOfLayer* goes from 1 to *numRadialLayers*
- *activeFieldName* stands for the name of the readout for the longitudinal layers
- *numRadialLayers* is the numbers of the longitudinal layers
- *addPileup* is expected to be set to False for FCCee

If you want to change the noise values
- Prepare your own version of the Root file with histograms of noise values per cell in individual layers as a function of |eta| with the naming convension described above
- The binning of the histogram is up to you. This script finds the correct bin based on the binning of the histogram automatically
- Add the path to the file in *noiseFileName*
- Use *elecNoiseHistoName* to set the names of the histograms used in your noise file

### How to limit number of layers in cluster reconstruction

If you would like to run the sliding window algorithm with only subset of longitudinal layers, you can use [LayeredCaloTowerTool](../../Reconstruction/RecCalorimeter/src/components/LayeredCaloTowerTool.h) tool to build towers. This tool creates towers from a single cell collection only (from one calorimeter). It will only consider cells within the defined layers of the calorimeter, if the layers are defined by 'layer' bitfield.

Example configuration of the LayeredCaloTowerTool
~~~[.py]
from Configurables import LayeredCaloTowerTool
towers = LayeredCaloTowerTool("towers",
                               deltaEtaTower = 0.01, deltaPhiTower = 2*pi/704.,
                               readoutName = ecalBarrelReadoutName,
			       addLayerRestriction = True, 
			       minimumLayer = 0,
			       maximumLayer = 4,
                               OutputLevel = INFO)
towers.cells.Path = ecalBarrelCellsName + "Noise"
~~~
- *deltaEtaTower* and *deltaPhiTower* describes the size of the tower in the *eta* and *phi*
- *addLayerRestriction* use only a subset of layers or not
- *minimumLayer* and *maximumLayer* are the minimum and maximum layer ID of cells to be added in the towers
