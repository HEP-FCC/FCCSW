#include "CreateFCChhCaloNeighbours.h"

#include "DD4hep/Detector.h"
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

#include "TFile.h"
#include "TTree.h"

DECLARE_COMPONENT(CreateFCChhCaloNeighbours)

CreateFCChhCaloNeighbours::CreateFCChhCaloNeighbours(const std::string& aName, ISvcLocator* aSL)
    : base_class(aName, aSL) {
  declareProperty( "outputFileName", m_outputFileName, "Name of the output file");
}

CreateFCChhCaloNeighbours::~CreateFCChhCaloNeighbours() {}

StatusCode CreateFCChhCaloNeighbours::initialize() {
  // Initialize necessary Gaudi components
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  std::unordered_map<uint64_t, std::vector<uint64_t>> map;
 
  // will be used for volume connecting
  int eCalLastLayer;
  std::pair<int, int> extremaECalLastLayerPhi;
  std::pair<int, int> extremaECalLastLayerEta;
  double eCalEtaOffset = 0;
  double eCalEtaSize = 0;
  double eCalPhiOffset = 0;
  double eCalPhiSize = 0;
  double hCalEtaOffset = 0;
  double hCalEtaSize = 0;
  double hCalPhiOffset = 0;
  dd4hep::DDSegmentation::BitFieldCoder* decoderECalBarrel = nullptr;
  // will be used for volume connecting
  std::pair<int, int> extremaHCalFirstLayerPhi;
  std::pair<int, int> extremaHCalFirstLayerEta;
  std::pair<int, int> extremaHCalFirstLayerZ;
  dd4hep::DDSegmentation::BitFieldCoder* decoderHCalBarrel = nullptr;

  //////////////////////////////////
  /// SEGMENTED ETA-PHI VOLUMES  ///
  //////////////////////////////////
  
  for (uint iSys = 0; iSys < m_readoutNamesSegmented.size(); iSys++) {
    // Check if readouts exist
    info() << "Readout: " << m_readoutNamesSegmented[iSys] << endmsg;
    if (m_geoSvc->lcdd()->readouts().find(m_readoutNamesSegmented[iSys]) == m_geoSvc->lcdd()->readouts().end()) {
      error() << "Readout <<" << m_readoutNamesSegmented[iSys] << ">> does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
    // get PhiEta segmentation
    dd4hep::DDSegmentation::FCCSWGridPhiEta* segmentation;
    segmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
        m_geoSvc->lcdd()->readout(m_readoutNamesSegmented[iSys]).segmentation().segmentation());
    if (segmentation == nullptr) {
      error() << "There is no phi-eta segmentation!!!!" << endmsg;
      return StatusCode::FAILURE;
    }

    info() << "GridPhiEta: size in eta " << segmentation->gridSizeEta() << " , bins in phi " << segmentation->phiBins()
           << endmsg;
    info() << "GridPhiEta: offset in eta " << segmentation->offsetEta() << " , offset in phi "
           << segmentation->offsetPhi() << endmsg;

    auto decoder = m_geoSvc->lcdd()->readout(m_readoutNamesSegmented[iSys]).idSpec().decoder();
    // will be used for volume connecting
    if (m_fieldNamesSegmented[iSys] == "system" && m_fieldValuesSegmented[iSys] == 5) {
      decoderECalBarrel = decoder;
      eCalEtaSize = segmentation->gridSizeEta();
      eCalPhiSize = 2 * M_PI / segmentation->phiBins();
      eCalEtaOffset = segmentation->offsetEta();
      eCalPhiOffset = segmentation->offsetPhi();
    }
    if (m_fieldNamesSegmented[iSys] == "system" && m_fieldValuesSegmented[iSys] == 8) {
      decoderHCalBarrel = decoder;
      hCalEtaSize = segmentation->gridSizeEta();
      hCalEtaOffset = segmentation->offsetEta();
      hCalPhiOffset = segmentation->offsetPhi();
    }
    // Loop over all cells in the calorimeter and retrieve existing cellIDs
    // Loop over active layers
    std::vector<std::pair<int, int>> extrema;
    extrema.push_back(std::make_pair(0, m_activeVolumesNumbersSegmented[iSys] - 1));
    extrema.push_back(std::make_pair(0, 0));
    extrema.push_back(std::make_pair(0, 0));
    for (unsigned int ilayer = 0; ilayer < m_activeVolumesNumbersSegmented[iSys]; ilayer++) {
      dd4hep::DDSegmentation::CellID volumeId = 0;
      // Get VolumeID
      (*decoder)[m_fieldNamesSegmented[iSys]].set(volumeId, m_fieldValuesSegmented[iSys]);
      (*decoder)[m_activeFieldNamesSegmented[iSys]].set(volumeId, ilayer);
      (*decoder)["eta"].set(volumeId, 0);
      (*decoder)["phi"].set(volumeId, 0);
      // Get number of segmentation cells within the active volume
      auto numCells = det::utils::numberOfCells(volumeId, *segmentation);
      extrema[1] = std::make_pair(0, numCells[0] - 1);
      // for layer N-1 of ECal barrel,  will be used for volume connecting
      if (ilayer == (m_activeVolumesNumbersSegmented[iSys] - 1) && m_fieldNamesSegmented[iSys] == "system" &&
          m_fieldValuesSegmented[iSys] == 5) {
        eCalLastLayer = m_activeVolumesNumbersSegmented[iSys] - 1;
        extremaECalLastLayerPhi = std::make_pair(0, numCells[0] - 1);
        extremaECalLastLayerEta = std::make_pair(numCells[2], numCells[1] + numCells[2] - 1);
	extrema[2] = std::make_pair(numCells[2], numCells[1] + numCells[2] - 1);
      }
      else if(m_fieldNamesSegmented[iSys] == "system" &&
	      m_fieldValuesSegmented[iSys] == 8 && m_readoutNamesSegmented[iSys]=="BarHCal_Readout_phieta"){
	uint cellsEta = ceil(( 2*m_activeVolumesEta[ilayer] - segmentation->gridSizeEta() ) / 2 / segmentation->gridSizeEta()) * 2 + 1; //ceil( 2*m_activeVolumesRadii[ilayer] / segmentation->gridSizeEta()) ;
	uint minEtaID = int(floor(( - m_activeVolumesEta[ilayer] + 0.5 * segmentation->gridSizeEta() - segmentation->offsetEta()) / segmentation->gridSizeEta()));
	numCells[1]=cellsEta;
	numCells[2]=minEtaID;
	// for layer 0 of HCal barrel,  will be used for volume connecting
	if (ilayer == 0){	
	  extremaHCalFirstLayerPhi = std::make_pair(0, numCells[0] - 1);
	  extremaHCalFirstLayerEta = std::make_pair(numCells[2], numCells[1] + numCells[2] - 1);	
	  extrema[2] = std::make_pair(numCells[2], numCells[1] + numCells[2] - 1);
	} 
      }
      debug() << "Number of segmentation cells in (phi,eta): " << numCells << endmsg;
      // Loop over segmenation cells
      for (unsigned int iphi = 0; iphi < numCells[0]; iphi++) {
        for (unsigned int ieta = 0; ieta < numCells[1]; ieta++) {
	  dd4hep::DDSegmentation::CellID cellId = volumeId;
	  decoder->set(cellId, "phi", iphi);
	  decoder->set(cellId, "eta", ieta + numCells[2]);  // start from the minimum existing eta cell in this layer
	  uint64_t id = cellId;
	  map.insert(std::pair<uint64_t, std::vector<uint64_t>>(
              id, det::utils::neighbours(*decoder, {m_activeFieldNamesSegmented[iSys], "phi", "eta"}, extrema,
					 id, {false, true, false}, true)));
        }
      }
    }
    if (msgLevel() <= MSG::DEBUG) {
      std::vector<int> counter;
      counter.assign(40, 0);
      for (const auto& item : map) {
        counter[item.second.size()]++;
      }
      for (uint iCount = 0; iCount < counter.size(); iCount++) {
        if (counter[iCount] != 0) {
          info() << counter[iCount] << " cells have " << iCount << " neighbours" << endmsg;
        }
      }
    }
    info() << "total number of cells:  " <<  map.size() << endmsg;
  }

  //////////////////////////////////
  ///      NESTED VOLUMES        ///
  //////////////////////////////////

  for (uint iSys = 0; iSys < m_readoutNamesNested.size(); iSys++) {
    // Sanity check
    if (m_activeFieldNamesNested.size() != 3) {
      error() << "Property activeFieldNamesNested requires 3 names." << endmsg;
      return StatusCode::FAILURE;
    }
    // Check if readouts exist
    info() << "Readout: " << m_readoutNamesNested[iSys] << endmsg;
    if (m_geoSvc->lcdd()->readouts().find(m_readoutNamesNested[iSys]) == m_geoSvc->lcdd()->readouts().end()) {
      error() << "Readout <<" << m_readoutNamesNested[iSys] << ">> does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
    auto decoder = m_geoSvc->lcdd()->readout(m_readoutNamesNested[iSys]).idSpec().decoder();
    // will be used for volume connecting
    if (m_fieldNameNested == "system" && m_fieldValuesNested[iSys] == 8) {
      decoderHCalBarrel = decoder;
    }
    hCalPhiOffset = m_hCalPhiOffset;
    // Get VolumeID
    dd4hep::DDSegmentation::CellID volumeId = 0;
    decoder->set(volumeId, m_fieldNameNested, m_fieldValuesNested[iSys]);
    // Get the total number of given hierarchy of active volumes
    auto highestVol = gGeoManager->GetTopVolume();
    std::vector<unsigned int> numVolumes;
    numVolumes.reserve(m_activeVolumeNamesNested.size());
    for (const auto& volName : m_activeVolumeNamesNested) {
      numVolumes.push_back(det::utils::countPlacedVolumes(highestVol, volName));
      info() << "Number of active volumes named " << volName << " is " << numVolumes.back() << endmsg;
      if (numVolumes.back() == 0) {
        error() << "Volume name " << volName << " not found! Check naming in detector description." << endmsg;
        return StatusCode::FAILURE;
      }
    }
    // First sort to figure out which volume is inside which one
    std::vector<std::pair<std::string, uint>> numVolumesMap;
    for (unsigned int it = 0; it < m_activeVolumeNamesNested.size(); it++) {
      // names of volumes (m_activeVolumeNamesNested) not needed anymore, only corresponding bitfield names are used
      // (m_activeFieldNamesNested)
      numVolumesMap.push_back(std::pair<std::string, uint>(m_activeFieldNamesNested[it], numVolumes[it]));
    }
    std::sort(
        numVolumesMap.begin(), numVolumesMap.end(),
        [](std::pair<std::string, uint> vol1, std::pair<std::string, uint> vol2) { return vol1.second < vol2.second; });
    // now recompute how many volumes exist within the larger volume
    for (unsigned int it = numVolumesMap.size() - 1; it > 0; it--) {
      if (numVolumesMap[it].second % numVolumesMap[it - 1].second != 0) {
        error() << "Given volumes are not nested in each other!" << endmsg;
        return StatusCode::FAILURE;
      }
      numVolumesMap[it].second /= numVolumesMap[it - 1].second;
    }
    // Debug calculation of total number of cells
    if (msgLevel() <= MSG::DEBUG) {
      unsigned int checkTotal = 1;
      for (const auto& vol : numVolumesMap) {
        debug() << "Number of active volumes named " << vol.first << " is " << vol.second << endmsg;
        checkTotal *= vol.second;
      }
      debug() << "Total number of cells ( " << numVolumesMap.back().first << " ) is " << checkTotal << endmsg;
    }
    // make sure the ordering above is as in property activeFieldNamesNested
    std::map<std::string, uint> activeVolumesNumbersNested;
    for (const auto& name : m_activeFieldNamesNested) {
      for (const auto& number : numVolumesMap) {
        if (name == number.first) {
          activeVolumesNumbersNested.insert(std::make_pair(number.first, number.second));
        }
      }
    }

    // Loop over all cells in the calorimeter and retrieve existing cellIDs
    // Loop over active layers
    std::vector<std::pair<int, int>> extrema;
    extrema.push_back(std::make_pair(0, activeVolumesNumbersNested.find(m_activeFieldNamesNested[0])->second - 1));
    extrema.push_back(std::make_pair(0, activeVolumesNumbersNested.find(m_activeFieldNamesNested[1])->second - 1));
    extrema.push_back(std::make_pair(0, activeVolumesNumbersNested.find(m_activeFieldNamesNested[2])->second - 1));
    // for layer 0 of HCal barrel
    if (m_fieldNameNested == "system" && m_fieldValuesNested[iSys] == 8) {
      extremaHCalFirstLayerPhi =
          std::make_pair(0, activeVolumesNumbersNested.find(m_activeFieldNamesNested[1])->second - 1);
      extremaHCalFirstLayerZ =
          std::make_pair(0, activeVolumesNumbersNested.find(m_activeFieldNamesNested[2])->second - 1);
    }
    for (unsigned int ilayer = 0; ilayer < activeVolumesNumbersNested.find(m_activeFieldNamesNested[0])->second;
         ilayer++) {
      for (unsigned int iphi = 0; iphi < activeVolumesNumbersNested.find(m_activeFieldNamesNested[1])->second; iphi++) {
        for (unsigned int iz = 0; iz < activeVolumesNumbersNested.find(m_activeFieldNamesNested[2])->second; iz++) {

	  dd4hep::DDSegmentation::CellID cID = volumeId;
          decoder->set(cID, m_activeFieldNamesNested[0], ilayer);
	  decoder->set(cID, m_activeFieldNamesNested[1], iphi);
	  decoder->set(cID, m_activeFieldNamesNested[2], iz);
          
          map.insert(std::pair<uint64_t, std::vector<uint64_t>>(
              cID, det::utils::neighbours(*decoder, {m_activeFieldNamesNested[0], m_activeFieldNamesNested[1],
                                                          m_activeFieldNamesNested[2]},
                                               extrema, cID, {false, true, false}, true)));
        }
      }
    }
    if (msgLevel() <= MSG::DEBUG) {
      std::vector<int> counter;
      counter.assign(40, 0);
      for (const auto& item : map) {
        counter[item.second.size()]++;
      }
      for (uint iCount = 0; iCount < counter.size(); iCount++) {
        if (counter[iCount] != 0) {
          info() << counter[iCount] << " cells have " << iCount << " neighbours" << endmsg;
        }
      }
    }
  }

  //////////////////////////////////////////////////
  ///      BARREL: connection ECAL + HCAL        ///
  /////////////////////////////////////////////////
  int count=0;
  if (m_connectBarrels) {
    // first check if ECAL barrel (system==5) and HCal barrel (system==8) are configured
    if (decoderECalBarrel == nullptr || decoderHCalBarrel == nullptr) {
      error() << "ECAL barrel and/or HCal barrel are not configured correctly!" << endmsg;
      return StatusCode::FAILURE;
    }
    // print how many cells in each dimensions will be matched
    if(m_readoutNamesNested.size()!=0){
      info() << "ECAL layer " << eCalLastLayer << " is a neighbour of HCAL layer 0." << endmsg;
      info() << "ECAL phi cells " << extremaECalLastLayerPhi.first << " - " << extremaECalLastLayerPhi.second
	     << " will be matched to HCAL " << m_activeFieldNamesNested[1] << "(s) " << extremaHCalFirstLayerPhi.first
	     << " - " << extremaHCalFirstLayerPhi.second << endmsg;
      info() << "ECAL eta cells " << extremaECalLastLayerEta.first << " - " << extremaECalLastLayerEta.second
	     << " will be matched to HCAL " << m_activeFieldNamesNested[2] << "(s) " << extremaHCalFirstLayerZ.first
	     << " - " << extremaHCalFirstLayerZ.second << endmsg;
    }
    else{
      info() << "ECAL layer " << eCalLastLayer << " is a neighbour of HCAL layer 0." << endmsg;
      info() << "ECAL phi cells " << extremaECalLastLayerPhi.first << " - " << extremaECalLastLayerPhi.second
	     << " will be matched to HCAL cells " << extremaHCalFirstLayerPhi.first
	     << " - " << extremaHCalFirstLayerPhi.second << endmsg;
      info() << "ECAL eta cells " << extremaECalLastLayerEta.first << " - " << extremaECalLastLayerEta.second
	     << " will be matched to HCAL " << extremaHCalFirstLayerEta.first
	     << " - " << extremaHCalFirstLayerEta.second << endmsg;
    }
    
    std::unordered_map<uint, std::vector<uint>> etaNeighbours;
    std::unordered_map<uint, std::vector<uint>> phiNeighbours;
    double hCalPhiSize = 2 * M_PI / (extremaHCalFirstLayerPhi.second - extremaHCalFirstLayerPhi.first + 1);
    // loop over z and find which eta cells to add
    if (m_readoutNamesNested.size()!=0){
      for (int iZ = 0; iZ < extremaHCalFirstLayerZ.second + 1; iZ++) {
	double lowZ = m_hCalZOffset + iZ * m_hCalZSize;
	double highZ = m_hCalZOffset + (iZ + 1) * m_hCalZSize;
	double lowEta = 0, highEta = 0;
	if (fabs(lowZ) < 1e-3) {
	  lowEta = 0;
	} else {
	  lowEta =
            lowZ / fabs(lowZ) * (-log(fabs(tan(atan(m_hCalRinner / lowZ) / 2.))));  // theta = atan(m_hCalRinner / lowZ)
	}
	if (fabs(highZ) < 1e-3) {
	  highEta = 0;
	} else {
	  highEta = highZ / fabs(highZ) * (-log(fabs(tan(atan(m_hCalRinner / highZ) / 2.))));
	}
	debug() << "HCal z id  : " << iZ << endmsg;
	debug() << "HCal eta range  : " << lowEta << " -  " << highEta << endmsg;
	int lowId = floor((lowEta - 0.5 * eCalEtaSize - eCalEtaOffset) / eCalEtaSize);
	int highId = floor((highEta + 0.5 * eCalEtaSize - eCalEtaOffset) / eCalEtaSize);
	debug() << "ECal eta range  : " << lowId * eCalEtaSize + eCalEtaOffset << " -  "
		<< highId * eCalEtaSize + eCalEtaOffset << endmsg;
	std::vector<uint> neighbours;
	for (int idEtaToAdd = lowId; idEtaToAdd <= highId; idEtaToAdd++) {
	  if (idEtaToAdd >= extremaECalLastLayerEta.first && idEtaToAdd <= extremaECalLastLayerEta.second) {
	    neighbours.push_back(idEtaToAdd);
	  }
	}
	debug() << "HCal z id  : " << iZ << endmsg;
	debug() << "Found ECal Neighbours in eta : " << neighbours.size() << endmsg;
	for (auto id : neighbours) {
	  debug() << "ECal Neighbours id : " << id << endmsg;
	}
	etaNeighbours.insert(std::pair<uint, std::vector<uint>>(iZ, neighbours));
      }
    }
    else{ // loop over eta cells to match in eta
      for (int iEta = 0; iEta < extremaHCalFirstLayerEta.second + 1; iEta++) {
	double lowEta = hCalEtaOffset + iEta * hCalEtaSize;
	double highEta = hCalEtaOffset + (iEta + 1) * hCalEtaSize;
	debug() << "HCal eta range  : " << lowEta << " -  " << highEta << endmsg;
	int lowId = floor((lowEta - 0.5 * eCalEtaSize - eCalEtaOffset) / eCalEtaSize);
	int highId = floor((highEta + 0.5 * eCalEtaSize - eCalEtaOffset) / eCalEtaSize);
	debug() << "ECal eta range  : " << lowId * eCalEtaSize + eCalEtaOffset << " -  "
		<< highId * eCalEtaSize + eCalEtaOffset << endmsg;
	std::vector<uint> neighbours;
	for (int idEtaToAdd = lowId; idEtaToAdd <= highId; idEtaToAdd++) {
	  neighbours.push_back(det::utils::cyclicNeighbour(idEtaToAdd, extremaECalLastLayerEta));
	}
	debug() << "HCal eta id  : " << iEta << endmsg;
	debug() << "Found ECal Neighbours in eta : " << neighbours.size() << endmsg;
	for (auto id : neighbours) {
	  debug() << "ECal Neighbours id : " << id << endmsg;
	}
	etaNeighbours.insert(std::pair<uint, std::vector<uint>>(iEta, neighbours));
      }
    }
    // loop over phi and find which phi cells to add
    for (int iPhi = 0; iPhi < extremaHCalFirstLayerPhi.second +1; iPhi++) {
      double lowPhi = hCalPhiOffset + iPhi * hCalPhiSize;
      double highPhi = hCalPhiOffset + (iPhi + 1) * hCalPhiSize;
      debug() << "HCal phi range  : " << lowPhi << " -  " << highPhi << endmsg;
      int lowId = floor((lowPhi - 0.5 * eCalPhiSize - eCalPhiOffset) / eCalPhiSize);
      int highId = floor((highPhi + 0.5 * eCalPhiSize - eCalPhiOffset) / eCalPhiSize);
      debug() << "ECal phi range  : " << lowId * eCalPhiSize + eCalPhiOffset << " -  "
             << highId * eCalPhiSize + eCalPhiOffset << endmsg;
      std::vector<uint> neighbours;
      for (int idPhiToAdd = lowId; idPhiToAdd <= highId; idPhiToAdd++) {
        neighbours.push_back(det::utils::cyclicNeighbour(idPhiToAdd, extremaECalLastLayerPhi));
      }
      debug() << "HCal phi id  : " << iPhi << endmsg;
      debug() << "Found ECal Neighbours in phi : " << neighbours.size() << endmsg;
      for (auto id : neighbours) {
        debug() << "ECal Neighbours id : " << id << endmsg;
      }
      phiNeighbours.insert(std::pair<uint, std::vector<uint>>(iPhi, neighbours));
    }
    // add neighbours to both ecal cell and hcal cells
    dd4hep::DDSegmentation::CellID ecalCellId = 0;
    dd4hep::DDSegmentation::CellID hcalCellId = 0;
    (*decoderECalBarrel)["system"].set(ecalCellId, 5);
    (*decoderECalBarrel)[m_activeFieldNamesSegmented[0]].set(ecalCellId, eCalLastLayer);
    (*decoderHCalBarrel)["system"].set(hcalCellId, 8);
    // loop over nested hcal cells
    if (m_readoutNamesNested.size()!=0){
      (*decoderHCalBarrel)[m_activeFieldNamesNested[0]].set(hcalCellId, 0);
      for (auto iZ : etaNeighbours) {
	(*decoderHCalBarrel)[m_activeFieldNamesNested[2]].set(hcalCellId, iZ.first);
	for (auto iMod : phiNeighbours) {
	  (*decoderHCalBarrel)[m_activeFieldNamesNested[1]].set(hcalCellId, iMod.first);
	  for (auto iEta : iZ.second) {
	    (*decoderECalBarrel)["eta"].set(ecalCellId, iEta);
	    for (auto iPhi : iMod.second) {
	      (*decoderECalBarrel)["phi"].set(ecalCellId, iPhi);
	      map.find(hcalCellId)->second.push_back(ecalCellId);
	      map.find(ecalCellId)->second.push_back(hcalCellId);
	      count++;
	    }
	  }
	}
      }
    }
    // loop over segmented hcal cells
    else {
      (*decoderHCalBarrel)[m_activeFieldNamesSegmented[1]].set(hcalCellId, 0);
      for (auto iEtaHCal : etaNeighbours) {
	(*decoderHCalBarrel)["eta"].set(hcalCellId, iEtaHCal.first); 
	for (auto iPhiHCal : phiNeighbours) {
	  (*decoderHCalBarrel)["phi"].set(hcalCellId, iPhiHCal.first);
	  for (auto iEta : iEtaHCal.second) {
	    (*decoderECalBarrel)["eta"].set(ecalCellId, iEta);
	    for (auto iPhi : iPhiHCal.second) {
	      (*decoderECalBarrel)["phi"].set(ecalCellId, iPhi);
	      map.find(hcalCellId)->second.push_back(ecalCellId);
	      map.find(ecalCellId)->second.push_back(hcalCellId);
	      count ++;
	    }
	  }
	}
      }
    }
  }
  if (msgLevel() <= MSG::DEBUG) {
    std::vector<int> counter;
    counter.assign(40, 0);
    for (const auto& item : map) {
      counter[item.second.size()]++;
    }
    for (uint iCount = 0; iCount < counter.size(); iCount++) {
      if (counter[iCount] != 0) {
        debug() << counter[iCount] << " cells have " << iCount << " neighbours" << endmsg;
      }
    }
  }
  debug() << "cells with neighbours across Calo boundaries: " << count << endmsg;

  std::unique_ptr<TFile> file(TFile::Open(m_outputFileName.c_str(), "RECREATE"));
  file->cd();
  TTree tree("neighbours", "Tree with map of neighbours");
  uint64_t saveCellId;
  std::vector<uint64_t> saveNeighbours;
  tree.Branch("cellId", &saveCellId, "cellId/l");
  tree.Branch("neighbours", &saveNeighbours);
  for (const auto& item : map) {
    saveCellId = item.first;
    saveNeighbours = item.second;
    tree.Fill();
  }
  file->Write();
  file->Close();

  return StatusCode::SUCCESS;
}

StatusCode CreateFCChhCaloNeighbours::finalize() { return Service::finalize(); }
