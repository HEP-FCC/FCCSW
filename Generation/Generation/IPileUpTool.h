#ifndef GENERATION_IPILEUPTOOL_H
#define GENERATION_IPILEUPTOOL_H

#include "GaudiKernel/IAlgTool.h"

/** @class IPileUpTool IPileUpTool.h "Generation/IPileUpTool.h"
 *
 *  Abstract interface to pile up tools. Generates the number of pile-up
 *  interactions to generate for each event.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

namespace HepMC {
class GenEvent;
}

static const InterfaceID IID_IPileUpTool("IPileUpTool", 3, 0);

class IPileUpTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IPileUpTool; }

  /** Computes the number of pile-up interactions in the event.
   *  @param[out] currentLuminosity  Luminosity of the current event.
   *  @return Number of pile-up interactions to generate.
   */
  virtual unsigned int numberOfPileUp() = 0;

  virtual double getMeanPileUp() = 0;

  /// Print various counters at the end of the job
  virtual void printPileUpCounters() = 0;

  virtual std::string getFilename() = 0;
};

#endif  // GENERATION_IPILEUPTOOL_H
