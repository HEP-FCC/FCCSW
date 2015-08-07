// $Id: WriteHepMCAsciiFile.cpp,v 1.3 2008-07-09 14:39:08 robbep Exp $
// 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/IO_GenEvent.h"
// ============================================================================
// Generators 
// ============================================================================
#include "Generators/Scale.h"
// ============================================================================
/** @class WriteHepMCAsciiFile WriteHepMCAsciiFile.cpp
 *
 *  Simple class to dump generators events in plain 
 *  output file in HepMC Ascii format. It could be used for portable 
 *  cross-transitions of events inbetween different generators 
 *
 *   The algorithm has 3 properties:
 * 
 *    - <c>Input</c>  : The TES location of LHCb::HepMCEvent::Container
 *              ( the default value is <c>LHCb::HepMCEventLocation::Default</c>)
 *    - <c>Ouput</c>  : The name of output HepMC Ascii file. 
 *                     The file is opened in "new/overwrite" mode.
 *                     ( the default value is <c>""</c> (empty string)
 *    - <c>Rescale<c> : Boolean flag to rescal the event from 
 *                      LHCb units to Pythia units.
 *                      The default value is <c>true</c>.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2005-10-05
 */
class WriteHepMCAsciiFile : public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<WriteHepMCAsciiFile> ;
public:
  /// initialize the algorithm
  virtual StatusCode initialize () 
  {
    StatusCode sc = GaudiAlgorithm::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
    /// check the output file 
    if ( m_output.empty() ) 
    { return Error ( "Output file name is not specified!" ) ; }
    // open the file 
    m_file = new HepMC::IO_GenEvent 
      ( m_output.c_str() , std::ios::out | std::ios::trunc ) ;
    //  
    if ( 0 == m_file || m_file->rdstate() == std::ios::failbit ) 
    { return Error ( "Failure to open the file '"+m_output+"'" ) ; }
    //
    //m_file->write_comment( "Written by WriteHepMCAsciiFile/"+name() );
    //
    return StatusCode::SUCCESS ;
  } ;
  /// execute it! 
  virtual StatusCode execute    () ;
  /// finalize the algorithm
  virtual StatusCode finalize   () 
  {
    // delete the stream  (close the file!)
    if ( 0 != m_file ) { delete m_file ; m_file = 0 ; }
    // finalize the base class ;
    return GaudiAlgorithm::finalize() ;
  } ;
protected:
  /** standard constructor
   *  @param name algorithm instance name 
   *  @param pSvc ppinter to Service Locator 
   */
  WriteHepMCAsciiFile
  ( const std::string& name ,
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    // default location of HepMC events in TES 
    , m_input    ( LHCb::HepMCEventLocation::Default ) 
    // no default value!
    , m_output   (   ) 
    // rescale event from LHCb to Pythia units ?
      , m_rescale  ( true )  ///< rescale event from LHCb to Pythia units ?
    // the file itself 
    , m_file     ( 0 )
  {
    declareProperty  ( "Input"   , m_input   ) ;
    declareProperty  ( "Output"  , m_output  ) ;    
    declareProperty  ( "Rescale" , m_rescale ) ;
  } ;
  /// virtual and ptotected destructor
  virtual ~WriteHepMCAsciiFile() 
  { if ( 0 != m_file ) { delete m_file ; m_file = 0  ; } }
private:
  // default constructor is disabled 
  WriteHepMCAsciiFile () ;
  // copy constructor is disabled 
  WriteHepMCAsciiFile ( const WriteHepMCAsciiFile& );
  // assignement operator is disabled 
  WriteHepMCAsciiFile& operator=( const WriteHepMCAsciiFile& );
private:
  // TES location of HepMC-events 
  std::string      m_input  ; ///< TES location of HepMC-events
  // the name of the output file 
  std::string      m_output ; ///< the name of the output file 
  // rescale event from LHCb to Pythia units ?
  bool             m_rescale ; ///< rescale event from LHCb units ?  
  // the output file ;
  HepMC::IO_GenEvent* m_file   ; ///< the output file ;
} ;
// ===========================================================================
/// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( WriteHepMCAsciiFile )
// ===========================================================================
/// Execut the algorithm
StatusCode WriteHepMCAsciiFile::execute    () 
{
  /// get input events 
  LHCb::HepMCEvent::Container* events 
    = get<LHCb::HepMCEvent::Container>( m_input ) ;
  if ( 0 == events ) { return StatusCode::FAILURE ; }
  //
  Assert ( 0 != m_file , "File is invalid!" ) ;
  //
  // loop over events and write them 
  for ( LHCb::HepMCEvent::Container::iterator 
          ievent = events->begin() ; events->end() != ievent  ; ++ievent )
  {
    LHCb::HepMCEvent* event = *ievent ;
    if ( 0 == event ) { continue ; }
    HepMC::GenEvent* evt = event->pGenEvt();
    if ( 0 == evt   ) { continue ; }
    
    // rescale the event if needed 
    if ( m_rescale ) 
    { GeneratorUtils::scale ( evt , 1./Gaudi::Units::GeV , 
                              Gaudi::Units::c_light / Gaudi::Units::mm ) ; }
    
    // write event to ascii file
    m_file->write_event(evt); 	//also writes HeavyIon and PdfInfo!
    
    // rescale back if needed (convert to LHCb units) 
    if ( m_rescale ) 
    { GeneratorUtils::scale ( evt , Gaudi::Units::GeV ,  
                              Gaudi::Units::mm / Gaudi::Units::c_light ) ; }
  }
  //
  return StatusCode::SUCCESS ;
}
// ===========================================================================

// ===========================================================================
// The END 
// ===========================================================================
