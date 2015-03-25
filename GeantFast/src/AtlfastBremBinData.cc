#include "AtlfastBremBinData.hh"

namespace Atlfast
{
  /** @brief Class to hold smearing matrix data.
   *
   * The data is provided through the constructor
   * in the format found in the flat file and the
   * correlation matrix corresponding to the bin
   * can be calculated and returned via public methods.
   */

  //-----------------------------------------------
  // PUBLIC - Constructor
  //-----------------------------------------------
  BremBinData::BremBinData( BinID& id,
                            vector< ParameterResolutions* > startPoints,
                            vector< ParameterResolutions* > slopes,
                            vector< ParameterResolutions* > sigmas,
                            int randSeed
			  ) :
                m_id(id),
                m_startPoints(startPoints),
                m_slopes(slopes),
                m_sigmas(sigmas)
  {
    m_randomEngine = new CLHEP::HepJamesRandom(randSeed);
  }


  //-----------------------------------------------
  // PUBLIC - Destructor
  //-----------------------------------------------
  BremBinData::~BremBinData()
  {
    for (size_t i = 0; i < m_startPoints.size(); i++)
      delete m_startPoints[i];
    for (size_t i = 0; i < m_slopes.size(); i++)
      delete m_slopes[i];
    for (size_t i = 0; i < m_sigmas.size(); i++)
      delete m_sigmas[i];
    delete m_randomEngine;
  }


  //--------------------------------------------------------------------
  // PUBLIC - TrackTrajectory getBremTrack
  // returns smeared track parameters
  //--------------------------------------------------------------------
  G4Track* BremBinData::getBremTrack( const G4Track& track ) const
  {

//TO BE FIXED: THE BREMSSTRAHLUNG


    // data necessary to generate bremsstrahlung corrections
    double x0[3], lambda[3], sigma[3];
    for ( int i = 0; i < 3; i++ )
    {
      x0[i]     = std::abs( m_startPoints[i]->resolution(track) );
      lambda[i] = std::abs( m_slopes[i]->resolution(track) );
      sigma[i]  = m_sigmas[i]->resolution(track);
    }
    // range of power-function distribution [x0, xMax]
    // for q/pT, 500MeV is the offline reconstruction cut
    double xMax[3] = { 90 * sigma[0], 90 * sigma[1], 1.0 / (0.5*CLHEP::GeV) };

    double charge = double( abs(track.GetDynamicParticle()->GetCharge()) / track.GetDynamicParticle()->GetCharge() );
    // bremsstrahlung results in both pos and neg tails, depending on charge and parameter
    double sign[3] = { -charge, charge, charge };

    double random[3];
    random[0] = m_randomEngine->flat();
    random[1] = random[0];  // introduce correlation (d0,phi0)
    random[2] = random[0];  // introduce correlation (d0,q/pT), (phi0,q/pT)

    double var[3];
    for ( int i = 0; i < 3; i++ )
    {
      double denom = 1 - random[i] * ( 1 - std::pow( x0[i] / xMax[i], lambda[i] - 1 ) );
      denom = std::pow( denom, 1.0 / ( lambda[i] - 1 ) );
      var[i] = sign[i] * x0[i] / denom;   // normalised power-series distribution
    }

    // create output G4Track object: longitudinal parameters remain unchanged
    // (small effects taken into account in double-Gaussian smearing in ElectronMatrixManager)

    double impactParameter = var[0];
    double zPerigee        = 0; // COMPUTE  !!!!!!!!!!!!!!!!!!!!!!!!!
    double phi             = var[1];
    double cotTheta        = 1/tan( track.GetMomentum().theta() );
    double invPtCharge     = var[2];

    //double curvature =  track.curvature();
    double pT = std::abs( 1. / invPtCharge );
    G4ThreeVector vec;
    vec.setX( pT * std::cos(phi) );
    vec.setY( pT * std::sin(phi) );
    vec.setZ( pT * cotTheta );

    G4ThreeVector pos;
    vec.setX( impactParameter * std::cos(phi) );
    vec.setY( impactParameter * std::sin(phi) );
    vec.setZ( zPerigee );

   G4DynamicParticle* part = new G4DynamicParticle(track.GetDynamicParticle()->GetDefinition(), vec);
   G4Track* newTrack = new G4Track(part, track.GetGlobalTime(), pos );
    return newTrack;
  }

} //namespace bracket
