// local
#include "SimG4Common/CharginoPlus.h"

namespace sim {
CharginoPlus* CharginoPlus::s_theInstance = NULL;
int CharginoPlus::pdgID = 1000024;
std::string CharginoPlus::name = "s_chi_plus_1";

CharginoPlus* CharginoPlus::Definition(G4double mass, G4double width, G4bool stable, G4double lifetime) {
  if (s_theInstance != 0 && (mass >= 0. || width >= 0. || lifetime >= 0.)) {
    G4ExceptionDescription description;
    description << "Trying to redefine the Chargino Plus properties after it has been constructed is not allowed";
    G4Exception("CharginoPlus", "FailedRedefinition", FatalException, description);
    abort();
  }

  if (s_theInstance != 0) {
    return s_theInstance;
  }

  //    Arguments for constructor are as follows
  //               name             mass          width         charge
  //             2*spin           parity  C-conjugation
  //          2*Isospin       2*Isospin3       G-parity
  //               type    lepton number  baryon number   PDG encoding
  //             stable         lifetime    decay table
  //             shortlived      subType    anti_encoding
  if (mass >= 0) {
    G4ParticleDefinition* anInstance =
        new G4ParticleDefinition(name, mass, width, +1. * CLHEP::eplus, 1, 0, 0, 0, 0, 0, "supersymmetric", 0, 0, pdgID,
                                 stable, lifetime, NULL, false, "CharginoPlus");

    s_theInstance = reinterpret_cast<CharginoPlus*>(anInstance);
    std::cout << "Defining chargino plus with Mass: " << mass << " and pdgID: " << pdgID << std::endl;
    return s_theInstance;
  } else {
    G4ExceptionDescription description;
    description << "Trying to create a particle with default constructor is not allowed";
    G4Exception("CharginoPlus", "DefaultConstructorCalled", FatalException, description);
    abort();
  }
}
}
