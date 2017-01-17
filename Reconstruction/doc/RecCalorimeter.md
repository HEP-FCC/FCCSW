RecCalorimeter package
===

Information about calorimeter reconstruction software within FCCSW. The software being tested using ECAL, but should be general enough to be used for other calorimeters.

# Detector description

ECAL calorimeter description in `Detector/DetFCChhECalSimple`:

* Tube geometry with alternating layers of active and passive material
* Using phi-eta segmentation
* Calorimeter cells defined by a layer in R + phi-eta segment

# Digitisation

Digitisation creates cells out of simulated energy deposits. From the EDM point of view, both input and output of the digitisation uses `fcc::CaloHit`.
The input (simulated deposits) contains raw information about the energy deposited in the cells of the sensitive volumes.
The output (cells) contains calibrated energy and the noise. The cells may correspond to the active volumes or to the segmentation cells. In particular, different segmentation may be used than the original cells of the sensitive volumes used in the simulation.

> Note: resegmentation procedures (`RedoSegmentation`) expect the truth position of the energy deposits, hence they require `fcc::PositionedCaloHit` collection.

`CreateCaloCells` is an algorithm for the digitisation. It calls tools to perform the energy calibration and cell noise addition. It is geometry agnostic and can be used to merge any calorimeter hit collection (unless the noise is added, [see 3.](3-noise-addition)).

## 1. Energy deposits merging

The output of the simulation contains a large collection of energy deposits. First step towards the digitisation is merging those hits into the hit collection with one hit per cell of the sensitive volume. The energy of such hit is a sum of all the deposits within that sensitive volume cell.
`CreateCaloCells` does not take time into account. The current implementation allows introduction of a time threshold (i.a. energy deposits with timing above threshold would be neglected). For any more complicated digitisation, new digitisation algorithm should be developed.

> Note: Energy deposits may be merged by Sensitive Detector implementation (`AggregateCalorimeterSD`), however it is not encouraged due to the time performance issues.

## 2. Energy calibration

Energy calibration is done for sampling calorimeters to compensate for the energy deposited in the passive material.

`CalibrateCaloHitsTool`: Calibrate Geant4 energy to electromagnetic (EM) scale. The calibration constant (inverse sampling fraction) is a configurable parameter of this tool.

## 3. Noise addition

Noise is added to all cells, not only those with signal. Hence a list of all existing in the geometry cell IDs is needed and it is retrieved from the geometry. This is the only part which is geometry dependent.

Such a list is provided by a Gaudi tool deriving from `ICalorimeterTool`. Currently there are two implementations: `TubeLayerPhiEtaCaloTool` and `NestedVolumesCaloTool`.

### ECal geometry

`TubeLayerPhiEtaCaloTool` is used for detectors like simple ECal. It expects cylindrical layers of active volume and phi-eta segmentation. The number of cells is calculated taking each active layer and checking how many phi and eta bins exist (number of phi bins is the same for all layers). The number of all active layers is searched in the geometry by given name.
> Note. Temporarily it is possible to use MergeLayer algorithm which does decrease the total number of existing layers (in cell IDs) without changing the geometry. Hence it is necessary to specify how many active volumes exist (disabling the look-up of active layers in geometry).

### HCal geometry

The second implementation (`NestedVolumesCaloTool`) may be used if no segmentation is used. In this case only volume IDs are used and the total number of active volumes taken from geometry is also the total number of cells. In particular, if active volumes are nested, this tool finds the correct identifiers of all volumes.

Once the total number of cells and their IDs are extracted, the noise is added. The noise tools derive from `INoiseCaloCellsTool`, with two existing implementations.
 `NoiseCaloCellsFlatTool`: Adding noise from Gaussian distribution (with sigma '\b cellNoise'), the same distribution for all cells
 `NoiseCaloCellsFromFileTool`: Adding noise from Gaussian distribution with sigma taken from a file.

> TODO: What is in the file, how to obtain it

# Reconstruction

Reconstruction creates clusters (`fcc::CaloCluster`) out of cells (`fcc::CaloHit`). Each cluster stores the information about its global position (x, y, z), energy and the relation to the cells it is composed of.

## Sliding window algorithm

Sliding window algorithm is an algorithm used for the reconstruction of electrons and photons. The algorithm is the same one as used by the ATLAS experiment [link](https://cds.cern.ch/record/1099735?ln=de).

`CreateCaloClustersSlidingWindow` implementation operates on towers in phi-eta space, hence it requires the cells to have eta-phi segmentation.

> Note: If this segmentation was not used for the original simulation and the true deposits' positions are saved (`fcc::PositionedCaloHit`, it is possible to re-segment using `RedoSegmentation` algorithm. This approach is currently being tested for HCal.

### 1. Create calorimeter towers.

A tower contains all cells within certain eta and phi (tower size: **deltaEtaTower**, **deltaPhiTower**). Distance in r plays no role in this algorithm.

>TODO: Currently there is no support of cell splitting, so each cell should be completely inside the tower and that can be achieved using `GridEtaPhi` segmentation.

### 2. Find local maxima.

Local maxima are found using the sliding window of a fixed size in eta x phi (**nEtaWindow** **nPhiWindow** in units of tower size). If a local max is found and its energy is above threshold (**energyThreshold**), it is added to the preclusters list. Each precluster contains the barycentre position and the transverse energy. Position is recalculated using the window size in eta x phi (**nEtaPosition**, **nPhiPosition**) that may be smaller than the sliding window to reduce the noise influence. Both windows are centered at the same tower. The energy of the precluster also needs recalculation and is done using the final cluster window (**nEtaFinal**, **nPhiFinal**). The precluster is created if that energy is still above the threshold.

### 3. Remove duplicates.

If two pre-clusters are found next to each other (within window **nEtaDuplicates**, **nPhiDuplicates**), the pre-cluster with lower energy is removed.

>TODO: Currently there is no support on energy sharing between clusters, so if the duplicate window is smaller than sliding window, some cells may be taken twice (instead of the weighting their energies).

### 4. Build clusters.

Clusters are created using the pre-clusters energy (energy of towers within the sliding window). Position is calculated from the barycentre position and the inner radius of the detector. For each cluster the cell collection is searched and all those inside the cluster are attached.

# Example

Example script which runs ECAL reconstruction can be found for three cases: [without noise](../RecCalorimeter/options/runEcalReconstructionWithotNoise.py), [with same Gaussian noise for all cells](../RecCalorimeter/options/runEcalReconstructionFlatNoise.py), [with noise from file](../RecCalorimeter/options/runEcalReconstruction.py).

* Read input file with Geant4 hits
* Produce calo cells (CreateCaloCells)
* Store cells as CaloHits with cellID (produced by CreateCaloCells)
* Reconstruct clusters using the sliding window algorithm (CreateCaloClustersSlidingWindow)

First, prepare the input file. Before running the script, load the library libDetSegmentation.so (necessary because of the phi-eta segmentation usage):
~~~{.sh}
LD_PRELOAD=build.$BINARY_TAG/lib/libDetSegmentation.so
./run gaudirun.py Reconstruction/RecCalorimeter/tests/options/geant_fullsim_ecal_singleparticles.py
~~~

Now, we can run the reconstruction:
~~~{.sh}
LD_PRELOAD=build.$BINARY_TAG/lib/libDetSegmentation.so
./run gaudirun.py Reconstruction/RecCalorimeter/tests/options/runEcalReconstructionWithoutNoise.py
./run gaudirun.py Reconstruction/RecCalorimeter/tests/options/runEcalReconstructionFlatNoise.py
./run gaudirun.py Reconstruction/RecCalorimeter/tests/options/runEcalReconstruction.py
~~~

 LocalWords:  AggregateCalorimeterSD CalibrateCaloHitsTool Geant4
 LocalWords:  ICalorimeterTool
