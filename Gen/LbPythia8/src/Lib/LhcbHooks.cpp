// Local.
#include "LbPythia8/LhcbHooks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LhcbHooks
//
// 2014-04-06 : Philip Ilten
//-----------------------------------------------------------------------------

namespace Pythia8 {
  double LhcbHooks::multiplySigmaBy(const SigmaProcess* sigmaProcessPtr,
				    const PhaseSpace* phaseSpacePtr, bool) {
    
    // Initialize.
    if (!isInit) {

      // Determine the damping factor.
      double eCM    = phaseSpacePtr->ecm();
      double pT0Ref = settingsPtr->parm("MultipartonInteractions:pT0Ref");
      double ecmRef = settingsPtr->parm("MultipartonInteractions:ecmRef");
      double ecmPow = settingsPtr->parm("MultipartonInteractions:ecmPow");
      double pT0    = pT0Ref * pow(eCM / ecmRef, ecmPow);
      pT20          = pT0 * pT0;

      // Determine alpha strong.
      double aSvalue = settingsPtr->parm("MultipartonInteractions:alphaSvalue");
      double aSorder = settingsPtr->mode("MultipartonInteractions:alphaSorder");
      int    aSnfmax = settingsPtr->mode("StandardModel:alphaSnfmax");
      alphaS.init(aSvalue, aSorder, aSnfmax, false);
      isInit = true;
    }

    // Calculate the weight.
    double wt = 1;
    
    // All 2 -> 2 hard QCD processes.
    int code = sigmaProcessPtr->code();
    if (sigmaProcessPtr->nFinal() == 2 &&
	((code >= 400 && code <= 600) || (code >= 111 && code <= 124))) {
      
      // pT scale of process. Weight pT^4 / (pT^2 + pT0^2)^2
      double pTHat     = phaseSpacePtr->pTHat();
      double pT2       = pTHat * pTHat;
      wt               = pow2(pT2 / (pT20 + pT2));
      
      // Renormalization scale and assumed alpha_strong.
      double Q2RenOld  = sigmaProcessPtr->Q2Ren();
      double alphaSOld = sigmaProcessPtr->alphaSRen();
      
      // Reweight to new alpha_strong at new scale.
      double Q2RenNew  = pT20 + Q2RenOld;
      double alphaSNew = alphaS.alphaS(Q2RenNew);
      wt              *= pow2(alphaSNew / alphaSOld);
    }
    
    // Return weight.
    return wt;
  }
}
