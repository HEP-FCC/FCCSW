
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef GenEvent_HepMCEvent_H
#define GenEvent_HepMCEvent_H 1

// Include files
#ifdef _WIN32
// Disable warning C4800 forcing value to bool true or false (performance warning), caused by HepMC/Flow.h
  #pragma warning ( disable : 4800 )
#endif
#ifdef __INTEL_COMPILER
  #pragma warning (disable:1572) // floating-point equality and inequality comparisons are unreliable
  #pragma warning(push)
#endif
#include "HepMC/GenEvent.h"
#ifdef __INTEL_COMPILER
  #pragma warning(pop)
#endif
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SerializeSTL.h"
#include <string>
#include <ostream>

// Forward declarations

namespace LHCb
{

  // Forward declarations
  using GaudiUtils::operator<<;
  
  
  // Class ID definition
  static const CLID CLID_HepMCEvent = 202;
  
  // Namespace for locations in TDS
  namespace HepMCEventLocation {
    static const std::string Default = "Gen/HepMCEvents";
    static const std::string Signal = "Gen/SignalDecayTree";
    static const std::string BInfo = "Gen/BInfo";
  }
  

  /** @class HepMCEvent HepMCEvent.h
   *
   * Gaudi wrapper for HepMC events
   *
   * @author W. Pokorski, modified by G.Corti to adapt to new event model
   * created Wed May 27 10:37:12 2015
   *
   */

  class HepMCEvent: public KeyedObject<int>
  {
  public:

    /// typedef for KeyedContainer of HepMCEvent
    typedef KeyedContainer<HepMCEvent, Containers::HashMap> Container;
  
    /// Status code in HepMC::GenParticle
    enum statusType{ Unknown = 0,
                     StableInProdGen = 1,
                     DecayedByProdGen = 2,
                     DocumentationParticle = 3,
                     DecayedByDecayGen = 777,
                     DecayedByDecayGenAndProducedByProdGen = 888,
                     SignalInLabFrame = 889,
                     SignalAtRest = 998,
                     StableInDecayGen = 999
      };
  
    /// Constructor
    HepMCEvent();
  
    /// Copy constructor
    HepMCEvent(const HepMCEvent& evt);
  
    /// Destructor
    ~HepMCEvent();
  
    // Retrieve pointer to class definition structure
    virtual const CLID& clID() const;
    static const CLID& classID();
  
    /// Fill the ASCII output stream
   virtual std::ostream& fillStream(std::ostream& s) const;
  
    /// Assignment operator
    HepMCEvent& operator=(const HepMCEvent& evt);
  
    /// Retrieve const  Generator Name
    const std::string& generatorName() const;
  
    /// Update  Generator Name
    void setGeneratorName(const std::string& value);
  
    /// Retrieve const  HepMC generator event
    const HepMC::GenEvent* pGenEvt() const;
  
    /// Retrieve  HepMC generator event
    HepMC::GenEvent* pGenEvt();
  
    /// Update  HepMC generator event
    void setPGenEvt(HepMC::GenEvent* value);
  
  
  #ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      using pool = boost::singleton_pool< HepMCEvent, sizeof(HepMCEvent),
                                          boost::default_user_allocator_new_delete,
                                          boost::details::pool::null_mutex, 128 >;
      return ( sizeof(HepMCEvent) == size ? pool::malloc() : ::operator new(size) );
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      using pool = boost::singleton_pool< HepMCEvent, sizeof(HepMCEvent),
                                          boost::default_user_allocator_new_delete,
                                          boost::details::pool::null_mutex, 128 >;
      pool::is_from(p) ? pool::free(p) : ::operator delete(p);
    }
  
    /// placement operator new
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new ( size, pObj );
    }
  
    /// placement operator delete
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete ( p, pObj );
    }
  #endif
  protected:

  private:

    std::string      m_generatorName; ///< Generator Name
    HepMC::GenEvent* m_pGenEvt;       ///< HepMC generator event
  
  }; // class HepMCEvent

  /// Definition of Keyed Container for HepMCEvent
  typedef KeyedContainer<HepMCEvent, Containers::HashMap> HepMCEvents;
  
  inline std::ostream& operator<< (std::ostream& str, const HepMCEvent& obj)
  {
    return obj.fillStream(str);
  }
  
  inline std::ostream & operator << (std::ostream & s, LHCb::HepMCEvent::statusType e) {
    switch (e) {
      case LHCb::HepMCEvent::Unknown                               : return s << "Unknown";
      case LHCb::HepMCEvent::StableInProdGen                       : return s << "StableInProdGen";
      case LHCb::HepMCEvent::DecayedByProdGen                      : return s << "DecayedByProdGen";
      case LHCb::HepMCEvent::DocumentationParticle                 : return s << "DocumentationParticle";
      case LHCb::HepMCEvent::DecayedByDecayGen                     : return s << "DecayedByDecayGen";
      case LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen : return s << "DecayedByDecayGenAndProducedByProdGen";
      case LHCb::HepMCEvent::SignalInLabFrame                      : return s << "SignalInLabFrame";
      case LHCb::HepMCEvent::SignalAtRest                          : return s << "SignalAtRest";
      case LHCb::HepMCEvent::StableInDecayGen                      : return s << "StableInDecayGen";
      default : return s << "ERROR wrong value " << int(e) << " for enum LHCb::HepMCEvent::statusType";
    }
  }
  
  
} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline LHCb::HepMCEvent::HepMCEvent() 
{

          m_pGenEvt = new HepMC::GenEvent();
        
}

inline LHCb::HepMCEvent::~HepMCEvent() 
{

          if(m_pGenEvt !=0) 
          {
          delete m_pGenEvt;
          }
        
}

inline const CLID& LHCb::HepMCEvent::clID() const
{
  return LHCb::HepMCEvent::classID();
}

inline const CLID& LHCb::HepMCEvent::classID()
{
  return CLID_HepMCEvent;
}

inline std::ostream& LHCb::HepMCEvent::fillStream(std::ostream& s) const
{
  s << "{ " << "generatorName :	" << m_generatorName << std::endl
            << "pGenEvt :	" << m_pGenEvt << std::endl << " }";
  return s;
}


inline const std::string& LHCb::HepMCEvent::generatorName() const 
{
  return m_generatorName;
}

inline void LHCb::HepMCEvent::setGeneratorName(const std::string& value) 
{
  m_generatorName = value;
}

inline const HepMC::GenEvent* LHCb::HepMCEvent::pGenEvt() const 
{
  return m_pGenEvt;
}

inline HepMC::GenEvent* LHCb::HepMCEvent::pGenEvt() 
{
  return m_pGenEvt;
}

inline void LHCb::HepMCEvent::setPGenEvt(HepMC::GenEvent* value) 
{
  m_pGenEvt = value;
}



#endif ///GenEvent_HepMCEvent_H
