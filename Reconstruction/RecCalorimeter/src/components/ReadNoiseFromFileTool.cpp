#include "ReadNoiseFromFileTool.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"

// Root
#include "TFile.h"
#include "TH1F.h"
#include "TMath.h"

DECLARE_TOOL_FACTORY(ReadNoiseFromFileTool)

ReadNoiseFromFileTool::ReadNoiseFromFileTool(const std::string& type, const std::string& name,
					     const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IReadNoiseFileTool>(this);
}

StatusCode ReadNoiseFromFileTool::initialize() {
  // Get GeoSvc
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  // Initialize random service
  if (service("RndmGenSvc", m_randSvc).isFailure()) {
    error() << "Couldn't get RndmGenSvc!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(0., 1.));

  // open and check file, read the histograms with noise constants
  if (ReadNoiseFromFileTool::initNoiseFromFile().isFailure()) {
    error() << "Couldn't open file with noise constants!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Get PhiEta segmentation
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation." << endmsg;
    return StatusCode::FAILURE;
  }

  debug() << "Filter noise threshold: " << m_filterThreshold << "*sigma" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  return sc;
}

StatusCode ReadNoiseFromFileTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}

StatusCode ReadNoiseFromFileTool::initNoiseFromFile() {
  // check if file exists
  if (m_noiseFileName.empty()) {
    error() << "Name of the file with noise values not set" << endmsg;
    return StatusCode::FAILURE;
  }
  TFile file(m_noiseFileName.value().c_str(), "READ");
  if (file.IsZombie()) {
    error() << "Couldn't open the file with noise constants" << endmsg;
    return StatusCode::FAILURE;
  } else {
    info() << "Opening the file with noise constants: " << m_noiseFileName << endmsg;
  }

  std::string elecNoiseLayerHistoName, pileupLayerHistoName;
  // Read the histograms with electronics noise and pileup from the file
  for (unsigned i = 0; i < m_numRadialLayers; i++) {
    elecNoiseLayerHistoName = m_elecNoiseHistoName + std::to_string(i + 1);
    debug() << "Getting histogram with a name " << elecNoiseLayerHistoName << endmsg;
    m_histoElecNoiseConst.push_back(*dynamic_cast<TH1F*>(file.Get(elecNoiseLayerHistoName.c_str())));
    if (m_histoElecNoiseConst.at(i).GetNbinsX() < 1) {
      error() << "Histogram  " << elecNoiseLayerHistoName
              << " has 0 bins! check the file with noise and the name of the histogram!" << endmsg;
      return StatusCode::FAILURE;
    }
    if (m_addPileup) {
      pileupLayerHistoName = m_pileupHistoName + std::to_string(i + 1);
      debug() << "Getting histogram with a name " << pileupLayerHistoName << endmsg;
      m_histoPileupConst.push_back(*dynamic_cast<TH1F*>(file.Get(pileupLayerHistoName.c_str())));
      if (m_histoPileupConst.at(i).GetNbinsX() < 1) {
        error() << "Histogram  " << pileupLayerHistoName
                << " has 0 bins! check the file with noise and the name of the histogram!" << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  // Check if we have same number of histograms (all layers) for pileup and electronics noise
  if (m_histoElecNoiseConst.size() == 0) {
    error() << "No histograms with noise found!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_addPileup) {
    if (m_histoElecNoiseConst.size() != m_histoPileupConst.size()) {
      error() << "Missing histograms! Different number of histograms for electronics noise and pileup!!!!" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

double ReadNoiseFromFileTool::getNoiseConstantPerCell(int64_t aCellId) {

  double elecNoise = 0.;
  double pileupNoise = 0.;

  // Get cell coordinates: eta and radial layer
  double cellEta = m_segmentation->eta(aCellId);
  // Take readout, bitfield from GeoSvc
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  decoder->setValue(aCellId);
  unsigned cellLayer = (*decoder)[m_activeFieldName];

  // All histograms have same binning, all bins with same size
  // Using the histogram in the first layer to get the bin size
  unsigned index = 0;
  if (m_histoElecNoiseConst.size() != 0) {
    int Nbins = m_histoElecNoiseConst.at(index).GetNbinsX();
    double deltaEtaBin =
        (m_histoElecNoiseConst.at(index).GetBinLowEdge(Nbins) + m_histoElecNoiseConst.at(index).GetBinWidth(Nbins) -
         m_histoElecNoiseConst.at(index).GetBinLowEdge(1)) /
        Nbins;
    // find the eta bin for the cell
    int ibin = floor(fabs(cellEta) / deltaEtaBin) + 1;
    if (ibin > Nbins) {
      error() << "eta outside range of the histograms! Cell eta: " << cellEta << " Nbins in histogram: " << Nbins
              << endmsg;
      ibin = Nbins;
    }
    // Check that there are not more layers than the constants are provided for
    if (cellLayer < m_histoElecNoiseConst.size()) {
      elecNoise = m_histoElecNoiseConst.at(cellLayer).GetBinContent(ibin);
      if (m_addPileup) {
        pileupNoise = m_histoPileupConst.at(cellLayer).GetBinContent(ibin);
      }
    } else {
      error()
          << "More radial layers than we have noise for!!!! Using the last layer for all histograms outside the range."
          << endmsg;
    }
  } else {
    error() << "No histograms with noise constants!!!!! " << endmsg;
  }

  // Total noise: electronics noise + pileup
  double totalNoise = sqrt(pow(elecNoise, 2) + pow(pileupNoise, 2));

  if (totalNoise < 1e-3) {
    warning() << "Zero noise: cell eta " << cellEta << " layer " << cellLayer << " noise " << totalNoise << endmsg;
  }

  return totalNoise;
}
