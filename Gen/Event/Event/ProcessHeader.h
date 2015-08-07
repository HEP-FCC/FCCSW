
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

#ifndef EventBase_ProcessHeader_H
#define EventBase_ProcessHeader_H 1

// Include files
#include "Event/CondDBNameTagPair.h"
#include "GaudiKernel/DataObject.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SerializeSTL.h"
#include <vector>
#include <string>
#include <ostream>

// Forward declarations

namespace LHCb
{

  // Forward declarations
  using GaudiUtils::operator<<;
  
  
  // Class ID definition
  static const CLID CLID_ProcessHeader = 103;
  
  // Namespace for locations in TDS
  namespace ProcessHeaderLocation {
    static const std::string Digi = "MC/DigiHeader";
    static const std::string MC = "MC/Header";
    static const std::string Rec = "Rec/Header";
  }
  

  /** @class ProcessHeader ProcessHeader.h
   *
   * Common base class for all process headers: GenHeader, MCHeader...
   *
   * @author P. Koppenburg
   * created Wed Jul 22 09:45:24 2015
   *
   */

  class ProcessHeader: public DataObject
  {
  public:

    /// Copy constructor. Creates a new ProcessHeader with the same information.
    ProcessHeader(const LHCb::ProcessHeader& head) : DataObject(),
                                                     m_applicationName(head.applicationName()),
                                                     m_applicationVersion(head.applicationVersion()),
                                                     m_runNumber(head.runNumber()),
                                                     m_condDBTags(head.condDBTags()) {}
  
    /// Default Constructor
    ProcessHeader() : m_applicationName("Unknown"),
                      m_applicationVersion("Unknown"),
                      m_runNumber(0),
                      m_condDBTags() {}
  
    /// Default Destructor
    virtual ~ProcessHeader() {}
  
    // Retrieve pointer to class definition structure
    virtual const CLID& clID() const;
    static const CLID& classID();
  
    /// Fill the ASCII output stream
   virtual std::ostream& fillStream(std::ostream& s) const;
  
    /// Retrieve const  Application or Program Name (e.g. Gauss, Boole or Pythia)
    const std::string& applicationName() const;
  
    /// Update  Application or Program Name (e.g. Gauss, Boole or Pythia)
    void setApplicationName(const std::string& value);
  
    /// Retrieve const  Application or Program version
    const std::string& applicationVersion() const;
  
    /// Update  Application or Program version
    void setApplicationVersion(const std::string& value);
  
    /// Retrieve const  Run number
    unsigned int runNumber() const;
  
    /// Update  Run number
    void setRunNumber(unsigned int value);
  
    /// Retrieve const  Conditions database tags
    const std::vector<LHCb::CondDBNameTagPair>& condDBTags() const;
  
    /// Update  Conditions database tags
    void setCondDBTags(const std::vector<LHCb::CondDBNameTagPair>& value);
  
  
  #ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      using pool = boost::singleton_pool< ProcessHeader, sizeof(ProcessHeader),
                                          boost::default_user_allocator_new_delete,
                                          boost::details::pool::null_mutex, 128 >;
      return ( sizeof(ProcessHeader) == size ? pool::malloc() : ::operator new(size) );
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      using pool = boost::singleton_pool< ProcessHeader, sizeof(ProcessHeader),
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

    std::string                          m_applicationName;    ///< Application or Program Name (e.g. Gauss, Boole or Pythia)
    std::string                          m_applicationVersion; ///< Application or Program version
    unsigned int                         m_runNumber;          ///< Run number
    std::vector<LHCb::CondDBNameTagPair> m_condDBTags;         ///< Conditions database tags
  
  }; // class ProcessHeader

  inline std::ostream& operator<< (std::ostream& str, const ProcessHeader& obj)
  {
    return obj.fillStream(str);
  }
  
} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& LHCb::ProcessHeader::clID() const
{
  return LHCb::ProcessHeader::classID();
}

inline const CLID& LHCb::ProcessHeader::classID()
{
  return CLID_ProcessHeader;
}

inline std::ostream& LHCb::ProcessHeader::fillStream(std::ostream& s) const
{
  s << "{ " << "applicationName :	" << m_applicationName << std::endl
            << "applicationVersion :	" << m_applicationVersion << std::endl
            << "runNumber :	" << m_runNumber << std::endl
            << "condDBTags :	" << m_condDBTags << std::endl << " }";
  return s;
}


inline const std::string& LHCb::ProcessHeader::applicationName() const 
{
  return m_applicationName;
}

inline void LHCb::ProcessHeader::setApplicationName(const std::string& value) 
{
  m_applicationName = value;
}

inline const std::string& LHCb::ProcessHeader::applicationVersion() const 
{
  return m_applicationVersion;
}

inline void LHCb::ProcessHeader::setApplicationVersion(const std::string& value) 
{
  m_applicationVersion = value;
}

inline unsigned int LHCb::ProcessHeader::runNumber() const 
{
  return m_runNumber;
}

inline void LHCb::ProcessHeader::setRunNumber(unsigned int value) 
{
  m_runNumber = value;
}

inline const std::vector<LHCb::CondDBNameTagPair>& LHCb::ProcessHeader::condDBTags() const 
{
  return m_condDBTags;
}

inline void LHCb::ProcessHeader::setCondDBTags(const std::vector<LHCb::CondDBNameTagPair>& value) 
{
  m_condDBTags = value;
}



#endif ///EventBase_ProcessHeader_H
