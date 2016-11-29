RecCalorimeter package
===

Information about calorimeter reconstruction software within FCCSW. The software is designed for ECAL, but should be general enough to be used for other calorimeters.

# Detector description

ECAL calorimeter description in `Detector/DetFCChhECalSimple`:

* Tube geometry with alternating layers of active and passive material
* Using phi-eta segmentation
* Calorimeter cells defined by a layer in R + phi-eta segment

# Digitisation

Digitisation creates cells out of simulated energy deposits. From the EDM point of view, both input and output of the digitisation uses `fcc::CaloHit`.
The input (simulated deposits) contains raw information about the energy deposited in the cells of the sensitive volumes.
The ouput (cells) contains calibrated energy and the noise. It may also use different segmentation than the original cells of the sensitive volumes.

> Note: some resegmentation procedures (`RedoSegmentation`) expect the truth position of the energy deposits, hence they require `fcc::PositionedCaloHit` collection.

`CreateCaloCells` is an algorithm for the digitisation. It calls 3 tools to perform the deposits merging, energy calibration and noise addition.

Options:
* hits (input data)
* cells (output data)
* doCellCalibration (bool, calibrate energy to EM scale?)
* addCellNoise (bool, add noise to cells?)
* filterCellNoise (bool, save only cells with energy above threshold?)
* readoutName (string, name of the detector readout)
* activeVolumeName (string, name of active volumes)
* activeFieldName (string, name of active layers for sampling calorimeter)
* fieldNames (list of strings, name of the fields describing the segmented volume)
* fieldValues (list of ints, values of the fields describing the segmented volume)

## 1. Deposits merging

The output of the simulation contains a large collection of energy deposits. First step towards the digitisation is merging those hits into the hit collection with one hit per cell of the sensitive volume. The energy of such hit is a sum of all the deposits within that sensitive volume cell. 

> Note: Energy deposits may be merged by Sensitive Detector implementation (`AggregateCalorimeterSD`), however it is not encouraged due to the time performance issues.

>TODO: Take time into account. At the moment the time of the last hit in the cell is stored.

`MergeCaloHitsTool`: Merge Geant4 energy deposits with same cellID

## 2. Energy calibration

Energy calibration is done for sampling calorimeters to compensate for the energy deposited in the passive material.

`CalibrateCaloHitsTool`: Calibrate Geant4 energy to electromagnetic (EM) scale. The calibration constant (inverse sampling fraction) is a configurable parameter of this tool.

## 3. Noise addition

Noise is added to all cells, not only those with signal. Hence a list of all cellIDs is needed and it is retrieved from the geometry. This is the only part which is geometry dependent and assumes tube geometry and phi-eta segmentation.

 `NoiseCaloCellsTool`: Prepare random noise hits for each cell.
 `MergeCaloHitsTool`: Merge signal with noise hits.
 `NoiseCaloCellsTool`: Filter cell energy and accept only cells with energy above threshold.


# Reconstruction

Reconstruction creates clusters (`fcc::CaloCluster`) out of cells (`fcc::CaloHit`). Each cluster stores the information about its global position (x, y, z), energy and the relation to the cells it is composed of.

## Sliding window algorithm

Sliding window algorithm is an algorithm used for the reconstruction of electrons and photons [ref].

`CreateCaloClustersSlidingWindow` implementation assumes the detector with eta-phi segmentation.

> Note: If this segmentation was not used for the original simulation and the true deposits' positions are saved (`fcc::PositionedCaloHit`, it is possible to resegment using `RedoSegmentation` algorithm.

Options:
* cells  (input data)
* clusters (output data)
* readoutName (string, used to retrieve the segmentation)
* fieldNames (list of strings, name of the fields describing the segmented volume)
* fieldValues (list of ints, values of the fields describing the segmented volume)
* deltaEtaTower (float, size of the tower in eta)
* deltaPhiTower (float, size of the tower in eta)
* nEtaWindow (int, size in eta of the sliding window, in units of tower size)
* nPhiWindow (int, size in phi of the sliding window, in units of tower size)
* nEtaPosition (int, size in eta of the window used for barycentre calculation, in units of tower size)
* nPhiPosition (int, size in phi of the window used for barycentre calculation, in units of tower size)
* nEtaDuplicates (int, size in eta of the window used to remove duplicates, in units of tower size)
* nPhiDuplicates (int, size in phi of the window used to remove duplicates, in units of tower size)
* energyThreshold (float, GeV, energy threshold for the sliding window to find pre-clusters)

### 1. Create calorimeter towers.

A tower contains all cells within certain eta and phi (tower size: **deltaEtaTower**, **deltaPhiTower**). Distance in r plays no role in this algorithm.

>TODO: Currently there is no support of cell splitting, so each cell should be completely inside the tower and that can be achieved using `GridEtaPhi` segmentation.

### 2. Find local maxima.

Local maxima are found using the sliding window of a fixed size in eta x phi (**nEtaWindow** **nPhiWindow** in units of tower size). If a local max is found and its energy is above threshold (**energyThreshold**), it is added to the preclusters list. Each precluster contains the barycentre position and the transverse energy. Position is recalculated using the window size in eta x phi (**nEtaPosition**, **nPhiPosition**) that may be smaller than the sliding window to reduce the noise influence. Both windows are centred at the same tower. The energy of the precluster is the energy calculated using the sliding window.

### 3. Remove duplicates.

If two pre-clusters are found next to each other (within window **nEtaDuplicates**, **nPhiDuplicates**), the pre-cluster with lower energy is removed.

>TODO: Currently there is no support on energy sharing between clusters, so if the duplicate window is smaller than sliding window, some cells may be taken twice (instead of the weighting their energies).

### 4. Build clusters.

Clusters are created using the pre-clusters energy (energy of towers within the sliding window). Position is calculated from the barycentre position and the inner radius of the detector. For each cluster the cell collection is searched and all those inside the cluster are attached.

>TODO: Final cluster may have a different size, so recalculation of energy around the tower containing barycentre should be done.

# Example

Example script which runs ECAL cell reconstruction is [here](../RecCalorimeter/options/runEcalReconstruction.py).

* Read input file with Geant4 hits
* Produce calo cells (CreateCaloCells)
* Store cells as CaloHits with cellID (produced by CreateCaloCells) and as CaloClusters with position (produced by CreatePositionedHit)
* Reconstruct clusters using the sliding window algorithm (CreateCaloClustersSlidingWindow)

Before running the script, load the library libDetSegmentation.so (necessary because of the phi-eta segmenation usage):
~~~{.sh}
LD_PRELOAD=build.$BINARY_TAG/lib/libDetSegmentation.so
./run gaudirun.py Reconstruction/RecCalorimeter/options/runEcalReconstruction.py
~~~
