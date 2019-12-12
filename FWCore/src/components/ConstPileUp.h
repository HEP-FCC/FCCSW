#ifndef GENERATION_CONSTPILEUP_H
#define GENERATION_CONSTPILEUP_H

#include "FWCore/IPileUpTool.h"
#include "GaudiAlg/GaudiTool.h"

/** @class ConstPileUp 
 *
 *  Tool to generate number of pile-up events to be mixed with signal event.
 *  Concrete implementation of a IPileUpTool -- the most trivial one, actually,
 *  returning just a constant (that can be specified as a property).
 *  The interface is kept close to the originals in LHCb's Gauss(ino). 
 *
 *  @author Valentin Volkl
 *  @date   2015-12-16
 */
class ConstPileUp : public GaudiTool, virtual public IPileUpTool {
public:
  ConstPileUp(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ConstPileUp();
  virtual StatusCode initialize();
  /// in the special case of constant pileup,
  /// this method is identical to getMeanPileUp
  virtual unsigned int numberOfPileUp();
  virtual double getMeanPileUp();
  virtual void printPileUpCounters();

private:
  /// Number of min bias events to pile on signal event.
  Gaudi::Property<unsigned int> m_numPileUpEvents{this, "numPileUpEvents", 0, "number of pile-up events"};
};

#endif  // GENERATION_CONSTPILEUP_H
