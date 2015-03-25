#ifndef FCC_SMEARER_H
#define FCC_SMEARER_H

#include "FCCOutput.hh"
#include "globals.hh"
#include "G4Track.hh"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandGauss.h"
#include "AtlfastMuonMatrixManager.hh"
#include "AtlfastPionMatrixManager.hh"
#include "AtlfastElectronMatrixManager.hh"

/**
	@brief     Smearing of the particle momentum or energy.
   @details   A singleton class used to smear (alter) the particle momentum (for tracking detectors) and energy (for calorimeters). In case the resolution is given, the momentum (energy) is smeared with Gaussian distribution. In case of AtlFast-like smearing, the perigee parameters are smeared separately.
   @author    Anna Zaborowska
*/

class FCCSmearer
{
public:

/**
   Allows the access to the FCCSmearer class member without creating the class object.
	@return A pointer to the FCCSmearer class.
 */
   static FCCSmearer* Instance();
/**
   Creates the managers fElectronManager, fPionManager and fMuonManager that read the parametrisation from the data files.
*/
   void MakeManagers();
/**
   Smears the momentum with a given resolution. Perigee representation can be saved.
   @param aTrack A track to smear.
   @param aResolution A resolution. If equal to -1, AtlFast smearing is done. Otherwise, Gaussian smearing is done with a given resolution as a standard deviation.
   @param aSavePerigee If not equal to FCCOutput::eNoSave, a perigee is *not* saved
*/
   G4ThreeVector SmearMomentum(const G4Track* aTrack, G4double aResolution = -1, FCCOutput::SaveType aSavePerigee  = FCCOutput::eNoSave);
/**
   Smears the energy deposit with a given resolution. Perigee representation can be saved.
   @param aTrack A track to smear.
   @param aResolution A resolution. If equal to -1, AtlFast smearing is done. Otherwise, Gaussian smearing is done with a given resolution as a standard deviation.
   @param aSavePerigee If not equal to FCCOutput::eNoSave, a perigee is *not* saved
*/
   G4double SmearEnergy(const G4Track* aTrack, G4double aResolution = -1, FCCOutput::SaveType aSavePerigee  = FCCOutput::eNoSave);
/**
   First possible type of smearing. Smears the momentum with a given resolution.
   @param aTrackOriginal A track to smear.
   @param aResolution A resolution taken as a standard deviation of a Gaussian distribution.
*/
   G4ThreeVector SmearGaussian(const G4Track* aTrackOriginal, G4double aResolution);
/**
   Returns a random number from a Gaussian distribution.
   @param aMean A mean of the Gaussian distribution.
   @param aStandardDeviation A standard deviation of a Gaussian distribution.
*/
   G4double Gauss(G4double aMean, G4double aStandardDeviation);
/**
   Second possible type of smearing. Smears the perigee track representation.
   @param aTrackOriginal A track to smear.
   @param aSavePerigee Sets the information about the perigee for a given detector in FCCPrimaryParticleInformation.
*/
   G4ThreeVector SmearPerigee(const G4Track* aTrackOriginal, FCCOutput::SaveType aSavePerigee  = FCCOutput::eNoSave);
/**
   Returns the smeared perigee track representation according to AtlFast algorithm.
   @param aTrackOriginal A track to smear.
*/
   G4double* Atlfast(const G4Track* aTrackOriginal);
/**
   Computes the track perigee representation.
   @param aCharge A particle charge.
   @param aVertexMomentum A momentum.
   @param aVertexPosition A position.
*/
   G4double* ComputeTrackParams(G4double aCharge, G4ThreeVector aVertexMomentum, G4ThreeVector aVertexPosition);
/**
   Computes the particle position from the perigee representation.
   @param aParams A perigee representation.
   @param aPhiVertex Angle phi at vertex point.
*/
   G4ThreeVector ComputePosFromParams(G4double* aParams, G4double aPhiVertex);
/**
   Computes the particle momentum from the perigee representation.
   @param aParams A perigee representation.
*/
   G4ThreeVector ComputeMomFromParams(G4double* aParams);
/**
   Checks if an angle is within limits (-pi,pi)
   @param aPhi An angle (in radians).
*/
   G4double CheckPhi(G4double aPhi);

protected:
/**
   A default constructor.
*/
   FCCSmearer();
   ~FCCSmearer();

private:
/**
   A pointer to FCCSmearer object.
*/
   static FCCSmearer* fFCCSmearer;
/**
   CLHEP random engine used by AtlFast.
*/
   CLHEP::HepRandomEngine*    fRandomEngine;
/**
   CLHEP random engine used in gaussian smearing.
*/
   CLHEP::RandGauss*    fRandomGauss;
/**
   Atlfast manager responsible for reading the input data files for electrons.
*/
   Atlfast::ElectronMatrixManager* fElectronManager;
/**
   Atlfast manager responsible for reading the input data files for pions.
*/
   Atlfast::PionMatrixManager* fPionManager;
/**
   Atlfast manager responsible for reading the input data files for muons.
*/
   Atlfast::MuonMatrixManager* fMuonManager;
};

#endif
