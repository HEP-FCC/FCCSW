#include "AtlfastPionBinData.hh"

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


  //-----------------------------------------------
  // PUBLIC - Constructor
  //-----------------------------------------------
  PionBinData::PionBinData( BinID& id,
                            vector< ParameterResolutions* > core,
                            vector< ParameterResolutions* > tails,
                            vector< ParameterResolutions* > fractions,
                            vector< ParameterResolutions* > correlations,
                            int randSeed
			  ) :
               m_id(id),
               m_cores(core),
               m_tails(tails),
               m_fractions(fractions),
               m_correlations(correlations)
{
    m_randomEngine = new CLHEP::HepJamesRandom(randSeed);
}


  PionBinData::~PionBinData()
  {
    delete m_randomEngine;
    for (size_t i=0; i < m_fractions.size(); i++)
      delete m_fractions[i];
    for (size_t i=0; i < m_correlations.size(); i++)
      delete m_correlations[i];
    for (size_t i=0; i < m_cores.size(); i++)
      delete m_cores[i];
    for (size_t i=0; i < m_tails.size(); i++)
      delete m_tails[i];
  }

  //--------------------------------------------------------------------
  // PUBLIC - CLHEP::HepSymMatrix getMatrix(random)
  // returns appropriate Sigma (=covariance) matrix
  //
  // NOTE: the representation of Sigma is determined by the track
  //       representation implicitly given in the parameter files,
  //       i.e., (d0, z0, phi0, cot(theta0), q/pT), which is for
  //       internal use only.
  //
  //       The main advantage of this representation is that in
  //       the context of the ID, for a solenoidal field, the three
  //       transverse track parameters (d0, phi0, q/pT) are to a
  //       good approximation uncorrelated with the two longitudinal
  //       ones (z0, cot(theta0)).
  //
  //       Sigma has to be converted into the representation used
  //       by Common Tracking, i.e. (d0, z0, phi0, theta0, q/p),
  //       when it is written to CBNTs or AODs.
  //--------------------------------------------------------------------
  CLHEP::HepSymMatrix PionBinData::getMatrix( const G4Track& track ) const
  {
    CLHEP::HepSymMatrix Sigma(5,0);

    double fraction, random[5];

    // order of parameters: d0, z0, phi0, cot(theta0), q/pT
    // introduce "correlation" between core <--> tails
    random[0] = m_randomEngine->flat();
    random[1] = m_randomEngine->flat();
    random[2] = random[0];
    random[3] = random[1];
    random[4] = random[0];

    // diagonals
    for ( int param = 0; param < 5; param++ )
    {
      fraction = m_fractions[param]->resolution(track);
      if ( fraction > 1.0 )  fraction = 1.0;
      Sigma[param][param] = ( random[param] < fraction )  ?
                            std::pow( m_cores[param]->resolution(track), 2 )  :
                            std::pow( m_tails[param]->resolution(track), 2 );
    }

    // off-diagonals
    // NOTE: m_correlations[] holds correlation coefficients, need covariances

    // (1,3) ... cov(d0,phi0)
    // (1,5) ... cov(d0,q/pT)
    // (3,5) ... cov(phi0,q/pT)
    double rho13 = m_correlations[0]->resolution(track);
    double rho15 = m_correlations[1]->resolution(track);
    double rho35 = m_correlations[2]->resolution(track);

    // covariance sub-matrix of transverse parameters needs to be positive definite
    // in order that its square root (cf. PionMatrixManager) exists
    double det3 = 1 - rho13 * rho13 - rho15 * rho15 - rho35 * rho35 + 2 * rho13 * rho15 * rho35;
    if ( det3 < 0 )
       rho13 = rho15 = rho35 = 0;

    // make sure that correlation coefficients stay within [-1,+1]
    if ( std::abs(rho13) > 1 )  rho13 *= 0.99 / std::abs(rho13);
    if ( std::abs(rho15) > 1 )  rho15 *= 0.99 / std::abs(rho15);
    if ( std::abs(rho35) > 1 )  rho35 *= 0.99 / std::abs(rho35);

    Sigma(1,3) = Sigma(3,1) = rho13 * std::sqrt( Sigma(1,1) * Sigma(3,3) );
    Sigma(1,5) = Sigma(5,1) = rho15 * std::sqrt( Sigma(1,1) * Sigma(5,5) );
    Sigma(3,5) = Sigma(5,3) = rho35 * std::sqrt( Sigma(3,3) * Sigma(5,5) );


    // (2,4) ... cov(z0,cot(theta0))
    double rho24 = m_correlations[3]->resolution(track);
    // make sure that correlation coefficient stays within [-1,+1]
    if ( std::abs(rho24) > 1 )  rho24 *= 0.99 / std::abs(rho24);
    Sigma(2,4) = Sigma(4,2) = rho24 * std::sqrt( Sigma(2,2) * Sigma(4,4) );



    // DONE!
    return Sigma;

  }

} //namespace bracket
