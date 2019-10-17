#ifndef GENERATION_POISSONPILEUP_H
#define GENERATION_POISSONPILEUP_H

#include "FWCore/IPileUpTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class PoissonPileUp PoissonPileUp.h "ConstPileup.h"
 *
 *  Tool to generate number of pile-up events to be mixed with signal event.
 *  Concrete implementation of a IPileUpTool, returning a random variable
 *   drawn from a Poisson distribution centered around the average number
 *   of pileup events.
 *
 *  @author Valentin Volkl
 *  @date   2016-01-18
 */

class PoissonPileUp : public GaudiTool, virtual public IPileUpTool {
public:
  PoissonPileUp(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~PoissonPileUp();

  virtual StatusCode initialize();

  virtual unsigned int numberOfPileUp();

  virtual double getMeanPileUp();

  virtual void printPileUpCounters();

  virtual std::string getFilename();

private:
  /// average number of min bias events to pile on signal event.
  Gaudi::Property<double> m_meanPileUpEvents{this, "numPileUpEvents", 0, "Average number of pile-up events"};
  /// holds last realization of the random variable
  unsigned int m_currentNumPileUpEvents;
  /// Name of the file from which to read pile-up events
  Gaudi::Property<std::string> m_filename{this, "filename", "", "File from which to read pile-up events"};
  /// Poisson random number generator
  Rndm::Numbers m_PoissonDist;
};

#endif  // GENERATION_POISSONPILEUP_H
