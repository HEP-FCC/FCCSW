///////////////////////////////////////////////////////////////////
// CurvilinearUVT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Trk
#include "TrkGeometryUtils/CurvilinearUVT.h"
//#include "TrkEventPrimitives/CurvilinearUVT.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"

Trk::CurvilinearUVT::~CurvilinearUVT()
{}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& Trk::operator << ( MsgStream& sl, const Trk::CurvilinearUVT& uvt)
{ 
    sl <<  "Trk::CuvilinearUVT - curvilinear frame (u,v,t)" << std::endl;
    sl << "          u = " << uvt.curvU() << std::endl;
    sl << "          v = " << uvt.curvV() << std::endl;
    sl << "          t = " << uvt.curvT() << std::endl;
    
    return sl;   
}

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::CurvilinearUVT& uvt)
{ 
    sl <<  "Trk::CuvilinearUVT - curvilinear frame (u,v,t)" << std::endl;
    sl << "          u = " << uvt.curvU() << std::endl;
    sl << "          v = " << uvt.curvV() << std::endl;
    sl << "          t = " << uvt.curvT() << std::endl;    

    return sl;
}   

