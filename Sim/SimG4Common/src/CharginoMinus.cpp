// local
#include "SimG4Common/CharginoMinus.h"

namespace sim {
CharginoMinus* CharginoMinus::s_theInstance = NULL;
int CharginoMinus::pdgID = -1000024;
std::string CharginoMinus::name = "s_chi_minus_1";

CharginoMinus* CharginoMinus::Definition(G4double mass, G4double width, G4bool stable, G4double lifetime) {
  if (s_theInstance != 0 && (mass >= 0. || width >= 0. || lifetime >= 0.)) {
    G4ExceptionDescription description;
    description << "Trying to redefine the Chargino Minus properties after it has been constructed is not allowed";
    G4Exception("CharginoMinus", "FailedRedefinition", FatalException, description);
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
        new G4ParticleDefinition(name, mass, width, -1. * CLHEP::eplus, 1, 0, 0, 0, 0, 0, "supersymmetric", 0, 0, pdgID,
                                 stable, lifetime, NULL, false, "CharginoMinus");
    s_theInstance = reinterpret_cast<CharginoMinus*>(anInstance);
    std::cout << "Defining chargino minus with Mass: " << mass << " and pdgID: " << pdgID << std::endl;
    return s_theInstance;
  } else {
    G4ExceptionDescription description;
    description << "Trying to create a particle with default constructor is not allowed";
    G4Exception("CharginoMinus", "DefaultConstructorCalled", FatalException, description);
    abort();
  }
}
}
