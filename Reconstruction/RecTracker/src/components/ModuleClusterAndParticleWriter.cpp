#include "ModuleClusterAndParticleWriter.h"

#include <cstring>
#include <string>
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Digitization/Segmentation.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "RecTracker/FCCDigitizationCell.h"
#include "TLorentzVector.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/MCParticleData.h"

DECLARE_COMPONENT(ModuleClusterAndParticleWriter)

ModuleClusterAndParticleWriter::ModuleClusterAndParticleWriter(const std::string& type, const std::string& nam,
                                                               const IInterface* parent)
    : GaudiTool(type, nam, parent),
      m_moduleID(-1),
      m_nChannels(0),
      m_nChannelsOn(0),
      m_moduleCache(),
      m_trackIDsPerCluster(100, 30) {}

StatusCode ModuleClusterAndParticleWriter::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  m_inputFile = TFile::Open(m_inputFilename.value().c_str());
  if (!m_inputFile) {
    error() << "Could not open " << m_inputFilename << endmsg;
    return StatusCode::FAILURE;
  }

  TTreeReader reader(m_inputTreename.value().c_str(), m_inputFile);
  TTreeReaderValue<std::vector<fcc::MCParticleData>> particles(reader, m_genParticlesName.value().c_str());
  TTreeReaderValue<std::vector<fcc::MCParticleData>> simparticles(reader, m_simParticlesName.value().c_str());

  // Setup ROOT I/O
  m_outputFile = TFile::Open(m_filename.value().c_str(), "RECREATE");
  if (!m_outputFile) {
    error() << "Could not open " << m_filename << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputFile->cd();

  // generated particles output tree
  m_genOutputTree = new TTree("genInfo", "TTree from ModuleClusterAndParticleWriter");
  if (!m_genOutputTree) {
    error() << "Could not create TTree "
            << "genInfo" << endmsg;
    return StatusCode::FAILURE;
  }
  m_genOutputTree->SetDirectory(0);

  m_genOutputTree->Branch("gen_eta", &m_gen_eta);
  m_genOutputTree->Branch("gen_phi", &m_gen_phi);
  m_genOutputTree->Branch("gen_pt", &m_gen_pt);
  m_genOutputTree->Branch("gen_energy", &m_gen_energy);
  m_genOutputTree->Branch("gen_status", &m_gen_status);
  m_genOutputTree->Branch("gen_pdgid", &m_gen_pdgid);
  m_genOutputTree->Branch("gen_charge", &m_gen_charge);
  m_genOutputTree->Branch("gen_vertexX", &m_gen_vertexX);
  m_genOutputTree->Branch("gen_vertexY", &m_gen_vertexY);
  m_genOutputTree->Branch("gen_vertexZ", &m_gen_vertexZ);

  // simulated particles output tree
  m_simOutputTree = new TTree("simInfo", "TTree from ModuleClusterAndParticleWriter");
  if (!m_simOutputTree) {
    error() << "Could not create TTree "
            << "simInfo" << endmsg;
    return StatusCode::FAILURE;
  }
  m_simOutputTree->SetDirectory(0);

  m_simOutputTree->Branch("sim_eta", &m_sim_eta);
  m_simOutputTree->Branch("sim_phi", &m_sim_phi);
  m_simOutputTree->Branch("sim_pt", &m_sim_pt);
  m_simOutputTree->Branch("sim_energy", &m_sim_energy);
  m_simOutputTree->Branch("sim_status", &m_sim_status);
  m_simOutputTree->Branch("sim_pdgid", &m_sim_pdgid);
  m_simOutputTree->Branch("sim_charge", &m_sim_charge);
  m_simOutputTree->Branch("sim_vertexX", &m_sim_vertexX);
  m_simOutputTree->Branch("sim_vertexY", &m_sim_vertexY);
  m_simOutputTree->Branch("sim_vertexZ", &m_sim_vertexZ);
  m_simOutputTree->Branch("sim_bits", &m_sim_bits);

  size_t nCharged = 0;
  size_t nChargedPrim = 0;
  size_t nPart = 0;
  size_t eventCount = 0;
  size_t nChargedGen = 0;

  // go through events
  while (reader.Next()) {
    // fill generated particles
    if (!(*particles).size()) {
      error() << "Particles: " << m_genParticlesName << " could not be accessed." << endmsg;
      return StatusCode::FAILURE;
    }
    for (auto& particle : (*particles)) {
      nPart++;
      m_gen_pdgid.push_back(particle.core.pdgId);
      m_gen_charge.push_back(particle.core.charge);
      m_gen_status.push_back(particle.core.status);
      TVector3 momentum(particle.core.p4.px, particle.core.p4.py, particle.core.p4.pz);
      float pt = momentum.Perp();
      float eta = momentum.Eta();
      float phi = momentum.Phi();
      TLorentzVector lvec;
      lvec.SetPtEtaPhiM(pt, eta, phi, particle.core.p4.mass);
      m_gen_pt.push_back(pt);
      m_gen_eta.push_back(eta);
      m_gen_phi.push_back(phi);
      m_gen_energy.push_back(lvec.E());
      m_gen_vertexX.push_back(particle.core.vertex.x);
      m_gen_vertexY.push_back(particle.core.vertex.y);
      m_gen_vertexZ.push_back(particle.core.vertex.z);
    }
    m_genOutputTree->Fill();
    // clear
    m_gen_eta.clear();
    m_gen_phi.clear();
    m_gen_pt.clear();
    m_gen_energy.clear();
    m_gen_charge.clear();
    m_gen_status.clear();
    m_gen_pdgid.clear();
    m_gen_vertexX.clear();
    m_gen_vertexY.clear();
    m_gen_vertexZ.clear();

    // fill simulated particles
    if (!(*simparticles).size()) {
      error() << "Particles: " << m_simParticlesName << " could not be accessed." << endmsg;
      return StatusCode::FAILURE;
    }
    for (auto& particle : (*simparticles)) {
      m_sim_pdgid.push_back(particle.core.pdgId);
      m_sim_charge.push_back(particle.core.charge);
      if (particle.core.charge != 0) {
        nCharged++;
        if (particle.core.status == 1) nChargedPrim++;
      }
      m_sim_status.push_back(particle.core.status);
      m_sim_bits.push_back(particle.core.bits);
      if (eventCount == 0) trackIDsParticles.insert(particle.core.bits);
      TVector3 momentum(particle.core.p4.px, particle.core.p4.py, particle.core.p4.pz);
      float pt = momentum.Perp();
      float eta = momentum.Eta();
      float phi = momentum.Phi();
      TLorentzVector lvec;
      lvec.SetPtEtaPhiM(pt, eta, phi, particle.core.p4.mass);
      m_sim_pt.push_back(pt);
      m_sim_eta.push_back(eta);
      m_sim_phi.push_back(phi);
      m_sim_energy.push_back(lvec.E());
      m_sim_vertexX.push_back(particle.core.vertex.x);
      m_sim_vertexY.push_back(particle.core.vertex.y);
      m_sim_vertexZ.push_back(particle.core.vertex.z);
    }
    m_simOutputTree->Fill();
    eventCount++;
    // clear
    m_sim_eta.clear();
    m_sim_phi.clear();
    m_sim_pt.clear();
    m_sim_energy.clear();
    m_sim_charge.clear();
    m_sim_bits.clear();
    m_sim_status.clear();
    m_sim_pdgid.clear();
    m_sim_vertexX.clear();
    m_sim_vertexY.clear();
    m_sim_vertexZ.clear();
  }
  m_outputFile->cd();
  m_simOutputTree->Write();
  m_genOutputTree->Write();
  m_inputFile->Close();

  m_outputTree = new TTree(m_treename.value().c_str(), "TTree from ModuleClusterAndParticleWriter");
  if (!m_outputTree) {
    error() << "Could not create TTree " << m_treename << endmsg;
    return StatusCode::FAILURE;
  }
  m_outputTree->SetDirectory(0);
  // Set the branches
  m_outputTree->Branch("event_nr", &m_eventNr);
  m_outputTree->Branch("moduleID", &m_moduleID);
  m_outputTree->Branch("nChannels", &m_nChannels);
  m_outputTree->Branch("nChannelsOn", &m_nChannelsOn);
  m_outputTree->Branch("nChannels_l0", &m_nChannels_l0);
  m_outputTree->Branch("nChannels_l1", &m_nChannels_l1);
  m_outputTree->Branch("s_x", &m_sX);
  m_outputTree->Branch("s_y", &m_sY);
  m_outputTree->Branch("s_z", &m_sZ);
  m_outputTree->Branch("g_x", &m_x);
  m_outputTree->Branch("g_y", &m_y);
  m_outputTree->Branch("g_z", &m_z);
  m_outputTree->Branch("trackIDsPerCluster", &m_trackIDsPerCluster);
  m_outputTree->Branch("nCells", &m_nCells);
  m_outputTree->Branch("size_x", &m_sizeX);
  m_outputTree->Branch("size_y", &m_sizeY);
  m_outputTree->Branch("energy", &m_energy);
  m_outputTree->Branch("time", &m_time);

  std::cout << "#nPart: " << nPart << " nCharged: " << nCharged << ", nChargedPrim: " << nChargedPrim << std::endl;

  return StatusCode::SUCCESS;
}

StatusCode ModuleClusterAndParticleWriter::write(const sim::FCCPlanarCluster& cluster, int eventNr) {
  // access cluster surface
  auto& clusterSurface = cluster.referenceSurface();
  // surface center
  const Acts::Vector3D& surfaceCenter = clusterSurface.center();
  // local cluster information: position
  auto parameters = cluster.parameters();
  Acts::Vector2D local(parameters[Acts::ParDef::eLOC_0], parameters[Acts::ParDef::eLOC_1]);
  /// prepare for calculating the
  Acts::Vector3D pos(0, 0, 0);
  // transform local into global position information
  clusterSurface.localToGlobal(local, Acts::Vector3D(0., 0., 0.), pos);
  // the associated detector element
  auto detectorElement = clusterSurface.associatedDetectorElement();
  auto digitationModule = detectorElement->digitizationModule();
  const Acts::Segmentation& segmentation = digitationModule->segmentation();
  // the cells
  const auto& cells = cluster.digitizationCells();
  /// cell IDs in lx and ly
  std::vector<int> cell_IDx;
  std::vector<int> cell_IDy;
  for (auto& cell : cells) {
    // cell identification
    cell_IDx.push_back(cell.channel0);
    cell_IDy.push_back(cell.channel1);
  }
  // fill the cluster size
  short int sizeX =
      (*std::max_element(cell_IDx.begin(), cell_IDx.end())) - (*std::min_element(cell_IDx.begin(), cell_IDx.end())) + 1;
  short int sizeY =
      (*std::max_element(cell_IDy.begin(), cell_IDy.end())) - (*std::min_element(cell_IDy.begin(), cell_IDy.end())) + 1;

  long long int moduleID = detectorElement->identify();

  for (auto& trackIDs : cluster.trackIDs) {
    trackIDsClusters.insert(trackIDs);
  }

  // first check if we are still on the same surface
  if (moduleID == m_moduleID) {
    // we are still on the same surface - update
    m_moduleCache.update(cells.size(), pos.x(), pos.y(), pos.z(), cluster.trackIDs, sizeX, sizeY, cluster.energy,
                         cluster.time);
  } else {
    // update module cache and parameters
    newModule(eventNr, moduleID, segmentation.binUtility().bins(), cells.size(), segmentation.binUtility().bins(0),
              segmentation.binUtility().bins(1), surfaceCenter.x(), surfaceCenter.y(), surfaceCenter.z(), pos.x(),
              pos.y(), pos.z(), cluster.trackIDs, sizeX, sizeY, cluster.energy, cluster.time);
  }

  return StatusCode::SUCCESS;
}

StatusCode ModuleClusterAndParticleWriter::finalize() {
  if (GaudiTool::finalize().isFailure()) {
    return StatusCode::FAILURE;
  }

  size_t counter = 0;
  for (auto& trackID : trackIDsParticles) {
    auto search = trackIDsClusters.find(trackID);
    if (search != trackIDsClusters.end()) {
      counter++;
    }
  }
  // write out all clusters
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return StatusCode::SUCCESS;
}
