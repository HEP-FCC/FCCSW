#ifndef GENERATORS_XMLCOUNTERLOGFILE_H 
#define GENERATORS_XMLCOUNTERLOGFILE_H 1

// Include files
// from STL 
#include <string>
#include <fstream>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Generators/ICounterLogFile.h"            // Interface

/** @class XmlCounterLogFile XmlCounterLogFile.h component/XmlCounterLogFile.h
 *  
 *  Tool to write counters in a xml file
 * 
 *  @author Patrick Robbe
 *  @date   2013-01-15
 */
class XmlCounterLogFile : public GaudiTool, virtual public ICounterLogFile {
public: 
  /// Standard constructor
  XmlCounterLogFile( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~XmlCounterLogFile( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /// Finalize method
  virtual StatusCode finalize( ) ;

  /// Add efficiency number
  virtual void addEfficiency( const std::string & name , const unsigned int after , 
                              const unsigned int before , const double fraction , 
                              const double err_fraction ) ;

  virtual void addCounter( const std::string & name , const unsigned int value ) ; 

  virtual void addFraction( const std::string & name , const unsigned int number , 
                            const double fraction , const double err_fraction ) ;

  virtual void addCrossSection( const std::string & name , 
                                const unsigned int processId , 
                                const unsigned int number , 
                                const double value ) ;

  virtual void addEventType( const unsigned int evType ) ;

  /// Add the Gauss version
  virtual void addGaussVersion( const std::string & version ) ;

  virtual void addMethod( const std::string & type ) ;

  virtual void addGenerator( const std::string & generator ) ;

protected:

private:
  std::string     m_version ;
  std::string     m_fileName ;
  std::ofstream   m_file ;

};
#endif // GENERATORS_XMLCOUNTERLOGFILE_H
