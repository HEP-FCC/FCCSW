//
//  DD4HepDetDesSvc.cxx
//  
//
//  Created by Julia Hrdinka on 13/10/14.
//
//

#include "DetDesServices/DD4HepDetDesSvc.h"

using namespace Gaudi;

DECLARE_COMPONENT(DD4HepDetDesSvc)

DD4HepDetDesSvc::DD4HepDetDesSvc(const std::string& name, ISvcLocator* svc)
: base_class(name, svc),
  m_lcdd(0),
  m_xmlFileName("file:DetectorDescription/Detectors/compact/TestTracker.xml"), //over joboptions
  m_log(msgSvc(), name)
{}

DD4HepDetDesSvc::~DD4HepDetDesSvc()
{
    
    if (destroyDetector().isFailure())
        m_log << MSG::WARNING << "Could not call destroyDetector() successfully." << endmsg;
    else
        m_log << MSG::INFO << "destroyDetector() returned SUCCESSFUL." << endmsg;
}

StatusCode
DD4HepDetDesSvc::initialize ()
{
    // we retrieve the the static instance of the DD4HEP::Geometry
    m_lcdd = &(DD4hep::Geometry::LCDD::getInstance());
    m_lcdd->addExtension<IDetDesSvc>(this);
    
    if (buildDetector().isFailure())
        m_log << MSG::WARNING << "Could not call buildDetector() successfully." << endmsg;
    else
        m_log << MSG::INFO << "buildDetector() returned SUCCESSFUL." << endmsg;
    
    IIncidentSvc* incidentsvc;
    StatusCode sc = service("IncidentSvc", incidentsvc);
    Incident detinc ("DD4HepDetDesSvc","DD4HepDetectorBuild");
    if (sc.isSuccess() ) {
        incidentsvc->fireIncident(detinc);
    }

    return StatusCode::SUCCESS;
}

StatusCode
DD4HepDetDesSvc::finalize ()
{
    
    return StatusCode::SUCCESS;
}


void DD4HepDetDesSvc::handle(const Incident& inc)
{
    m_log << MSG::INFO << "Handling incident type'" << inc.type() << "'" << endmsg;
    m_log << MSG::INFO << "Incident source '" << inc.source() << "'" << endmsg;
}

StatusCode
DD4HepDetDesSvc::buildDetector ()
{
   m_log << MSG::INFO << "buildDetector() called." << endmsg;
    
    //load geometry
   m_log << MSG::INFO << "loading geometry from file:  '" << m_xmlFileName << "'" << endmsg;
   char* arg = (char*) m_xmlFileName.c_str();
   m_lcdd->apply("DD4hepXMLLoader", 1, &arg);
    
  return StatusCode::SUCCESS;
}

StatusCode
DD4HepDetDesSvc::destroyDetector ()
{
    //m_lcdd->removeExtension <IDetDesSvc> (false);
    m_lcdd->destroyInstance();
    return StatusCode::SUCCESS;
}

DD4hep::Geometry::LCDD* DD4HepDetDesSvc::lcdd()
{
    return (m_lcdd);
}

