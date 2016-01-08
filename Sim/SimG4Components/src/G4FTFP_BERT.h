#ifndef SIM_G4FTFP_BERT_H
#define SIM_G4FTFP_BERT_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/IG4PhysicsList.h"

class G4FTFP_BERT: public GaudiTool, virtual public IG4PhysicsList {
public:
  G4FTFP_BERT(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4FTFP_BERT();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual G4VModularPhysicsList* getPhysicsList();

};

#endif /* SIM_G4FTFP_BERT_H */
