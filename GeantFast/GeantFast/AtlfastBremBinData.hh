#ifndef ATLFAST_BREMBINDATA_H
#define ATLFAST_BREMBINDATA_H
#include <cmath>
#include <vector>

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "G4Track.hh"
#include "AtlfastBinID.hh"
#include "AtlfastParameterResolutions.hh"

//======================================================
//
// BremBinData
//
// Class to hold smearing matrix data and
// calculate bremsstrahlung corrections
// for a given track
//
//======================================================


namespace Atlfast
{

  using std::vector;

  /** @brief Class to hold smearing matrix data.
   *
   * The data is provided through the constructor
   * in the format found in the flat file and the
   * correlation matrix corresponding to the bin
   * can be calculated and returned via public methods.
   */

  class BremBinData
  {
    public:

      /** Constructor from flat file contents */
      BremBinData( BinID&,
                   vector< ParameterResolutions* >,
                   vector< ParameterResolutions* >,
                   vector< ParameterResolutions* >,
                   int randSeed
                 );

      /** Default destructor */
      ~BremBinData();

      /** returns smeared track parameters depending on an
       * internally generated random number and
       * track trajectory supplied as argument
       * getMatrix is not needed in this context
       */
      G4Track* getBremTrack( const G4Track& track ) const;

    private:

      BinID m_id;

      vector< ParameterResolutions* >  m_startPoints;
      vector< ParameterResolutions* >  m_slopes;
      vector< ParameterResolutions* >  m_sigmas;

      CLHEP::HepRandomEngine*  m_randomEngine;
  };

}
#endif
