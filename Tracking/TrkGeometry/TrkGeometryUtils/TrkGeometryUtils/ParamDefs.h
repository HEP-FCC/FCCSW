///////////////////////////////////////////////////////////////////
// ParamDefs.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_PARAMDEFS_H
#define TRKEVENTPRIMITIVES_PARAMDEFS_H

#include <vector>
#include <string>

namespace Trk {

 /** @enum ParamDefs 
     @brief This file defines the parameter enums in the Trk namespace.

     Note that the enums refer to the [] operator and not to the () operator
     which makes a big difference in classes inherited or used from CLHEP.
     CLHEP::HepVector and not CLHEP::Hep2Vector is taken, as CLHEP::Hep2Vector provides
     additional functions like x(), y(), phi() which might be meaningless
     when using specific natural detector frames.
   
     <b>Usage examples:</b>
     - Access the y-coordinate of the cartesian local frame:<br>
       \c LocalPosition \c locpos(2.3, 4.5);<br>
       \c double \c x = \c locpos[Trk::locX];
     - Access the eta-value of a track state on a surface (Tsos):<br>
       \c double \c theEta = \c Tsos[Trk::eta]
     - Access the eta-value of a track state on a surface (Tsos):<br>
       \c double \c theEta = \c Tsos[Trk::eta]
   
    @author Andreas.Salzburger@cern.ch
   */

   enum ParamDefs        {
   // Enums for LocalParameters - LocalPosition/
                            loc1=0, loc2=1,                     //!< generic first and second local coordinate
                            locX=0, locY=1 ,                    //!< local cartesian
                            locRPhi=0, locPhiR=0, locZ=1,       //!< local cylindrical
                            locR=0, locPhi=1,                   //!< local polar
                            iPhi=0, iEta=1,                     //!< (old readout) will be skipped
                            distPhi=0, distEta=1,               //!< readout for silicon
                            driftRadius=0,                      //!< trt, straws
   // Enums for const Amg::Vector3D & GlobalMomentum /
                            x=0,   y=1,  z=2,                   //!< global position (cartesian)
                            px=0, py=1, pz=2,                   //!< global momentum (cartesian)
   // Enums for Perigee //
                            d0=0, z0=1, phi0=2, theta=3, qOverP=4, //!< perigee
    /* Enums for TrackState on Surfaces
     The first two enums in the TrackParameters refer to the local Frame, i.e.
      - LocalCartesian for AtanArbitraryPlane
      - LocalCylindrical for AtaCylinder (includes line)
      - LocalPolar for AtaDisc
      The other three enums are standard \f$(\phi, \eta, \frac{q}{p_{T}})\f$
     */
                            phi=2,
   /**Enums for curvilinear frames*/
                            u=0,  v=1,        
			    
  /** Extended perigee: mass*/			    
			    trkMass=5
			    
                            };                            //!< parameters on surface
			    			    			    
  /** @struct ParamDefsAccessor
      
      Simple struct to access the 
      ParamDefs enum with ints
      
      @author Andreas.Salzburger@cern.ch
    */
  struct ParamDefsAccessor {
     /**Constructor*/
     ParamDefsAccessor()
      { 
        pardef.reserve(6);

        pardef.push_back(Trk::loc1);
        pardef.push_back(Trk::loc2);
        pardef.push_back(Trk::phi);
        pardef.push_back(Trk::theta);
        pardef.push_back(Trk::qOverP);
	pardef.push_back(Trk::trkMass);
      }
     /**The member*/ 
     std::vector<ParamDefs> pardef;
  
  };    
      
  /** @struct ParamDefsStrings
      
      Simple struct to access the 
      strings of the ParamDefs enum names 
      for output reasons
      
      @author Andreas.Salzburger@cern.ch
    */
  struct ParamDefsStrings {
     /**Constructor*/
     ParamDefsStrings()
      { 
        pardefname.reserve(6);

        pardefname.push_back("loc1");
        pardefname.push_back("loc2");
        pardefname.push_back("phi");
        pardefname.push_back("theta");
        pardefname.push_back("qOverP");
        pardefname.push_back("trkMass");
      }
     
     /**The member*/ 
     std::vector<std::string> pardefname;
  
  };                                    
                            
  
} // end of namespace

#endif // TRKEVENTPRIMITIVES_PARAMDEFS_H

