#ifndef _DELPHESSIMULATION_H_
#define _DELPHESSIMULATION_H_

#include "TObjArray.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataObjectHandle.h"
#include "DataObjects/ParticleCollection.h"
#include "DataObjects/GenJetCollection.h"
#include "DataObjects/MissingEnergyCollection.h"
#include "DataObjects/MCParticleCollection.h"


class DelphesSimulation: public GaudiAlgorithm {
  friend class AlgFactory<DelphesSimulation> ;

public:
  /// Constructor.
  DelphesSimulation(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute. This function actually does no simulation,
  /// and simply converts the stable MCParticles in the input collection
  /// into Particles that are written to the output collection. 
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

 void  ConvertParticle(TObjArray *  Input, ParticleCollection * coll ); 
 void  ConvertJet(TObjArray *  Input, GenJetCollection * coll ); 
 void  ConvertMET(TObjArray *  Input, ParticleCollection * coll ); 
 void  ConvertHT(TObjArray *  Input, ParticleCollection * coll ); 



private:



  // detector card to be read
  std::string m_filename;
  std::string m_detectorcard;
  std::vector<std::string> m_outputcollections;

  bool  m_debug_delphes; // if true one run the standalone delphes as well 

  /// Handle for the MCParticleCollection to be read
  //DataObjectHandle<MCParticleCollection> m_genphandle;
  /// Handle for the "reconstructed" to be written
  DataObjectHandle<ParticleCollection> m_recgphandle; // genparticles particles
  DataObjectHandle<ParticleCollection> m_recparthandle; // partons
  DataObjectHandle<ParticleCollection> m_recphandle; // stable particles
  DataObjectHandle<ParticleCollection> m_recmhandle; // muons
  DataObjectHandle<ParticleCollection> m_recehandle; // electrons
  DataObjectHandle<ParticleCollection> m_recphhandle; // photons
  DataObjectHandle<GenJetCollection> m_recjhandle; // jets
  DataObjectHandle<ParticleCollection> m_recmethandle; // met
  DataObjectHandle<ParticleCollection> m_rechthandle; // ht
};

#endif
