#include "EventAction.h"
#include "G4Event.hh"

#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

EventAction::EventAction() : G4UserEventAction()
{
   m_particleCollection = new ParticleCollection();
}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event* /*aEvent*/)
{}

void EventAction::EndOfEventAction(const G4Event* aEvent)
{
   // ParticleCollection* particles = new ParticleCollection();
   ParticleHandle outptc = m_particleCollection->create();
   BareParticle& core = outptc.mod().Core;
   G4PrimaryParticle* g4p;
   G4PrimaryVertex* g4v;
   for(int i=0;i<aEvent->GetNumberOfPrimaryVertex();++i)
   {
      g4v = aEvent->GetPrimaryVertex();
      g4p = g4v->GetPrimary();
      for(int j=0;j<g4v->GetNumberOfParticle();++j)
      {
         core.Type = g4p->GetPDGcode();
         core.Status = 0;//ptc.status();
         core.P4.Px = g4p->GetMomentum().x();
         core.P4.Py = g4p->GetMomentum().y();
         core.P4.Pz = g4p->GetMomentum().z();
         core.P4.Mass = g4p->GetMass();
         g4p->GetNext();
         std::cout<<g4p->GetPDGcode()<<std::endl;
      }
         g4v->GetNext();
   }
   // m_particleCollection.put(particles);
}
