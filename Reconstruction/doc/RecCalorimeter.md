RecCalorimeter package
===

Information about calorimeter reconstruction software within FCCSW. The software is designed for ECAL, but should be general enough to be used for other calorimeters.

ECAL calorimeter description in Detector/DetFCChhECalSimple:
* Tube geometry with alternating layers of active and passive materials
* Using phi-eta segmentation
* Calorimeter cells defined by a layer in R + phi-eta segment

# CreateCaloCells algorithm
--
Cell energy reconstruction from Geant4 energy deposits
Input: Geant4 hits (EDM: CaloHitCollection)
Output: Calorimeter cells (EDM: CaloHitCollection)

Following steps are done
* Merge Geant4 energy deposits with same cellID (MergeCaloHitsTool)
* Calibrate Geant4 energy to electromagnetic (EM) scale (CalibrateCaloHitsTool)
* Prepare random noise hits for each cell (NoiseCaloCellsTool)
A list of all cellIDs are needed for the noise tool (we want to simulate noise in all cells, not only in thouse with signal). This is done in the initialize method of the algorithm. This is the only part which is geometry dependent (asuming tube geometry and phi-eta segmentation).
* Merge signal with noise hits (MergeCaloHitsTool)
* Filter cell energy and accept only cells with energy above threshold (NoiseCaloCellsTool)

**Configurables:**
* hits (input data)
* cells (output data)	
* doCellCalibration (bool, calibrate energy to EM scale?)
* addCellNoise (bool, add noise to cells?)
* filterCellNoise (bool, save only cells with energy above threshold?)
* readoutName (string, name of the detector readout)
* activeVolumeName (string, name of active volumes - material name)
* numVolumesRemove (unsigned, number of volumes with active material which are not readout(e.g. ECAL: LAr bath in cryostat))
* activeFieldName (string, name of active layers for sampling calorimeter)
* fieldNames (vector of string, name of the fields describing the segmented volume)
* fieldValues (vector of int, values of the fields describing the segmented volume);


# MergeCaloHitsTool
--
Tool for merging calo hits (energy) with same cellID. Creates a vector of merged hits and a map (cellID, pointer to CaloHit in the vector of merged hits).
**Methods:**
* std::vector<fcc::CaloHit*> mergeOneCollection(const fcc::CaloHitCollection& aHits) - merge collection aHits
* std::vector<fcc::CaloHit*> mergeTwoVectors(const std::vector<fcc::CaloHit*>& aHits1, const std::vector<fcc::CaloHit*>& aHits2) - two vectors of hits to be merged. The second vector is expected to have unique cellIDs (no merging done inside this vector)!
Currently only energy of the hits are merged. Time not considered at the moment.

# CalibrateCaloHitsTool
--
Tool for energy calibration to electromagnetic scale.
1/sampling fraction (Ebeam/Etotal_hit_energy) used for the calibration, same factor for each cell used. The factor depends on geometry (thickness of active/passive material, materials)
**Method:**
* void calibrate(std::vector<fcc::CaloHit*>& aHits)
**Configurable:**
* invSamplingFraction (double, 1/sampling fraction)

# NoiseCaloCellsTool
--
Tool for calorimeter noise.
Energy units are MeV (calibrated to EM scale) -> cannot be merged directly with Geant4 energy deposits for sampling calorimeters!!!
Noise defined by a single value - sigma of the noise, same for each cell.
**Methods:**
* void createRandomCellNoise(std::vector<fcc::CaloHit*>& aCells): create random CaloHits (gaussian distribution) for the vector of cells. Vector of cells must contain all cells in the calorimeter with their cellIDs.
* void filterCellNoise(std::vector<fcc::CaloHit*>& aCells): remove cells with energy bellow threshold*sigma from the vector of cells
**Configurables:**
* cellNoise (double, sigma of the cell noise)
* filterThreshold (double, energy threshold, cells with energy < filterThreshold*cellNoise removed)


# CreatePositionedHit algorithm
--
Translate cellID into position. Geometry dependent, same input variables describing geometry as CreateCaloCells.

# Example macro 
--
Example macro which runs ECAL cell reconstruction is [here] (https://github.com/HEP-FCC/FCCSW/blame/master/Reconstruction/RecCalorimeter/options/runEcalReconstruction.py)
* Read input file with Geant4 hits
* Produce calo cells (CreateCaloCells)
* Store cells as CaloHits with cellID (produced by CreateCaloCells) and as CaloClusters with position (produced by CreatePositionedHit)
