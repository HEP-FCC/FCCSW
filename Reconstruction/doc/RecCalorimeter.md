RecCalorimeter package
===

Information about calorimeter reconstruction software within FCCSW. The software is designed for ECAL, but should be general enough to be used for other calorimeters.

ECAL calorimeter description in Detector/DetFCChhECalSimple:
* Tube geometry with alternating layers of active and passive materials
* Using phi-eta segmentation
* Calorimeter cells defined by a layer in R + phi-eta segment

# CreateCaloCells

Cell energy reconstruction from Geant4 energy deposits.

Input: Geant4 hits (EDM: CaloHitCollection)

Output: Calorimeter cells (EDM: CaloHitCollection)

**Tools used:**
* MergeCaloHitsTool: Merge Geant4 energy deposits with same cellID
* CalibrateCaloHitsTool: Calibrate Geant4 energy to electromagnetic (EM) scale
* NoiseCaloCellsTool: Prepare random noise hits for each cell

  A list of all cellIDs are needed for the noise tool (we want to simulate noise in all cells, not only in thouse with signal). This is done in the initialize method of the  algorithm. This is the only part which is geometry dependent (assuming tube geometry and phi-eta segmentation).
* MergeCaloHitsTool: Merge signal with noise hits
* NoiseCaloCellsTool: Filter cell energy and accept only cells with energy above threshold

**Configurables:**
* hits (input data)
* cells (output data)	
* doCellCalibration (bool, calibrate energy to EM scale?)
* addCellNoise (bool, add noise to cells?)
* filterCellNoise (bool, save only cells with energy above threshold?)
* readoutName (string, name of the detector readout)
* activeVolumeName (string, name of active volumes - material name)
* numVolumesRemove (unsigned, number of volumes with active material which are not readout - e.g. ECAL: LAr bath in cryostat)
* activeFieldName (string, name of active layers for sampling calorimeter)
* fieldNames (vector of string, name of the fields describing the segmented volume)
* fieldValues (vector of int, values of the fields describing the segmented volume);

More details can be found in [Doxygen documentation] (http://fccsw.web.cern.ch/fccsw/).

# Example

Example script which runs ECAL cell reconstruction is [here] (https://github.com/HEP-FCC/FCCSW/blame/master/Reconstruction/RecCalorimeter/options/runEcalReconstruction.py).
* Read input file with Geant4 hits
* Produce calo cells (CreateCaloCells)
* Store cells as CaloHits with cellID (produced by CreateCaloCells) and as CaloClusters with position (produced by CreatePositionedHit)
