#ifndef GENERATION_IVERTEXSMEARINGTOOL_H
#define GENERATION_IVERTEXSMEARINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "HepMC/GenEvent.h"

/** @class IVertexSmearingTool IVertexSmearingTool.h "IVertexSmearingTool.h"
 *  
 *  Abstract interface to vertex smearing tools. Concrete implementations 
 *  apply vertex smearing algorithms to each generated pile-up interactions.
 * 
 *  @author Patrick Robbe
 *  @author Daniel Funke
 *  @date   2005-08-17
 */

static const InterfaceID IID_IVertexSmearingTool( "IVertexSmearingTool" , 1 ,  0 ) ;

class IVertexSmearingTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IVertexSmearingTool ; }
  
  /// Smear the vertex of the interaction (independantly of the others)
  virtual StatusCode smearVertex( HepMC::GenEvent * theEvent ) = 0 ;
};
#endif // GENERATION_ISMEARINGTOOL_H
