#ifndef SIMG4COMPONENTS_G4FTFPBERT_H
#define SIMG4COMPONENTS_G4FTFPBERT_H
// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Interface/IG4PhysicsList.h"

/** @class G4FtfpBert SimG4Components/src/G4FtfpBert.h G4FtfpBert.h
 *
 *  FTFP_BERT physics list tool.
 *
 *  @author Anna Zaborowska
 */

class G4FtfpBert: public AlgTool, virtual public IG4PhysicsList {
public:
  explicit G4FtfpBert(const std::string& aType , const std::string& aName,
                  const IInterface* aParent);
  virtual ~G4FtfpBert();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize();
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize();
  /** Get the physics list.
   *  @return pointer to G4VModularPhysicsList
   */
  virtual G4VModularPhysicsList* getPhysicsList();

};

#endif /* SIMG4COMPONENTS_G4FTFPBERT_H */
