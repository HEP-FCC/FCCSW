#include "BFieldWriter.h"
#include "ACTS/Utilities/Units.hpp"
#include "TFile.h"
#include "TTree.h"

BFieldWriter::BFieldWriter(const std::string& name, ISvcLocator* svcLoc) : Service(name, svcLoc) {}

StatusCode BFieldWriter::initialize() {
  if (Service::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  if (!m_bFieldSvc) {
    error() << "No magnetic field service handed over." << endmsg;
    return StatusCode::FAILURE;
  }
  // apply units to input
  m_rMin = m_rMin * Acts::units::_mm;
  m_rMax = m_rMax * Acts::units::_mm;
  m_zMax = m_zMax * Acts::units::_mm;
  m_zMin = m_zMin * Acts::units::_mm;

  std::unique_ptr<TFile> outputFile(TFile::Open(m_filename.value().c_str(), "RECREATE"));
  outputFile->cd();
  TTree* outputTree = new TTree(m_treename.value().c_str(), m_filename.value().c_str());
  if (!outputTree) throw std::bad_alloc();

  double z;
  outputTree->Branch("z", &z);
  double Bz;
  outputTree->Branch("Bz", &Bz);

  if (m_cartesian) {
    double x;
    outputTree->Branch("x", &x);
    double y;
    outputTree->Branch("y", &y);
    double Bx;
    outputTree->Branch("Bx", &Bx);
    double By;
    outputTree->Branch("By", &By);

    // the step sizes
    double stepX = fabs(m_rMax - m_rMin) / m_rBins;
    double stepY = fabs(m_rMax - m_rMin) / m_rBins;
    double stepZ = fabs(m_zMax - m_zMin) / m_zBins;

    // write out
    for (int i = 0; i < m_rBins; i++) {
      double raw_x = m_rMin + i * stepX;
      for (int j = 0; j < m_rBins; j++) {
        double raw_y = m_rMin + j * stepY;
        for (int k = 0; k < m_zBins; k++) {
          double raw_z = m_zMin + k * stepZ;
          Acts::Vector3D position(raw_x, raw_y, raw_z);
          if (m_bFieldSvc->isInside(position)) {
            auto bField = m_bFieldSvc->getField(position);

            x = raw_x / Acts::units::_mm;
            y = raw_y / Acts::units::_mm;
            z = raw_z / Acts::units::_mm;
            Bx = bField.x() / Acts::units::_T;
            By = bField.y() / Acts::units::_T;
            Bz = bField.z() / Acts::units::_T;

            outputTree->Fill();
          }
        }  // for z
      }    // for y
    }      // for x

  } else {
    double r;
    outputTree->Branch("r", &r);
    double Br;
    outputTree->Branch("Br", &Br);

        // the step sizes
    double stepR = fabs(m_rMax - m_rMin) / m_rBins;
    double stepZ = fabs(m_zMax - m_zMin) / m_zBins;
    double stepPhi = (2. * M_PI) / m_phiBins;
    double minPhi = -M_PI;

    // write out
    for (int i = 0; i < m_phiBins; i++) {
      double phi = minPhi + i * stepPhi;
      for (int k = 0; k < m_zBins; k++) {
        double raw_z = m_zMin + k * stepZ;
        for (int j = 0; j < m_rBins; j++) {
          double raw_r = m_rMin + j * stepR;
          Acts::Vector3D position(raw_r * cos(phi), raw_r * sin(phi), raw_z);
          if (m_bFieldSvc->isInside(position)) {
            auto bField = m_bFieldSvc->getField(position);
            z = raw_z / Acts::units::_mm;
            r = raw_r / Acts::units::_mm;
            Bz = bField.z() / Acts::units::_T;
            Br = bField.perp() / Acts::units::_T;
            outputTree->Fill();
          }
        }
      }
    }  // for
  }

  // Tear down ROOT I/O
  debug() << "Closing and Writing ROOT output File : " << endmsg;
  outputFile->cd();
  outputTree->Write();
  outputFile->Close();
  return StatusCode::SUCCESS;
}

StatusCode BFieldWriter::finalize() { return StatusCode::SUCCESS; }

DECLARE_SERVICE_FACTORY(BFieldWriter)
