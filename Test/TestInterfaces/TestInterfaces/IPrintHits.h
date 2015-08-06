//
//  IPrintHits.h
//  
//
//  Created by Julia Hrdinka on 29/04/15.
//
//

#ifndef IPRINTHITS_H
#define IPRINTHITS_H

#include "GaudiKernel/IAlgTool.h"

#include "Algebra/AlgPrimitives.h"

namespace Reco {
    class Surface;
    class ContainerVolume;
}

static const InterfaceID IID_IPrintHits ("IPrintHits", 1, 0);

class IPrintHits : virtual public IAlgTool {
    
public:
    static const InterfaceID& interfaceID() {
        return IID_IPrintHits;
    }
    
    virtual StatusCode printMaterial(std::shared_ptr<const Reco::ContainerVolume> worldVolume, const Alg::Point3D& start, size_t Nevents) = 0;
    virtual StatusCode printHits(std::vector<std::pair<double, const Alg::Vector3D>>& hits) = 0;
    virtual StatusCode printMaterial(std::shared_ptr<const Reco::ContainerVolume> worldVolume, const Alg::Point3D& start, std::vector<Alg::Vector3D> directions) = 0;
    
protected:
    ~IPrintHits() {}
};


#endif //IPRINTHITS_H
