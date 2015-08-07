// Include files

// GaudiKernel
// =============================================================================
#include "GaudiKernel/ToolFactory.h"
// =============================================================================

// =============================================================================
// Generators 
// =============================================================================
#include "MCInterfaces/IFullGenEventCutTool.h"

// from FastJet
#include "fastjet/ClusterSequence.hh"
#include "fastjet/config.h"

// from LoKi
#include "LoKi/GenTypes.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/IGenHybridFactory.h"
#include "LoKi/IGenDecay.h"
#include "LoKi/IGenHybridFactory.h"
#include "LoKi/FilterTool.h"

/** @class InvariantMassQQMCJets InvariantMassQQMCJets.h
 *  
 *  Tool to force pairs of qqbar above certain mass
 *  Concrete implementation of IFullGenEventCutTool.
 * 
 *
 *  @author Xabier Cid Vidal
 *  @date   2013-10-29
 */


namespace 
{
  // ===========================================================================
  typedef  LoKi::BasicFunctors<LoKi::GenTypes::GenContainer>::BooleanConstant _BOOLC ;
  typedef std::pair<fastjet::PseudoJet, std::vector<HepMC::GenParticle*> > JetParticlesPair;
  // ===========================================================================
}


// same structure as FullGenEventCut
namespace LoKi 
{

  class InvariantMassQQMCJets
  // : public extends1<LoKi::FilterTool,IFullGenEventCutTool>
    : public         LoKi::FilterTool
    , public virtual IFullGenEventCutTool
  {
    // =========================================================================
    // friend factor for instantiation 
    friend class ToolFactory<LoKi::InvariantMassQQMCJets> ;
    // =========================================================================
  public:
    // =========================================================================
    /// Initialization
    virtual StatusCode initialize () { return LoKi::FilterTool::initialize () ; }  
    /// Finalization 
    virtual StatusCode finalize   () ;
    
    // =========================================================================
    /** Apply the cut on a event.
     *  @param theEvents      Container of all interactions in the event.
     *  @param theCollisions  Container of hard process informations of each 
     *                            pile-up interactions of the event.
     *  @return    true  if the full event passes the cut.
     *  @see IFullGEnEventrCutTool
     *  Accept events with qqbar MCjets above certain mass
     */
    virtual bool studyFullEvent
    ( LHCb::HepMCEvents*       theEvents        ,
      LHCb::GenCollisions*  /* theCollisions */ ) const ;
    // =========================================================================
  public:
    // =========================================================================
    /// perform the actual decoding of the functor 
    virtual StatusCode decode () ; // perform the actual decoding of the functor 
    // =========================================================================
  protected:
    // =========================================================================
    /** standard constructor 
     *  @param type the tool type
     *  @param name the actual instance name 
     *  @param parent the tool parent 
     */
    InvariantMassQQMCJets 
    ( const std::string&  type   ,                     //    the tool type (???)
      const std::string&  name   ,                     // the tool isntance name 
      const IInterface*   parent ) ;                   //        the tool parent 
    /// virtual & protected desctructor 
    virtual ~InvariantMassQQMCJets () ;             // virtual & protected desctructor 
    // =========================================================================
  private:
    // =========================================================================
    /// the default constructor is disabled 
    InvariantMassQQMCJets () ;                  // the default constructor is disabled 
    /// the copy constructor is disabled 
    InvariantMassQQMCJets ( const InvariantMassQQMCJets& ) ;          // no copy constructor 
    /// assignement operator is disabled 
    InvariantMassQQMCJets& operator= ( const InvariantMassQQMCJets& );     // no assignement 
    // =========================================================================
  private:
    // =========================================================================
    
    // sign function
    int sign(const int n) const;
    
    
    //associate each parton to the jet
    std::vector<JetParticlesPair> addParticlesInfo(const HepMC::GenEvent *  theEvent,
                                                   const  std::vector<fastjet::PseudoJet> listOfMCJets) const;
    
    
    /// build a list of MC jets from the particles that come from strings
    std::vector<fastjet::PseudoJet> buildMCJets(const HepMC::GenEvent *  theEvent , 
                                                fastjet::ClusterSequence* clusters) const;
    
    /// check if the combination of two MCjets is allowed (have b and antib hadron associated and are not the same)  
    bool isCombination( const JetParticlesPair MCJetP1,
                        const JetParticlesPair MCJetP2) const;
    
    /// check if quark is in particle
    bool hasParticleQuark( const HepMC::GenParticle* particle, const int quark_id) const;
    
    
    /// minimum pt of MCJets
    double m_jetptmin;
    
    /// maximum mass of qqbar pair
    double m_maxmassqq;
    
    /// minimum cut of qqbar pair
    double m_minmassqq;
    
    /// size of the MCJets
    double m_MCJetSize;
    
    /// deltaR between the hadron and the mcjet
    double m_DeltaRHadMCJet;
    
    /// PDGID of quark1 
    int m_quarkOnePDG;
    
    /// PDGID of quark2 
    int m_quarkTwoPDG;
    
    // Constraint input particles to those stables at LHCb?
    // Should be true for a FullEventCut
    bool m_particleTypes;
    
    /// the actual LoKi code 
    LoKi::Types::GCutVal   m_cutval ;  // the actual code 
    // =========================================================================
  } ;
  // ===========================================================================
} //                                                       end of namespace LoKi
// =============================================================================



