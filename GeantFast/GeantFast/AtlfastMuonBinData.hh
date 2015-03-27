#ifndef ATLFAST_MUONBINDATA_H
#define ATLFAST_MUONBINDATA_H
#include <cmath>
#include <vector>
#include "CLHEP/Matrix/SymMatrix.h"
#include "G4Track.hh"
#include "AtlfastBinID.hh"
#include "AtlfastIBinData.hh"
#include "AtlfastParameterResolutions.hh"

//=================================================
//
// Muon Bin Data
//
// Class to hold smearing matrix data
// and calculate correlation matrix for a given track
//==================================================


namespace Atlfast {

  /** @brief Class to hold smearing matrix data
   *
   * The data is provided through the constructor
   * in the format found in the flat file and the
   * correlation matrix corresponding to the bin
   * can be calculated and returned via public methods.
   */
  using std::pair;


  class MuonBinData: public IBinData {

  public:
    /** Constructor from flat file contents */
    MuonBinData( BinID&,
		 vector< ParameterResolutions* >,
		 vector< ParameterResolutions* >
               );

   /** Default destructor */
    virtual ~MuonBinData();

    /** returns the correlation matrix;
     * track trajectory supplied
     * as argument
     */
    CLHEP::HepSymMatrix getMatrix( const G4Track& track ) const;

     private:
    BinID m_id;

    vector< ParameterResolutions* >  m_sigmas;
    vector< ParameterResolutions* >  m_correlations;
  };

}

#endif
