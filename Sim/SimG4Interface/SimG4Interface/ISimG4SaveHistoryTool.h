#ifndef SIMG4INTERFACE_ISIMG4SAVEHISTORYTOOL_H
#define SIMG4INTERFACE_ISIMG4SAVEHISTORYTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "SimG4Interface/ISimG4ParticleHistory.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"


// Geant
class G4Event;


/** @class ISimG4SaveHistoryTool SimG4Interface/SimG4Interface/ISimG4SaveHistoryTool.h ISimG4SaveHistoryTool.h
 *
 * Interface for tool that saves the particle history. Save functionality derived from ISimG4SaveHistoryTool, this adds
 * retrieval of the history
 *
 *  @author J. Lingemann
 */

class ISimG4SaveHistoryTool : virtual public ISimG4SaveOutputTool {
public:
  DeclareInterfaceID(ISimG4SaveHistoryTool, 1, 0);

  // This resets the particle history
  virtual void reset() = 0;
};
#endif /* SIMG4INTERFACE_ISIMG4SAVEHISTORYTOOL_H */
