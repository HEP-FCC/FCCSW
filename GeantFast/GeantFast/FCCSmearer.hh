#ifndef FCC_SMEARER_H
#define FCC_SMEARER_H

#include "FCCOutput.hh"
#include "globals.hh"
#include "G4Track.hh"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandGauss.h"

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
   Smears the momentum with a given resolution. Perigee representation can be saved.
   @param aTrack A track to smear.
   @param aResolution A resolution. Gaussian smearing is done with a given resolution as a standard deviation.
*/
   G4ThreeVector SmearMomentum(const G4Track* aTrack, G4double aResolution = -1);
/**
   Smears the energy deposit with a given resolution. Perigee representation can be saved.
   @param aTrack A track to smear.
   @param aResolution A resolution. Gaussian smearing is done with a given resolution as a standard deviation.
*/
   G4double SmearEnergy(const G4Track* aTrack, G4double aResolution = -1);
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
   CLHEP random engine.
*/
   CLHEP::HepRandomEngine*    fRandomEngine;
/**
   CLHEP random engine used in gaussian smearing.
*/
   CLHEP::RandGauss*    fRandomGauss;
};

#endif
