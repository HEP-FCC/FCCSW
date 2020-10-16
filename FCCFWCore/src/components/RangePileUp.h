#ifndef GENERATION_RANGEPILEUP_H
#define GENERATION_RANGEPILEUP_H

#include "FWCore/IPileUpTool.h"
#include "GaudiAlg/GaudiTool.h"

/** @class RangePileUp 
 *
 *  Tool to generate number of pile-up events to be mixed with signal event.
 *  Concrete implementation of a IPileUpTool, returning a random variable
 *   drawn from a pre-defined list - useful for detailed detector studies of 
 *  the effects of increasing pile-up.
 *
 *  @author Valentin Volkl
 *  @date   2016-01-18
 */
class RangePileUp : public GaudiTool, virtual public IPileUpTool {
public:
  RangePileUp(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~RangePileUp();
  virtual StatusCode initialize();
  virtual unsigned int numberOfPileUp();
  virtual double getMeanPileUp();
  virtual void printPileUpCounters();

private:
  /// average number of min bias events to pile on signal event.
  Gaudi::Property<std::vector<int>> m_pileupRange{this, "numPileUpEvents", {0}, "Average number of pile-up events"};
  unsigned int m_currentNumPileUpEvents;
  unsigned int m_rangeIndex;
};

#endif  // GENERATION_RANGEPILEUP_H
