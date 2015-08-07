// local
#include "HistoSmearVertex.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector4DTypes.h"

// from Event
#include "Event/HepMCEvent.h"
#include "TFile.h"
#include "TH3.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HistoSmearVertex
//
// 2014-08-02 : Dominik Mitzel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HistoSmearVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HistoSmearVertex::HistoSmearVertex( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty("InputFileName", m_inputFileName = "");
    declareProperty("HistogramPath", m_histoPath = "");
    declareProperty("BeamDirection", m_zDir = 0 );
}

//=============================================================================
// Destructor
//=============================================================================
HistoSmearVertex::~HistoSmearVertex( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode HistoSmearVertex::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // -- Open the file containing the VX distribution
  TFile *file = TFile::Open( m_inputFileName.c_str() );
  if( !file ){
    error() << "Could not find vertex distribution template file!" << endmsg;
    return StatusCode::FAILURE;
  }

  // -- Get the histogram template file for the particle momentum spectrum
  m_hist = (TH3D*) file->Get( m_histoPath.c_str() );
  if ( !m_hist ) {
    error() << "Could not find vertex distribution template histogram!" << endmsg;
    return StatusCode::FAILURE;
  }

  std::string infoMsg = " applying TOF of interaction with ";
  if ( m_zDir == -1 ) {
    infoMsg = infoMsg + "negative beam direction";
  } else if ( m_zDir == 1 ) {
    infoMsg = infoMsg + "positive beam direction";
  } else if ( m_zDir == 0 ) {
    infoMsg = " with TOF of interaction equal to zero ";
  } else {
    return Error("BeamDirection can only be set to -1 or 1, or 0 to switch off TOF");
  }

  info() << "Smearing of interaction point with external  distribution "
         << " in x, y and z " << endmsg;
  info() << infoMsg << endmsg;

  return sc ;
}

//=============================================================================
// Smearing function
//=============================================================================
StatusCode HistoSmearVertex::smearVertex( LHCb::HepMCEvent * theEvent ) {
  double dx , dy , dz , dt ;
  m_hist->GetRandom3(dx,dy,dz);

  dt = m_zDir * dz/Gaudi::Units::c_light ;

  Gaudi::LorentzVector dpos( dx , dy , dz , dt ) ;

  HepMC::GenEvent::vertex_iterator vit ;
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  for ( vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ;
        ++vit ) {
    Gaudi::LorentzVector pos ( (*vit) -> position() ) ;
    pos += dpos ;
    (*vit) -> set_position( HepMC::FourVector( pos.x() , pos.y() , pos.z() ,
                                               pos.t() ) ) ;
  }

  return StatusCode::SUCCESS;
}

