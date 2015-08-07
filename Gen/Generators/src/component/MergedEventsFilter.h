// $Id: MergedEventsFilter.h,v 1.1 2008-05-06 08:27:55 gcorti Exp $
#ifndef GENERATORS_MERGEDEVENTSFILTER_H 
#define GENERATORS_MERGEDEVENTSFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IFullGenEventCutTool;

/** @class MergedEventsFilter MergedEventsFilter.h component/MergedEventsFilter.h
 *  
 *
 *  @author Gloria CORTI
 *  @date   2008-04-30
 */
class MergedEventsFilter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MergedEventsFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MergedEventsFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
 
  std::string  m_hepMCEventLocation ;    ///< Input TES for HepMC events
  std::string  m_genCollisionLocation ;  ///< Input TES for GenCollisions

  std::string           m_fullGenEventCutToolName; ///< Name of event cut tool
  IFullGenEventCutTool* m_fullGenEventCutTool;     ///< Pointer to event cut tool

};
#endif // GENERATORS_MERGEDEVENTSFILTER_H
