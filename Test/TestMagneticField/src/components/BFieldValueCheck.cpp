#include "BFieldValueCheck.h"
#include "ACTS/Utilities/Units.hpp"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TTree.h"
#include <fstream>
#include <limits>
#include <utility>

BFieldValueCheck::BFieldValueCheck(const std::string& name, ISvcLocator* svcLoc) : Service(name, svcLoc) {}

StatusCode BFieldValueCheck::initialize() {
  if (Service::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  if (!m_bFieldSvc) {
    error() << "No magnetic field service handed over." << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_fieldMapFile.empty()) {
    error() << "No fieldmap file for comparison handed over!" << endmsg;
    return StatusCode::SUCCESS;
  }

  /// [1] Read in field map file for comparison
  // Grid position points in r and z
  std::vector<double> rPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector2D> bField;
  // [1] Read in file and fill values
  debug() << "Opening file: " << m_fieldMapFile << endmsg;
  std::unique_ptr<TFile> inputFile(TFile::Open(m_fieldMapFile.value().c_str()));
  TTree* tree = (TTree*)inputFile->Get(m_treeName.value().c_str());
  Int_t entries = tree->GetEntries();

  double r, z;
  double Br, Bz;

  tree->SetBranchAddress("r", &r);
  tree->SetBranchAddress("z", &z);

  tree->SetBranchAddress("Br", &Br);
  tree->SetBranchAddress("Bz", &Bz);

  // reserve size
  rPos.reserve(entries);
  zPos.reserve(entries);
  bField.reserve(entries);

  // read in all b Field values
  for (int i = 0; i < entries; i++) {
    tree->GetEvent(i);
    rPos.push_back(r * Acts::units::_mm * m_lengthScalor);
    zPos.push_back(z * Acts::units::_mm * m_lengthScalor);
    bField.push_back(Acts::Vector2D(Br * Acts::units::_T * m_lengthScalor, Bz * Acts::units::_T * m_bFieldScalor));
  }
  inputFile->Close();

  if (rPos.size() != bField.size() || rPos.size() != zPos.size()) {
    error() << "Values in r, z or Bfield values do not have the same number! # r values: " << rPos.size()
            << ", # z values: " << zPos.size() << ", # bField values: " << bField.size() << std::endl;
    return StatusCode::FAILURE;
  }
  StatusCode sc = StatusCode::SUCCESS;

  // Now loop through values and check if values are correct
  for (size_t i = 0; i < rPos.size(); i++) {
    // create position from r and z value
    Acts::Vector3D pos(rPos.at(i), 0., zPos.at(i));
    // the read in bField
    Acts::Vector3D bFieldVec(bField.at(i).x(), 0., bField.at(i).y());
    // the reference bField value
    auto refBField = m_bFieldSvc->getField(pos);
    // check r value
    sc = equalTo(refBField.perp() / Acts::units::_mm, bFieldVec.perp());
    if (sc != StatusCode::SUCCESS) return sc;
    // check z value
    sc = equalTo(refBField.z() / Acts::units::_mm, bFieldVec.z());
    if (sc != StatusCode::SUCCESS) return sc;
  }
  return StatusCode::SUCCESS;
}

StatusCode BFieldValueCheck::finalize() { return StatusCode::SUCCESS; }

StatusCode BFieldValueCheck::equalTo(const double& a, const double& b) const {
  // check if relative error is smaller or equal to epsilon
  bool equal = (std::fabs(a - b) <= (m_epsilon * std::fabs(a + b)));
  if (!equal) {
    error() << "Values: " << a << " and " << b << " are not equal to each other!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

DECLARE_SERVICE_FACTORY(BFieldValueCheck)
