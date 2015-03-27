#ifndef ATLFAST_IMATRIXMANAGER_H
#define ATLFAST_IMATRIXMANAGER_H

#include "G4Track.hh"
#include "AtlfastBinID.hh"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include <vector>


namespace Atlfast {
  using std::vector;

  /** @brief Defines the interface for Atlfast matrix managers
   *
   * These are used by TrackSmearer to provide smear matrix data
   * corresponding to a given track trajectory
   */
  class IMatrixManager {
  public:
     static IMatrixManager* Instance(){};
    /** returns track parameter offsets for smearing the given track trajectory */
    virtual vector<double> getVariables(const G4Track& track,
					CLHEP::HepSymMatrix& usedSigma) const = 0;
  protected:
    /** Virtual destructor */
     virtual ~IMatrixManager(){};
  };
}

#endif





