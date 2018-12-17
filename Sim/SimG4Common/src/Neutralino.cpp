// local
#include "SimG4Common/Neutralino.h"

namespace sim {
Neutralino* Neutralino::s_theInstance = NULL;
int Neutralino::pdgID = 1000022;
std::string Neutralino::name = "s_chi_0_1";

Neutralino* Neutralino::Definition(G4double mass, G4double width, G4bool stable, G4double lifetime) {
  if (s_theInstance != 0 && (mass >= 0. || width >= 0. || lifetime >= 0.)) {
    G4ExceptionDescription description;
    description << "Trying to redefine theNeutralino properties after it has been constructed is not allowed";
    G4Exception("Neutralino", "FailedRedefinition", FatalException, description);
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
        new G4ParticleDefinition(name, mass, width, 0. * CLHEP::eplus, 1, 0, 0, 0, 0, 0, "supersymmetric", 0, 0, pdgID,
                                 stable, lifetime, NULL, false, "Neutralino");

    s_theInstance = reinterpret_cast<Neutralino*>(anInstance);
    std::cout << "Defining neutralino plus with Mass: " << mass << " and pdgID: " << pdgID << std::endl;
    return s_theInstance;
  } else {
    G4ExceptionDescription description;
    description << "Trying to create a particle with default constructor is not allowed";
    G4Exception("Neutralino", "DefaultConstructorCalled", FatalException, description);
    abort();
  }
}
}
