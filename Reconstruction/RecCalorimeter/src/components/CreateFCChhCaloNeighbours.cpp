#include "CreateFCChhCaloNeighbours.h"

#include "DD4hep/LCDD.h"
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

#include "TFile.h"
#include "TTree.h"

DECLARE_SERVICE_FACTORY(CreateFCChhCaloNeighbours)

CreateFCChhCaloNeighbours::CreateFCChhCaloNeighbours(const std::string& aName, ISvcLocator* aSL)
    : base_class(aName, aSL) {}

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
  //////////////////////////////////
  /// SEGMENTED ETA-PHI VOLUMES  ///
  //////////////////////////////////

  // will be used for volume connecting
  int eCalLastLayer;
  std::pair<int, int> extremaECalLastLayerPhi;
  std::pair<int, int> extremaECalLastLayerEta;
  double eCalEtaOffset = 0;
  double eCalEtaSize = 0;
  double eCalPhiOffset = 0;
  double eCalPhiSize = 0;
  DD4hep::DDSegmentation::BitField64* decoderECalBarrel = nullptr;

  for (uint iSys = 0; iSys < m_readoutNamesSegmented.size(); iSys++) {
    // Check if readouts exist
    info() << "Readout: " << m_readoutNamesSegmented[iSys] << endmsg;
    if (m_geoSvc->lcdd()->readouts().find(m_readoutNamesSegmented[iSys]) == m_geoSvc->lcdd()->readouts().end()) {
      error() << "Readout <<" << m_readoutNamesSegmented[iSys] << ">> does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
    // get PhiEta segmentation
    DD4hep::DDSegmentation::GridPhiEta* segmentation;
    segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
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
    // Loop over all cells in the calorimeter and retrieve existing cellIDs
    // Loop over active layers
    std::vector<std::pair<int, int>> extrema;
    extrema.push_back(std::make_pair(0, m_activeVolumesNumbersSegmented[iSys] - 1));
    extrema.push_back(std::make_pair(0, 0));
    extrema.push_back(std::make_pair(0, 0));
    for (unsigned int ilayer = 0; ilayer < m_activeVolumesNumbersSegmented[iSys]; ilayer++) {
      // Get VolumeID
      (*decoder)[m_fieldNamesSegmented[iSys]] = m_fieldValuesSegmented[iSys];
      (*decoder)[m_activeFieldNamesSegmented[iSys]] = ilayer;
      (*decoder)["eta"] = 0;
      (*decoder)["phi"] = 0;
      uint64_t volumeId = decoder->getValue();

      // Get number of segmentation cells within the active volume
      auto numCells = det::utils::numberOfCells(volumeId, *segmentation);
      debug() << "Number of segmentation cells in (phi,eta): " << numCells << endmsg;
      extrema[1] = std::make_pair(0, numCells[0] - 1);
      extrema[2] = std::make_pair(numCells[2], numCells[1] - 1);
      // for layer N-1 of ECal barrel,  will be used for volume connecting
      if (ilayer == (m_activeVolumesNumbersSegmented[iSys] - 1) && m_fieldNamesSegmented[iSys] == "system" &&
          m_fieldValuesSegmented[iSys] == 5) {
        eCalLastLayer = m_activeVolumesNumbersSegmented[iSys] - 1;
        extremaECalLastLayerPhi = std::make_pair(0, numCells[0] - 1);
        extremaECalLastLayerEta = std::make_pair(numCells[2], numCells[1] - 1);
      }
      // Loop over segmenation cells
      for (unsigned int iphi = 0; iphi < numCells[0]; iphi++) {
        for (unsigned int ieta = numCells[2]; ieta < numCells[1]; ieta++) {
          (*decoder)["phi"] = iphi;
          (*decoder)["eta"] = ieta;
          uint64_t cellId = decoder->getValue();
          map.insert(std::pair<uint64_t, std::vector<uint64_t>>(
              cellId, det::utils::neighbours(*decoder, {m_activeFieldNamesSegmented[iSys], "phi", "eta"}, extrema,
                                             cellId, {false, true, false}, true)));
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

  //////////////////////////////////
  ///      NESTED VOLUMES        ///
  //////////////////////////////////

  // will be used for volume connecting
  std::pair<int, int> extremaHCalFirstLayerPhi;
  std::pair<int, int> extremaHCalFirstLayerZ;
  DD4hep::DDSegmentation::BitField64* decoderHCalBarrel = nullptr;

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
    if (m_fieldNameNested == "system" && m_fieldValuesNested[iSys] == 6) {
      decoderHCalBarrel = decoder;
    }
    // Get VolumeID
    (*decoder)[m_fieldNameNested] = m_fieldValuesNested[iSys];
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
    if (m_fieldNameNested == "system" && m_fieldValuesNested[iSys] == 6) {
      extremaHCalFirstLayerPhi =
          std::make_pair(0, activeVolumesNumbersNested.find(m_activeFieldNamesNested[1])->second - 1);
      extremaHCalFirstLayerZ =
          std::make_pair(0, activeVolumesNumbersNested.find(m_activeFieldNamesNested[2])->second - 1);
    }
    for (unsigned int ilayer = 0; ilayer < activeVolumesNumbersNested.find(m_activeFieldNamesNested[0])->second;
         ilayer++) {
      for (unsigned int iphi = 0; iphi < activeVolumesNumbersNested.find(m_activeFieldNamesNested[1])->second; iphi++) {
        for (unsigned int iz = 0; iz < activeVolumesNumbersNested.find(m_activeFieldNamesNested[2])->second; iz++) {

          (*decoder)[m_activeFieldNamesNested[0]] = ilayer;
          (*decoder)[m_activeFieldNamesNested[1]] = iphi;
          (*decoder)[m_activeFieldNamesNested[2]] = iz;
          uint64_t volumeId = decoder->getValue();

          map.insert(std::pair<uint64_t, std::vector<uint64_t>>(
              volumeId, det::utils::neighbours(*decoder, {m_activeFieldNamesNested[0], m_activeFieldNamesNested[1],
                                                          m_activeFieldNamesNested[2]},
                                               extrema, volumeId, {false, true, false}, true)));
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
  if (m_connectBarrels) {
    // first check if ECAL barrel (system==5) and HCal barrel (system==6) are configured
    if (decoderECalBarrel == nullptr || decoderHCalBarrel == nullptr) {
      error() << "ECAL barrel and/or HCal barrel are not configured correctly!" << endmsg;
      return StatusCode::FAILURE;
    }
    // print how many cells in each dimensions will be matched
    info() << "ECAL layer " << eCalLastLayer << " is a neighbour of HCAL layer 0." << endmsg;
    info() << "ECAL phi cells " << extremaECalLastLayerPhi.first << " - " << extremaECalLastLayerPhi.second
           << " will be matched to HCAL " << m_activeFieldNamesNested[1] << "(s) " << extremaHCalFirstLayerPhi.first
           << " - " << extremaHCalFirstLayerPhi.second << endmsg;
    info() << "ECAL eta cells " << extremaECalLastLayerEta.first << " - " << extremaECalLastLayerEta.second
           << " will be matched to HCAL " << m_activeFieldNamesNested[2] << "(s) " << extremaHCalFirstLayerZ.first
           << " - " << extremaHCalFirstLayerZ.second << endmsg;
    std::unordered_map<uint, std::vector<uint>> etaNeighbours;
    std::unordered_map<uint, std::vector<uint>> phiNeighbours;
    double hCalPhiSize = 2 * M_PI / (extremaHCalFirstLayerPhi.second - extremaHCalFirstLayerPhi.first + 1);
    // loop over z and find which eta cells to add
    for (int iZ = 0; iZ < extremaHCalFirstLayerZ.second + 1; iZ++) {
      double lowZ = m_hCalZOffset + iZ * m_hCalZSize;
      double highZ = m_hCalZOffset + (iZ + 1) * m_hCalZSize;
      double lowEta = 0, highEta = 0;
      if (fabs(lowZ) < 1e-3) {
        lowEta = 0;
      } else {
        lowEta = lowZ / fabs(lowZ) * (-log(fabs(tan(atan(m_hCalRinner / lowZ) / 2))));
      }
      if (fabs(highZ) < 1e-3) {
        highEta = 0;
      } else {
        highEta = highZ / fabs(highZ) * (-log(fabs(tan(atan(m_hCalRinner / highZ) / 2))));
      }
      int lowId = floor((lowEta + 0.5 * eCalEtaSize - eCalEtaOffset) / eCalEtaSize);
      int highId = floor((highEta + 0.5 * eCalEtaSize - eCalEtaOffset) / eCalEtaSize);
      std::vector<uint> neighbours;
      for (int idEtaToAdd = lowId; idEtaToAdd <= highId; idEtaToAdd++) {
        if (idEtaToAdd >= extremaECalLastLayerEta.first && idEtaToAdd <= extremaECalLastLayerEta.second) {
          neighbours.push_back(idEtaToAdd);
        }
      }
      etaNeighbours.insert(std::pair<uint, std::vector<uint>>(iZ, neighbours));
    }
    // loop over phi and find which phi cells to add
    for (int iPhi = 0; iPhi < extremaHCalFirstLayerPhi.second + 1; iPhi++) {
      double lowPhi = m_hCalPhiOffset + iPhi * hCalPhiSize;
      double highPhi = m_hCalPhiOffset + (iPhi + 1) * hCalPhiSize;
      int lowId = floor((lowPhi + 0.5 * eCalPhiSize - eCalPhiOffset) / eCalPhiSize);
      int highId = floor((highPhi + 0.5 * eCalPhiSize - eCalPhiOffset) / eCalPhiSize);
      std::vector<uint> neighbours;
      for (int idPhiToAdd = lowId; idPhiToAdd <= highId; idPhiToAdd++) {
        neighbours.push_back(det::utils::cyclicNeighbour(idPhiToAdd, extremaECalLastLayerPhi));
      }
      phiNeighbours.insert(std::pair<uint, std::vector<uint>>(iPhi, neighbours));
    }
    // add neighbours to both ecal cell and hcal cell
    (*decoderECalBarrel)[m_activeFieldNamesSegmented[0]] = eCalLastLayer;
    (*decoderHCalBarrel)[m_activeFieldNamesNested[0]] = 0;
    for (auto iZ : etaNeighbours) {
      (*decoderHCalBarrel)[m_activeFieldNamesNested[2]] = iZ.first;
      for (auto iMod : phiNeighbours) {
        (*decoderHCalBarrel)[m_activeFieldNamesNested[1]] = iMod.first;
        uint64_t hcalCellId = decoderHCalBarrel->getValue();
        for (auto iEta : iZ.second) {
          (*decoderECalBarrel)["eta"] = iEta;
          for (auto iPhi : iMod.second) {
            (*decoderECalBarrel)["phi"] = iPhi;
            uint64_t ecalCellId = decoderECalBarrel->getValue();
            map.find(hcalCellId)->second.push_back(ecalCellId);
            map.find(ecalCellId)->second.push_back(hcalCellId);
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
        info() << counter[iCount] << " cells have " << iCount << " neighbours" << endmsg;
      }
    }
  }

  TFile file("neighbours_map.root", "RECREATE");
  file.cd();
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
  file.Write();
  file.Close();

  return StatusCode::SUCCESS;
}

StatusCode CreateFCChhCaloNeighbours::finalize() { return Service::finalize(); }
