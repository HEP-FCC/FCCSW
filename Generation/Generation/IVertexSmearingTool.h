// $Id: IVertexSmearingTool.h,v 1.3 2005-12-31 17:30:37 robbep Exp $
#ifndef GENERATORS_IVERTEXSMEARINGTOOL_H 
#define GENERATORS_IVERTEXSMEARINGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "DataObjects/HepMCEntry.h"

/** @class IVertexSmearingTool IVertexSmearingTool.h "Generators/IVertexSmearingTool.h"
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
#endif // GENERATORS_ISMEARINGTOOL_H
