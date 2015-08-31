///////////////////////////////////////////////////////////////////
// BinningType.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNINGTYPE_H
#define TRKDETDESCRUTILS_BINNINGTYPE_H

namespace Trk {

  /** @enum BinningType, BinningOption & BinningAccess 
     
     - BinningType: 
     
       Enumeration to qualify the binning type for the use of the 
       LayerArrayCreator and the TrackingVolumeArrayCreator 

      - BinningOption:
        open:   [0,max]
        closed:  0 -> nextbin -> max

      - BinningAcces
        necessary access to global positions

      @author Andreas.Salzburger@cern.ch
     */
  enum BinningType { equidistant, biequidistant, arbitrary };
  
  /** enum BinValue */
  enum BinningOption { open, closed };

  /**  how to take the global / local position */                        
  enum BinningValue { binX, binY, binZ, binR, binPhi, binRPhi, binH, binEta };
  
  
}
#endif

