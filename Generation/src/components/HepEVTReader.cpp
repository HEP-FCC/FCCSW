
#include "HepEVTReader.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IEventProcessor.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

#include <math.h>

DECLARE_COMPONENT(HepEVTReader)

HepEVTReader::HepEVTReader(const std::string& name, ISvcLocator* svcLoc):
  GaudiAlgorithm(name, svcLoc),
  m_filename() {

  declareProperty("Filename", m_filename="", "Name of the HepMC file to read");
  declareProperty("genparticles", m_genphandle, "Generated particles collection (output)");
  declareProperty("genvertices", m_genvhandle, "Generated vertices collection (output)");

}

StatusCode HepEVTReader::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();

  m_input.open(m_filename.c_str(), std::ifstream::in);

  if ( !m_input.good() )   {
    error() << "Failed to open input stream:"+m_filename << endmsg;
    return StatusCode::FAILURE;
  }

  m_input >> NHEP;
  return StatusCode::SUCCESS;
}

StatusCode HepEVTReader::execute()
{
  // First check the input file status
  if(m_input.eof())
    {
      error() << "End of file reached" << endmsg;
      return StatusCode::FAILURE;
    }
  
  //  Loop over particles
  int ISTHEP;   // status code
  int IDHEP;    // PDG code
  int JMOHEP1;  // first mother
  int JMOHEP2;  // last mother
  int JDAHEP1;  // first daughter
  int JDAHEP2;  // last daughter
  double PHEP1; // px in GeV/c
  double PHEP2; // py in GeV/c
  double PHEP3; // pz in GeV/c
  double PHEP4; // energy in GeV
  double PHEP5; // mass in GeV/c**2
  double VHEP1; // x vertex position in mm
  double VHEP2; // y vertex position in mm
  double VHEP3; // z vertex position in mm
  double VHEP4; // production time in mm/c
  
  /*
  vector<int> daughter1;
  vector<int> daughter2;
  */
  fcc::MCParticleCollection* particles = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* vertices = new fcc::GenVertexCollection();


  for(int IHEP=0; IHEP<NHEP; IHEP++)
    {
    // if (m_format == HEPEvtShort)
    //   {
    // 	m_input >> ISTHEP >> IDHEP >> JDAHEP1 >> JDAHEP2
    // 		>> PHEP1 >> PHEP2 >> PHEP3 >> PHEP5;
    //   }
    // else
    //   {
      m_input >> ISTHEP >> IDHEP
	      >> JMOHEP1 >> JMOHEP2
	      >> JDAHEP1 >> JDAHEP2
	      >> PHEP1 >> PHEP2 >> PHEP3
	      >> PHEP4 >> PHEP5
	      >> VHEP1 >> VHEP2 >> VHEP3
	      >> VHEP4;
      // }
      
      if(m_input.eof())
	{
	  error() << "End of file reached before reading all the hits" << endmsg;
	  return StatusCode::FAILURE;
	}
      
      auto vertex = vertices->create();
      auto& position = vertex.position();
      
      position.x = VHEP1;
      position.y = VHEP2;
      position.z = VHEP3;
      vertex.ctau(VHEP4); // is ctau like this?

      fcc::MCParticle particle = particles->create();
      // PDGID
      particle.pdgId(IDHEP);
      particle.status(ISTHEP); // if ( ISTHEP == 1 ) status.set(G4PARTICLE_GEN_STABLE);

      double angle=15/1000.;
      double ta=tan(angle);
      auto& p4 = particle.p4();
      p4.px = PHEP1;
      p4.py = PHEP2;
      p4.pz = PHEP3;
      p4.mass = PHEP5;
      particle.startVertex(vertex);
    }

  m_genphandle.put(particles);
  m_genvhandle.put(vertices);
  return StatusCode::SUCCESS;
}


StatusCode HepEVTReader::finalize() 
{
  m_input.close();
  return GaudiAlgorithm::finalize();
}
