#include "DelphesSaveChargedParticles.h"

// Delphes
#include "classes/DelphesClasses.h"
#include "modules/Delphes.h"

// datamodel
#include "datamodel/MCParticleCollection.h"
#include "datamodel/TrackStateCollection.h"
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"
#include "datamodel/TaggedParticleCollection.h"
// ROOT
#include "SimDelphesInterface/ParticleStatus.h"
#include "TObjArray.h"
#include <TMatrixD.h>

static const double defaultMagneticField = 2.;
static const double mTommConversionFactor = 1000.;

// Covariance conversion to ACTS format
TMatrixDSym CovToACTS(TMatrixDSym Cov, double ct /*cot(theta)*/, double C /*half curvature*/,
		      double fB=defaultMagneticField /*magnetic field in T*/)
{
  TMatrixDSym cACTS(6); cACTS.Zero();
  Double_t b = -0.29979 * fB / 2. / mTommConversionFactor;
  //
  // Fill derivative matrix
  TMatrixD A(5, 5); A.Zero();
  A(0, 0) = mTommConversionFactor;    // D-D  conversion to mm
  A(1, 2) = 1.0;    // phi0-phi0
  A(2, 4) = 1.0/(TMath::Sqrt(1.0 + ct*ct) * b); // q/p-C
  A(3, 1) = mTommConversionFactor;    // z0-z0 conversion to mm
  A(4, 3) = -1.0 / (1.0 + ct*ct); // theta - cot(theta)
  A(4, 4) = -C*ct / (b*pow(1.0 + ct*ct,3.0/2.0)); // q/p-cot(theta)
  //
  TMatrixDSym Cv = Cov;
  TMatrixD At(5, 5);
  At.Transpose(A);
  Cv.Similarity(At);
  TMatrixDSub(cACTS, 0, 4, 0, 4) = Cv;
  cACTS(5, 5) = 0.1;  // Currently undefined: set to arbitrary value to avoid crashes
  //
  return cACTS;
}

DECLARE_COMPONENT(DelphesSaveChargedParticles)

DelphesSaveChargedParticles::DelphesSaveChargedParticles(const std::string& aType,
                                                         const std::string& aName,
                                                         const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IDelphesSaveOutputTool>(this);
  declareProperty("particles", m_particles, "Handle the particles to be saved");
  declareProperty("particles_trkCov", m_particles_trkCov, "Handle the particles to be saved");
  declareProperty("mcAssociations", m_mcAssociations, "Handle to associate particles with MCParticles");
  declareProperty("isolationTags", m_isolationTaggedParticles, "Handle for isolation tags");
}

DelphesSaveChargedParticles::~DelphesSaveChargedParticles() {}

StatusCode DelphesSaveChargedParticles::initialize() { 
  
  
 // if (m_saveTrkCov) {
 //  m_particles_trkCov = std::make_unique<DataHandle<fcc::TrackStateCollection>>( "particles_trkCov", Gaudi::DataHandle::Writer, this);
 // 
  
  return GaudiTool::initialize(); }

StatusCode DelphesSaveChargedParticles::finalize() { return GaudiTool::finalize(); }

StatusCode DelphesSaveChargedParticles::saveOutput(Delphes& delphes, const fcc::MCParticleCollection& mcParticles) {
  // Create the collections
  auto colParticles = m_particles.createAndPut();
    fcc::TrackStateCollection* colParticles_trkCov = nullptr;
  if (m_saveTrkCov) {
    colParticles_trkCov = m_particles_trkCov.createAndPut();
  }
  auto ascColParticlesToMC = m_mcAssociations.createAndPut();

  fcc::TaggedParticleCollection* colITags(nullptr);
  if (m_saveIso) {
    colITags = m_isolationTaggedParticles.createAndPut();
  }

  // Get the longitudinal magnetic field component
  double Bz = delphes.GetDouble("Bz", defaultMagneticField);
  
  const TObjArray* delphesColl = delphes.ImportArray(m_delphesArrayName.value().c_str());
  if (delphesColl == nullptr) {
    warning() << "Delphes collection " << m_delphesArrayName << " not present. Skipping it." << endmsg;
    return StatusCode::SUCCESS;
  }
  for (int j = 0; j < delphesColl->GetEntries(); j++) {
    auto cand = static_cast<Candidate*>(delphesColl->At(j));
    auto particle = colParticles->create();

    if (m_saveTrkCov) {
      auto cov_d = CovToACTS(cand->TrackCovariance, cand->CtgTheta, cand->C, Bz);
      auto t = colParticles_trkCov->create();
      t.d0(mTommConversionFactor * cand->D0);
      t.z0(mTommConversionFactor * cand->DZ);
      t.phi(cand->Phi);
      t.theta(TMath::ATan2(1.0, cand->CtgTheta));
      Double_t b = -0.29979 * Bz / 2. / mTommConversionFactor; //mag field in T
      t.qOverP(cand->C / (b*TMath::Sqrt(1 + cand->CtgTheta*cand->CtgTheta)));
      std::array<float,15> t_c;
      // save upper right triangle, row first
      t_c[0] = cov_d[0][0];
      t_c[1] = cov_d[0][1];
      t_c[2] = cov_d[0][2];
      t_c[3] = cov_d[0][3];
      t_c[4] = cov_d[0][4];
      t_c[5] = cov_d[1][1];
      t_c[6] = cov_d[1][2];
      t_c[7] = cov_d[1][3];
      t_c[8] = cov_d[1][4];
      t_c[9] = cov_d[2][2];
      t_c[10] = cov_d[2][3];
      t_c[11] = cov_d[2][4];
      t_c[12] = cov_d[3][3];
      t_c[13] = cov_d[3][4];
      t_c[14] = cov_d[4][4];
      t.cov(t_c);
    }

    auto& barePart = particle.core();
    barePart.pdgId = cand->PID;
    barePart.status = cand->Status;
    barePart.p4.px = cand->Momentum.Px();
    barePart.p4.py = cand->Momentum.Py();
    barePart.p4.pz = cand->Momentum.Pz();
    barePart.p4.mass = cand->Momentum.M();
    barePart.charge = cand->Charge;
    barePart.vertex.x = cand->InitialPosition.X();
    barePart.vertex.y = cand->InitialPosition.Y();
    barePart.vertex.z = cand->InitialPosition.Z();

    // Isolation-tag info
    float iTagValue = 0;
    if (colITags != nullptr) {
      auto iTag = colITags->create();
      iTag.tag(cand->IsolationVar);
      iTag.particle(particle);
      iTagValue = iTag.tag();
    }

    // Reference to MC - Delphes holds references to all objects related to the <T> object, only one relates to MC
    // particle
    auto relation = ascColParticlesToMC->create();
    if (cand->GetCandidates()->GetEntries() > 0) {
      auto refCand = static_cast<Candidate*>(cand->GetCandidates()->At(0));

      // find refCand in mcParticle collection
      int index = -1;
      for (int k = 0; k < mcParticles.size(); k++) {
        if (mcParticles.at(k).core().bits == refCand->GetUniqueID()) {
          index = k;
          break;
        }
      }

      if (index > 0) {
        barePart.bits = static_cast<unsigned>(ParticleStatus::kMatched);
        relation.rec(particle);
        relation.sim(mcParticles.at(index));
      } else {
        barePart.bits = static_cast<unsigned>(ParticleStatus::kUnmatched);
        warning() << "Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
      }
    } else {
      barePart.bits = static_cast<unsigned>(ParticleStatus::kUnmatched);
      warning() << "Can't build relation from Electron/Muon/ChHadron to MC particle!" << std::endl;
    }

    // Debug: print FCC-EDM track info
    if (msgLevel() <= MSG::DEBUG) {

      double energy = sqrt(particle.p4().px * particle.p4().px + particle.p4().py * particle.p4().py +
                           particle.p4().pz * particle.p4().pz + particle.p4().mass * particle.p4().mass);
      double recE =
          sqrt(relation.rec().p4().px * relation.rec().p4().px + relation.rec().p4().py * relation.rec().p4().py +
               relation.rec().p4().pz * relation.rec().p4().pz + relation.rec().p4().mass * relation.rec().p4().mass);
      double simE =
          sqrt(relation.sim().p4().px * relation.sim().p4().px + relation.sim().p4().py * relation.sim().p4().py +
               relation.sim().p4().pz * relation.sim().p4().pz + relation.sim().p4().mass * relation.sim().p4().mass);

      debug() << "Track: "
              << " Id: " << std::setw(3) << j + 1 << " Pdg: " << std::setw(5) << particle.pdgId()
              << " Stat: " << std::setw(2) << particle.status() << " Bits: " << std::setw(2) << particle.bits();

      if (colITags != nullptr) {

        debug() << " ITag: " << std::setprecision(1) << std::setw(4) << iTagValue;
      }

      debug() << std::scientific << " Px: " << std::setprecision(2) << std::setw(9) << particle.p4().px
              << " Py: " << std::setprecision(2) << std::setw(9) << particle.p4().py << " Pz: " << std::setprecision(2)
              << std::setw(9) << particle.p4().pz << " E: " << std::setprecision(2) << std::setw(9) << energy
              << " M: " << std::setprecision(2) << std::setw(9) << particle.p4().mass << " Vx: " << std::setprecision(2)
              << std::setw(9) << particle.vertex().x << " Vy: " << std::setprecision(2) << std::setw(9)
              << particle.vertex().y << " Vz: " << std::setprecision(2) << std::setw(9) << particle.vertex().z
              << " Rel E: " << std::setprecision(2) << std::setw(9) << simE << " <-> " << std::setw(9) << recE
              << std::fixed << endmsg;
    }  // Debug
  }    // For - tracks
  return StatusCode::SUCCESS;
}
