#include "SimpleSmear.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_COMPONENT(SimpleSmear)

SimpleSmear::SimpleSmear( const std::string& type,  const std::string& name,
                          const IInterface* parent ) : GaudiTool(type, name, parent), m_hSmMom(nullptr), m_hSmEn(nullptr)
{
   declareInterface<ISmearingTool>(this);
   declareProperty("sigma", m_sigma = 0.01);
   declareProperty("histograms", m_hist = true);
}

SimpleSmear::~SimpleSmear(){}

StatusCode SimpleSmear::initialize()
{
   if(GaudiTool::initialize().isFailure())
      return StatusCode::FAILURE;
   if(service( "RndmGenSvc" , m_randSvc ).isFailure())
   {
      error() << "Couldn't get RndmGenSvc" << endmsg;
      return StatusCode::FAILURE;
   }
   m_gauss.initialize(m_randSvc, Rndm::Gauss(1,m_sigma));
   if(m_hist)
   {
      if(service( "THistSvc" , m_tHistSvc ).isFailure())
      {
         error() << "Couldn't get THistSvc" << endmsg;
         return StatusCode::FAILURE;
      }
      m_hSmMom = new TH1F("hSmMom", "Relative momentum difference", 100, -1, 1);
      if (m_tHistSvc->regHist("/sim/SmMom", m_hSmMom).isFailure()) {
         error() << "Couldn't register histogram SmMom in THistSvc" << endmsg;
      }
      m_hSmEn = new TH1F("hSmEn", "Relative energy difference", 100, -1, 1);
      if (m_tHistSvc->regHist("/sim/SmEn", m_hSmEn).isFailure()) {
         error() << "Couldn't register histogram SmEn in THistSvc" << endmsg;
      }
   }
   return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearMomentum( G4ThreeVector& aMom )
{
   double tmp = m_gauss.shoot();
   aMom *= tmp;
   if(m_hist)
      m_hSmMom->Fill(tmp-1);
   return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearEnergy( G4double& aEn )
{
   double tmp;
   do
   {
      tmp = m_gauss.shoot();
      aEn *= tmp;
   }
   while(aEn<0);
   if(m_hist)
      m_hSmEn->Fill(tmp-1);
   return StatusCode::SUCCESS;
}
