#ifndef LBPYTHIA8_LHCBHOOKS_H 
#define LBPYTHIA8_LHCBHOOKS_H 1

#include "Pythia8/Pythia.h"

/** @class LhcbHooks LhcbHooks.h LbPythia8/LhcbHooks.h
 *
 *  Custom LHCb user hooks used to modify default Pythia 8 event generation.
 *  Currently the hooks are used to provide pT damping of the QCD 2 -> 2
 *  processes.
 *
 *  @author Philip Ilten
 *  @date   2014-04-06
 */

namespace Pythia8 {
  class LhcbHooks : public UserHooks {
    
  public:
    
    /// Constructor.
    LhcbHooks() {isInit = false;}
    
    /// Modifiy cross-section.
    virtual bool canModifySigma() {return true;}
    
    /// Cross-section weight.
    virtual double multiplySigmaBy(const SigmaProcess* sigmaProcessPtr,
				   const PhaseSpace* phaseSpacePtr, bool);
    
  private:
    
    // Members.
    bool   isInit;      ///< Flag whether the object has been initialized.
    double pT20;        ///< Stored value of the pT damping parameter.
    AlphaStrong alphaS; ///< Stored value of alpha strong.
  };
}

#endif // LBPYTHIA8_LHCBHOOKS_H
