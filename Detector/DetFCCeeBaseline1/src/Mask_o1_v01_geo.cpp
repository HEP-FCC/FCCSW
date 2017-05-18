//====================================================================
//  LCGeo - LC detector models in DD4hep 
//--------------------------------------------------------------------
// Mask based on the Beampipe driver (based on TubeX from Mokka), but not
// filling the centre with Beam vaccum, just places cylinders in arbitrary
// places with different alignments
//  A.Sailer, CERN
//  $Id: Mask_o1_v01_geo.cpp 567 2015-07-13 09:37:26Z gaede $
//====================================================================
#include "OtherDetectorHelpers.h"

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include <cmath>
#include <string>
// the following are added in order to create tracking surfaces
#include "DDRec/Surface.h"
#include "DDRec/DetectorData.h"
#include "XML/Utilities.h"
#include "XMLHandlerDB.h"

using DD4hep::Geometry::Transform3D;
using DD4hep::Geometry::Position;
using DD4hep::Geometry::RotationY;
using DD4hep::Geometry::RotateY;
using DD4hep::Geometry::ConeSegment;
using DD4hep::Geometry::SubtractionSolid;
using DD4hep::Geometry::Material;
using DD4hep::Geometry::Volume;
using DD4hep::Geometry::Solid;
using DD4hep::Geometry::Tube;
using DD4hep::Geometry::PlacedVolume;
using DD4hep::Geometry::Assembly;

using namespace DD4hep;
using namespace DD4hep::Geometry;
using namespace DDRec;
using namespace DDSurfaces ;

static DD4hep::Geometry::Ref_t create_element(DD4hep::Geometry::LCDD& lcdd,
					      xml_h xmlHandle,
					      DD4hep::Geometry::SensitiveDetector /*sens*/) {

  //------------------------------------------
  //  See comments starting with '//**' for
  //     hints on porting issues
  //------------------------------------------

  std::cout << "This is the Mask:"  << std::endl;

  //Access to the XML File
  xml_det_t xmlMask = xmlHandle;
  const std::string name = xmlMask.nameStr();

  //--------------------------------
  Assembly envelope( name + "_assembly"  ) ;
  //--------------------------------

  DD4hep::Geometry::DetElement tube(  name, xmlMask.id()  ) ;


  const double phi1 = 0 ;
  const double phi2 = 360.0*dd4hep::degree;
  
  //Parameters we have to know about
  DD4hep::XML::Component xmlParameter = xmlMask.child(_Unicode(parameter));
  const double crossingAngle  = xmlParameter.attr< double >(_Unicode(crossingangle))*0.5; //  only half the angle

  for(xml_coll_t c( xmlMask ,Unicode("section")); c; ++c) {

    xml_comp_t xmlSection( c );
    
    ODH::ECrossType crossType = ODH::getCrossType(xmlSection.attr< std::string >(_Unicode(type)));
    const double zStart       = xmlSection.attr< double > (_Unicode(start));
    const double zEnd         = xmlSection.attr< double > (_Unicode(end));
    const double rInnerStart  = xmlSection.attr< double > (_Unicode(rMin1));
    const double rInnerEnd    = xmlSection.attr< double > (_Unicode(rMin2));
    const double rOuterStart  = xmlSection.attr< double > (_Unicode(rMax1));
    const double rOuterEnd    = xmlSection.attr< double > (_Unicode(rMax2));
    const double thickness    = rOuterStart - rInnerStart;
    Material sectionMat  = lcdd.material(xmlSection.materialStr());
    const std::string volName      = "tube_" + xmlSection.nameStr();

    std::cout << std::setw(8) << zStart
	      << std::setw(8) << zEnd
	      << std::setw(8) << rInnerStart
	      << std::setw(8) << rInnerEnd
	      << std::setw(8) << rOuterStart
	      << std::setw(8) << rOuterEnd
	      << std::setw(8) << thickness
	      << std::setw(8) << crossType
	      << std::setw(15) << volName
	      << std::setw(15) << sectionMat.name()  << std::endl;    

    // things which can be calculated immediately
    const double zHalf       = fabs(zEnd - zStart) * 0.5; // half z length of the cone
    const double zPosition   = fabs(zEnd + zStart) * 0.5; // middle z position
    Material material    = sectionMat;

    // this could mess up your geometry, so better check it
    if (not ODH::checkForSensibleGeometry(crossingAngle, crossType)){
      throw std::runtime_error( " Mask_o1_v01_geo.cpp : checkForSensibleGeometry() failed " ) ;
    }

    const double rotateAngle = getCurrentAngle(crossingAngle, crossType); // for the placement at +z (better make it const now)
    const double mirrorAngle = M_PI - rotateAngle; // for the "mirrored" placement at -z
    // the "mirroring" in fact is done by a rotation of (almost) 180 degrees around the y-axis

    switch (crossType) {
    case ODH::kCenter:
    case ODH::kUpstream:
    case ODH::kDnstream: {
      // a volume on the z-axis, on the upstream branch, or on the downstream branch
      
      // absolute transformations for the placement in the world
      Transform3D transformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition), rotateAngle) );
      Transform3D transmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition), mirrorAngle) );

      std::cout <<  std::setw(35) << " $$$$$$$$$ I am puting a segment at " << zPosition << std::endl ;
      
      // solid for the tube (including vacuum and wall): a solid cone
      ConeSegment tubeSolid( zHalf, rInnerStart, rOuterStart, rInnerEnd, rOuterEnd , phi1, phi2);

      std::cout <<  std::setw(350) << " rInnerStart " << rInnerStart << " rOuterStart " << rOuterStart  << " rInnerEnd " << rInnerEnd << " rOuterEnd " << rOuterEnd  << std::endl ;
        
      // tube consists of vacuum
      Volume tubeLog( volName, tubeSolid, material ) ;
      Volume tubeLog2( volName, tubeSolid, material ) ;
      std::cout << std::setw(350) << " I am creating a tube of " << material.name() << " with Atomic number " << material.Z() <<  " with thickness " << thickness << std::endl ;
      tubeLog.setVisAttributes(lcdd, xmlMask.visStr() );
      
      //------------------------------------------------------------------
      // add surface for tracking ....
      //------------------------------------------------------------------
      //const bool isCylinder = ( rInnerStart == rInnerEnd );
      /*
      
      //if (isCylinder) {  // cylinder
      
      Vector3D ocyl(  rInnerStart + thickness/2.  , 0. , 0. ) ;
      
      VolCylinder cylSurf1( tubeLog , SurfaceType( SurfaceType::Helper ) , 0.5*thickness  , 0.5*thickness , ocyl );
      VolCylinder cylSurf2( tubeLog2, SurfaceType( SurfaceType::Helper ) , 0.5*thickness  , 0.5*thickness , ocyl );
      
      std::cout << std::setw(350) << "$$$$$$$$$$$$$ I am adding a surface with thickness "<< thickness << " at Z = " << zHalf << std::endl ;
      
      */
      //}
      //---------------------------------------------------------------------
      
      
      
      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume( tubeLog,  transformer );
      envelope.placeVolume( tubeLog2,  transmirror );
      
      //tube.setPlacement(envelope.placeVolume( tubeLog,  transformer ));
      //tube.setPlacement(envelope.placeVolume( tubeLog2,  transformer ));
      
      
      //volSurfaceList( tube )->push_back( cylSurf1 );
      //volSurfaceList( tube )->push_back( cylSurf2 );

      //std::cout << std::setw(350) << " length along u " << cylSurf1.length_along_u()  << " length along v " << cylSurf1.length_along_v() << std::endl ;

    }  
      break;
      
    case ODH::kPunchedCenter: {
      // a cone with one or two inner holes (two tubes are punched out)

      const double rUpstreamPunch = rInnerStart; // just alias names denoting what is meant here
      const double rDnstreamPunch = rInnerEnd; // (the database entries are "abused" in this case)

      // relative transformations for the composition of the SubtractionVolumes
      Transform3D upstreamTransformer(RotationY(-crossingAngle), Position(zPosition * tan(-crossingAngle), 0, 0));
      Transform3D dnstreamTransformer(RotationY(+crossingAngle), Position(zPosition * tan(+crossingAngle), 0, 0));

      // absolute transformations for the final placement in the world (angles always equal zero and 180 deg)
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition) , mirrorAngle) );

      // the main solid and the two pieces (only tubes, for the moment) which will be punched out
      ConeSegment wholeSolid(  zHalf, 0, rOuterStart, 0, rOuterEnd, phi1, phi2 );
      Solid tmpSolid0, tmpSolid1, finalSolid0, finalSolid1;

      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x

      if ( rUpstreamPunch > 1e-6 ) { // do we need a hole on the upstream branch?
	Tube upstreamPunch( 0, rUpstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
	tmpSolid0 = SubtractionSolid( wholeSolid, upstreamPunch, upstreamTransformer);
	tmpSolid1 = SubtractionSolid( wholeSolid, upstreamPunch, dnstreamTransformer); // [sic]
      } else { // dont't do anything, just pass on the unmodified shape
	tmpSolid0 = wholeSolid;
	tmpSolid1 = wholeSolid;
      }

      if (rDnstreamPunch > 1e-6 ) { // do we need a hole on the downstream branch?
	Tube dnstreamPunch( 0, rDnstreamPunch, 5 * zHalf, phi1, phi2); // a bit longer
	finalSolid0 = SubtractionSolid( tmpSolid0, dnstreamPunch, dnstreamTransformer);
	finalSolid1 = SubtractionSolid( tmpSolid1, dnstreamPunch, upstreamTransformer); // [sic]
      } else { // dont't do anything, just pass on the unmodified shape
	finalSolid0 = tmpSolid0;
	finalSolid1 = tmpSolid1;
      }

      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", finalSolid0, material );
      Volume tubeLog1( volName + "_1", finalSolid1, material );
      tubeLog0.setVisAttributes(lcdd, xmlMask.visStr() );
      tubeLog1.setVisAttributes(lcdd, xmlMask.visStr() );

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume( tubeLog0, placementTransformer );
      envelope.placeVolume( tubeLog1, placementTransmirror );

      break;
    }

    case ODH::kPunchedUpstream:
    case ODH::kPunchedDnstream: {
      // a volume on the upstream or downstream branch with two inner holes
      // (implemented as a cone from which another tube is punched out)
      
      const double rCenterPunch = (crossType == ODH::kPunchedUpstream) ? (rInnerStart) : (rInnerEnd); // just alias names denoting what is meant here
      const double rOffsetPunch = (crossType == ODH::kPunchedDnstream) ? (rInnerStart) : (rInnerEnd); // (the database entries are "abused" in this case)
      
      // relative transformations for the composition of the SubtractionVolumes
      Transform3D punchTransformer(RotationY(-2 * rotateAngle), Position(zPosition * tan(-2 * rotateAngle), 0, 0));
      Transform3D punchTransmirror(RotationY(+2 * rotateAngle), Position(zPosition * tan(+2 * rotateAngle), 0, 0));
      
      // absolute transformations for the final placement in the world
      Transform3D placementTransformer(RotationY(rotateAngle), RotateY( Position(0, 0, zPosition) , rotateAngle) );
      Transform3D placementTransmirror(RotationY(mirrorAngle), RotateY( Position(0, 0, zPosition) , mirrorAngle) );

      // the main solid and the piece (only a tube, for the moment) which will be punched out
      ConeSegment wholeSolid( zHalf, rCenterPunch , rOuterStart, rCenterPunch, rOuterEnd, phi1, phi2);
      Tube punchSolid( 0, rOffsetPunch, 5 * zHalf, phi1, phi2); // a bit longer
      
      // the punched subtraction solids can be asymmetric and therefore have to be created twice:
      // one time in the "right" way, another time in the "reverse" way, because the "mirroring"
      // rotation around the y-axis will not only exchange +z and -z, but also +x and -x
      SubtractionSolid finalSolid0( wholeSolid, punchSolid, punchTransformer);
      SubtractionSolid finalSolid1( wholeSolid, punchSolid, punchTransmirror);
      
      // tube consists of vacuum (will later have two different daughters)
      Volume tubeLog0( volName + "_0", finalSolid0, material );
      Volume tubeLog1( volName + "_1", finalSolid1, material );
      tubeLog0.setVisAttributes(lcdd, xmlMask.visStr() );
      tubeLog1.setVisAttributes(lcdd, xmlMask.visStr() );

      // placement of the tube in the world, both at +z and -z
      envelope.placeVolume( tubeLog0, placementTransformer );
      envelope.placeVolume( tubeLog1, placementTransmirror );
      
      break;
    }
    default: {
      throw std::runtime_error( " Mask_o1_v01_geo.cpp : fatal failure !! ??  " ) ;
    }

    }//end switch
  }//for all xmlSections

  //--------------------------------------
  Volume mother =  lcdd.pickMotherVolume( tube ) ;
  PlacedVolume pv(mother.placeVolume(envelope));
  pv.addPhysVolID( "system", xmlMask.id() ) ; //.addPhysVolID("side", 0 ) ;

  tube.setVisAttributes( lcdd, xmlMask.visStr(), envelope );

  tube.setPlacement(pv);
  
  return tube;
}
DECLARE_DETELEMENT(Mask_o1_v01,create_element)
