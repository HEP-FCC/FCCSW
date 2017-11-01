#ifndef GENERATION_CONSTPILEUP_H
#define GENERATION_CONSTPILEUP_H

#include "FWCore/IPileUpTool.h"

#include "GaudiAlg/GaudiTool.h"

/** @class ConstPileUp ConstPileUp.h "ConstPileup.h"
 *
 *  Tool to generate number of pile-up events to be mixed with signal event.
 *  Concrete implementation of a IPileUpTool -- the most trivial one, actually,
 *  returning just a constant (that can be specified as a property).
 *
 *  @author Valentin Volkl
 *  @date   2015-12-16
 */

class ConstPileUp : public GaudiTool, virtual public IPileUpTool {
public:
  /// Standard constructor
  ConstPileUp(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~ConstPileUp();  ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();

  /** Implements IPileUpTool::numberOfPileUp.
   */
  virtual unsigned int numberOfPileUp();

  virtual double getMeanPileUp();
  /** Implements IPileUpTool::printPileUpCounters.
   */
  virtual void printPileUpCounters();

  virtual std::string getFilename();

private:
  /// Number of min bias events to pile on signal event.
  Gaudi::Property<unsigned int> m_numPileUpEvents{this, "numPileUpEvents", 0, "number of pile-up events"};
  /// Name of the file from which to read the pile-up
  Gaudi::Property<std::string> m_filename{this, "filename", "", "File from which to read the pile-up"};
};

#endif  // GENERATION_CONSTPILEUP_H
