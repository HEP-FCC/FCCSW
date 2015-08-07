#ifndef COMPONENT_SAVESIGNALBINFORMATION_H 
#define COMPONENT_SAVESIGNALBINFORMATION_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations
namespace HepMC {
  class GenVertex ;
  class GenParticle ;
}

/** @class SaveSignalBInformation SaveSignalBInformation.h
 *  
 *  Algorithm that takes the b string information from HepMC 
 *  and stores it at a specific location
 *
 *  @author Patrick Robbe
 *  @date   2013-06-26
 */
class SaveSignalBInformation : public GaudiAlgorithm {
public: 
  /// Standard constructor
  SaveSignalBInformation( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SaveSignalBInformation( ) { } ; ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  /// Extract B string from signal
  StatusCode extractSignal( const HepMC::GenVertex * theVertex ) const ;
  
  /// make a new HepMC event
  StatusCode fillHepMCEvent( HepMC::GenParticle * theNewParticle, 
                             const HepMC::GenParticle * theOldParticle ) 
    const ;

  std::string m_inputHepMCEvent  ;
  std::string m_outputHepMCEvent ;
};
#endif // COMPONENT_SAVESIGNALBINFORMATION_H
