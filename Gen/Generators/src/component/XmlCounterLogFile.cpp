// Include files
// local
#include "XmlCounterLogFile.h"

// from BOOST
#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 


//-----------------------------------------------------------------------------
// Implementation file for class : XmlCounterLogFile
//
// 2013-01-15 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( XmlCounterLogFile )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XmlCounterLogFile::XmlCounterLogFile( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
: GaudiTool ( type, name , parent ) , m_version( "1.1" ) 
{
  declareInterface<ICounterLogFile>(this);
  declareProperty( "FileName" , m_fileName = "GeneratorLog.xml" ) ;
}

//=============================================================================
// Destructor
//=============================================================================
XmlCounterLogFile::~XmlCounterLogFile() {} 

//=============================================================================
// Add efficiency in xml file
//=============================================================================
void XmlCounterLogFile::addEfficiency( const std::string & name , const unsigned int after , 
                                       const unsigned int before , const double fraction , 
                                       const double err_fraction )
{
  m_file << "  <efficiency name = \"" << name << "\">" << std::endl 
         << "    <after> " << after << " </after>" << std::endl
         << "    <before> " << before << " </before>" << std::endl 
         << "    <value> " << format( "%.5g" , fraction ) << " </value>" << std::endl 
         << "    <error> " << format( "%.5g" ,err_fraction ) << " </error>" << std::endl 
         << "  </efficiency>" << std::endl ;
}

//=============================================================================
// Add counter in xml file
//=============================================================================
void XmlCounterLogFile::addCounter( const std::string & name , const unsigned int value ) 
{
  m_file << "  <counter name = \"" << name << "\">" << std::endl
         << "    <value> " << value << " </value>" << std::endl 
         << "  </counter>" << std::endl ;
}

//=============================================================================
// Add fraction in xml file
//=============================================================================
void XmlCounterLogFile::addFraction( const std::string & name , const unsigned int number , 
                                     const double fraction , const double err_fraction ) 
{
  m_file << "  <fraction name = \"" << name << "\">" << std::endl 
         << "    <number> " << number << " </number>" << std::endl
         << "    <value> " << format( "%.5g" , fraction ) << " </value>" << std::endl 
         << "    <error> " << format( "%.5g" , err_fraction ) << " </error>" << std::endl 
         << "  </fraction>" << std::endl ;
}

//=============================================================================
// Add cross section value
//=============================================================================
void XmlCounterLogFile::addCrossSection( const std::string & name , 
                                         const unsigned int processId , 
                                         const unsigned int number , 
                                         const double value ) 
{
  m_file << "  <crosssection id = \"" << processId << "\">" << std::endl 
         << "    <description> \""<< name << "\" </description>" << std::endl
         << "    <generated> " << number << " </generated>" << std::endl 
         << "    <value> " << format( "%.5g" , value ) << " </value>" << std::endl 
         << "  </crosssection>" << std::endl ;
}

//=============================================================================
// Add event type value
//=============================================================================
void XmlCounterLogFile::addEventType( const unsigned int evType )
{
  m_file << "  <eventType>" << evType << "</eventType>" << std::endl ;
}

//=============================================================================
// Add version value
//=============================================================================
void XmlCounterLogFile::addGaussVersion( const std::string & version )
{
  m_file << "  <gaussVersion>" << version << "</gaussVersion>" << std::endl ;
}

//=============================================================================
// Add version value
//=============================================================================
void XmlCounterLogFile::addMethod( const std::string & type )
{
  m_file << "  <method>" << type << "</method>" << std::endl ;
}

//=============================================================================
// Add version value
//=============================================================================
void XmlCounterLogFile::addGenerator( const std::string & generator )
{
  m_file << "  <generator>" << generator << "</generator>" << std::endl ;
}

//=============================================================================
// Initialize function
//=============================================================================
StatusCode XmlCounterLogFile::initialize( ) 
{
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Opens the file
  if ( boost::filesystem::exists( m_fileName ) ) boost::filesystem::remove( m_fileName ) ;
  
  m_file.open( m_fileName.c_str() ) ;
  if (  ! m_file.is_open() ) 
    return Error( "Cannot open xml log file" ) ;
  
  m_file << "<?xml version=\"1.0\"?>" << std::endl 
         << "<generatorCounters>" << std::endl 
         << "  <version>" << m_version << "</version>" << std::endl ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Finalize function
//=============================================================================
StatusCode XmlCounterLogFile::finalize( ) 
{
  m_file << "</generatorCounters>" << std::endl ;
  m_file.close() ;
  return GaudiTool::finalize( ) ;
}
