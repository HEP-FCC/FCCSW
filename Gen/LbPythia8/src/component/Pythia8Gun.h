#ifndef LBPYTHIA8_PYTHIA8GUN_H
#define LBPYTHIA8_PYTHIA8GUN_H 1

// LbPythia8
#include "LbPythia8/GaudiRandomForPythia8.h" 
#include "LbPythia8/BeamToolForPythia8.h"
#include "LbPythia8/ILHAupFortranTool.h"

// Tool
#include "GaudiKernel/ToolFactory.h"

// Includes
#include "LbPythia8/Pythia8Production.h"

namespace GenMode {
  enum Mode { GaussMode=1, FlatMode };
}

//=============================================================================
// Header file for class: Pythia8Gun
// 
// 2013-08-21: chitsanu.khurewathanakul@epfl.ch
//
// Derived from JetProduction by Neal Gueissaz (2007)
//
//=============================================================================

class Pythia8Gun: public Pythia8Production {
  public:
    Pythia8Gun(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

    virtual ~Pythia8Gun();
    virtual StatusCode initialize();
    virtual StatusCode generateEvent(HepMC::GenEvent* theEvent,
                                     LHCb::GenCollision* theCollision);

  private:
    double generateValue(const int mode, const double mean,
                         const double sigma, const double min, const double max);

    std::vector<int> m_listPdg;
    
    // Energy
    double m_energyMin; 
    double m_energyMax;
    double m_energyMean;
    double m_energySigma;
    int m_energyGenMode;

    // Theta
    double m_thetaMin; 
    double m_thetaMax;
    double m_thetaMean;
    double m_thetaSigma;
    int m_thetaGenMode;

    // Theta
    double m_phiMin; 
    double m_phiMax;
    double m_phiMean;
    double m_phiSigma;
    int m_phiGenMode;

    //Define the method and the Flat random number generator
    // mutable IRndmGenSvc* m_RGS;
    // IRndmGenSvc* randSvc() const;
    Rndm::Numbers m_flatGenerator;
    Rndm::Numbers m_gaussGenerator;

};
#endif // LBPYTHIA8_PYTHIA8GUN_H
