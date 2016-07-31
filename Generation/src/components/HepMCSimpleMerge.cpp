
#include "HepMCSimpleMerge.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IEventProcessor.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HepMCSimpleMerge)

HepMCSimpleMerge::HepMCSimpleMerge(
  const std::string& type,
  const std::string& name,
  const IInterface* parent)
  : GaudiTool(type, name, parent) {
  declareInterface< IHepMCMergeTool >( this );
}

HepMCSimpleMerge::~HepMCSimpleMerge() {
}

StatusCode HepMCSimpleMerge::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (!sc.isSuccess()) return sc;
  return sc;
}

StatusCode HepMCSimpleMerge::merge(HepMC::GenEvent& signalEvent, const std::vector<HepMC::GenEvent>& eventVector) {
  // iterate over vertices and add them to signalEvent
  for (auto it=eventVector.cbegin(), end = eventVector.cend(); it != end; ++it) {
    std::unordered_map<const HepMC::GenVertex*, HepMC::GenVertex*> inputToMergedVertexMap;
    for (auto v = (*it).vertices_begin(); v != (*it).vertices_end(); ++v ) {
        HepMC::GenVertex* newVertex = new HepMC::GenVertex((*v)->position());
        inputToMergedVertexMap[*v] = newVertex;
      }
    for (auto p = (*it).particles_begin();
          p != (*it).particles_end();
          ++p ) {
        // simple check if final-state particle:
        // has no end vertex and correct status code meaning no further decays
        if(!(*p)->end_vertex() && (*p)->status() == 1) {
          // ownership of the particle  (newParticle) is then given to the vertex (newVertex)
          HepMC::GenParticle* newParticle = new HepMC::GenParticle(**p);
          // each pile up particle is associated to a new production vertex
          // the position information is preserved
          // ownership of the vertex (newVertex) is given to the event (newEvent)
          HepMC::GenVertex* newVertex = inputToMergedVertexMap[(*p)->production_vertex()];
          newVertex->add_particle_out(newParticle);
          signalEvent.add_vertex(newVertex);
        }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode HepMCSimpleMerge::finalize() {
  return GaudiTool::finalize();
}
