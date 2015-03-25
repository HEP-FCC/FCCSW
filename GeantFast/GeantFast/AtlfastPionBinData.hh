#ifndef ATLFAST_PIONBINDATA_H
#define ATLFAST_PIONBINDATA_H
#include <cmath>
#include <vector>
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Random/JamesRandom.h"

#include "G4Track.hh"
#include "AtlfastBinID.hh"
#include "AtlfastIBinData.hh"
#include "AtlfastParameterResolutions.hh"

//======================================================
//
// PionBinData
//
// Class to hold smearing matrix data
// and calculate correlation matrix for a given track
//
//======================================================


namespace Atlfast
{
  /**
   * @brief Class to hold smearing matrix data.
   *
   * The data is provided through the constructor
   * in the format found in the flat file and the
   * correlation matrix corresponding to the bin
   * can be calculated and returned via public methods.
   */
  using std::pair;

  class PionBinData: public IBinData
  {
    public:

      /** Constructor from flat file contents */
      PionBinData( BinID&,
                   vector< ParameterResolutions* >,
                   vector< ParameterResolutions* >,
                   vector< ParameterResolutions* >,
                   vector< ParameterResolutions* >,
                   int randSeed
                  );

      /** returns the correlation matrix depending on an
       * internally generated random number and
       * track trajectory supplied
       * as argument
       */
      CLHEP::HepSymMatrix getMatrix( const G4Track& track ) const;
      /** Default Destructor **/
      virtual ~PionBinData();



    private:

      BinID m_id;

      vector< ParameterResolutions* >  m_cores;
      vector< ParameterResolutions* >  m_tails;
      vector< ParameterResolutions* >  m_fractions;
      vector< ParameterResolutions* >  m_correlations;

      CLHEP::HepRandomEngine*  m_randomEngine;
  };

}
#endif
