#ifndef ATLFAST_IBINDATA_H
#define ATLFAST_IBINDATA_H
#include <cmath>
#include <vector>
#include "CLHEP/Matrix/SymMatrix.h"
#include "G4Track.hh"

//=================================================
//
// Bin Data
//
// Class to hold smearing matrix data
// and calculate correlation matrix for a given track
//==================================================


namespace Atlfast {

  /** @brief Defines the interface for Atlfast bin data objects.
   *
   * Class to hold smearing matrix data
   * The data is provided through the constructor 
   * in the format found in the flat file and the 
   * correlation matrix corresponding to the bin
   * can be calculated and returned via public methods.
   */
  using std::vector;


  class IBinData {
    
  public:

    /** Default virtual destructor */
    virtual ~IBinData() {}

    /** returns the correlation matrix depending on a random number supplied
     * as argument */
    virtual CLHEP::HepSymMatrix getMatrix(const G4Track& track) const=0;
  };
  
  
}
#endif







