//
//  Volume.cxx
//  
//
//  Created by Julia Hrdinka on 09/01/15.
//
//

#include "RecoGeometry/Volume.h"

Alg::Point3D    Reco::Volume::s_origin;
Alg::Transform3D Reco::Volume::s_idTransform;

Reco::Volume::Volume() :
m_center(0),
m_transform(0),
m_layers(0),
m_boundarySurfaces(),
m_volumeType(Reco::Volume::None),
m_translationType(Reco::Volume::none),
m_coordinates(3,0.)
{}

Reco::Volume::Volume(std::shared_ptr<const Alg::Transform3D> transf, LayerArray* layers) :
m_center(0),
m_transform(transf),
m_layers(layers),
m_boundarySurfaces(),
m_volumeType(Reco::Volume::None),
m_translationType(Reco::Volume::none),
m_coordinates(3,0.)
{}

Reco::Volume::Volume(std::shared_ptr<const Alg::Transform3D> transf) :
m_center(0),
m_transform(transf),
m_layers(0),
m_boundarySurfaces(),
m_volumeType(Reco::Volume::None),
m_translationType(Reco::Volume::none),
m_coordinates(3,0.)
{}

Reco::Volume::Volume(LayerArray* layers) :
m_center(0),
m_transform(0),
m_layers(layers),
m_boundarySurfaces(),
m_volumeType(Reco::Volume::None),
m_translationType(Reco::Volume::none),
m_coordinates(3,0.)
{}

Reco::Volume::Volume(LayerArray* layers, TGeoNode* node) :
m_center(0),
m_transform(0),
m_layers(layers),
m_boundarySurfaces(),
m_volumeType(Reco::Volume::None),
m_translationType(Reco::Volume::none),
m_coordinates(3,0.)
{
    const Double_t* rotation    = (node->GetMatrix()->GetRotationMatrix());
    const Double_t* translation = (node->GetMatrix()->GetTranslation());
    
    m_transform = std::make_shared<const Alg::Transform3D>(Alg::Transform3D(
                                       rotation[0], rotation[1], rotation[2], translation[0],
                                       rotation[3], rotation[4], rotation[5], translation[1],
                                       rotation[6], rotation[7], rotation[8], translation[2]));
    
}

Reco::Volume::Volume(TGeoNode* node) :
m_center(0),
m_transform(0),
m_layers(0),
m_boundarySurfaces(),
m_volumeType(Reco::Volume::None),
m_translationType(Reco::Volume::none),
m_coordinates(3,0.)
{
    const Double_t* rotation    = (node->GetMatrix()->GetRotationMatrix());
    const Double_t* translation = (node->GetMatrix()->GetTranslation());
    
    m_transform = m_transform = std::make_shared<const Alg::Transform3D>(Alg::Transform3D(
                                       rotation[0], rotation[1], rotation[2], translation[0],
                                       rotation[3], rotation[4], rotation[5], translation[1],
                                       rotation[6], rotation[7], rotation[8], translation[2]));

}

Reco::Volume::Volume(const Reco::Volume& volume) :
m_center(0),
m_transform(0),
m_layers(0),
m_boundarySurfaces(volume.m_boundarySurfaces),
m_volumeType(Reco::Volume::None),
m_translationType(Reco::Volume::none),
m_coordinates(volume.m_coordinates)
{
    m_transform = std::make_shared<const Alg::Transform3D>(volume.transform());
    m_layers = volume.m_layers;
}

Reco::Volume::~Volume()
{
    if (m_center) delete m_center;
    m_layers = 0;
}

Reco::Volume& Reco::Volume::operator=(const Reco::Volume& volume)
{
    if (this!=&volume) {
        delete m_center; m_center = 0;
        m_transform = volume.m_transform;
        m_layers = volume.m_layers;
        m_boundarySurfaces = volume.m_boundarySurfaces;
        m_volumeType = volume.m_volumeType;
        m_translationType = volume.m_translationType;
        m_coordinates = volume.m_coordinates;
    }
    return (*this);
}

const Alg::Point3D& Reco::Volume::center() const
{
    if (!m_transform && !m_center) return (s_origin);
    else if(!m_center) m_center = new Alg::Point3D(m_transform->Translation().Vect());
        return (*m_center);
    
}

const Alg::Transform3D& Reco::Volume::transform() const
{
    if(m_transform) return (*m_transform);
    return (s_idTransform);
}

int Reco::Volume::NumberOfSurfaces() const
{
    return (m_boundarySurfaces.size());
}

std::vector<std::weak_ptr<const Reco::BoundarySurface>> Reco::Volume::boundarySurfaces() const
{
    std::vector<std::weak_ptr<const Reco::BoundarySurface>> boundarySurf;
    for (auto it : m_boundarySurfaces) {
        boundarySurf.push_back(it);
    }
    return (boundarySurf);
}

const Reco::BoundarySurface* Reco::Volume::getBoundarySurface(size_t n) const
{
    if(n<m_boundarySurfaces.size()) return (m_boundarySurfaces.at(n).get());
    else return 0;
}

bool Reco::Volume::setBoundarySurface(size_t n,std::shared_ptr<const BoundarySurface> boundarysurface) const
{
    if (n<m_boundarySurfaces.size()) {
   //     m_boundarySurfaces.at(n).reset(boundarysurface);
        m_boundarySurfaces[n] = boundarysurface;
        return true;
    }
    else return false;
}

void Reco::Volume::setTranslationType(Reco::Volume::TranslationType translationType) const
{
    m_translationType = translationType;
}

Reco::Volume::TranslationType Reco::Volume::getTranslationType() const
{
    return m_translationType;
}

Reco::Volume::VolumeType Reco::Volume::type() const
{
    return m_volumeType;
}

double Reco::Volume::getCoordinate(size_t n) const
{
    return (m_coordinates.at(n));
}

std::vector<const Reco::Layer*> Reco::Volume::materialLayersOrdered(const Alg::Point3D& glopos,const Alg::Vector3D& mom, double) const
{   std::vector<const Reco::Layer*> result;
    std::vector<const Reco::Layer*> fulllayers;
    if (m_layers) {
        fulllayers = (m_layers->arrayObjects());
        if (isInside(glopos)) {
            const Trk::BinUtility* binutil = m_layers->binUtility();
            Alg::Point3D newpos = glopos;
            if (binutil->orderDirection(glopos,mom)==1)
            {
                int counter = 0;
                for(auto it = (fulllayers.cbegin()+binutil->bin(glopos)); it!=fulllayers.cend();++it)
                {
                    double path = 0;
                    //navigationlayer
                    if ((*it)->type()==Reco::Layer::navigation) {
                        if (counter ==0) {
                            const Reco::Layer* nextlayer = ((*it)->getNextLayer());
                            if (nextlayer) {
                                path = (nextlayer)->surfaceRepresentation()->straightLineIntersection(newpos,mom.Unit()).pathlength;
                                newpos = newpos + mom.Unit()*path;
                            }
                            else return (result);
                        }
                    }
                    else {
                        if((*it)->onLayer(newpos)) {
                            result.push_back(*it);
                            const Reco::Layer* nextlayer = ((*it)->getNextLayer());
                            if (nextlayer) {
                                path = (nextlayer)->surfaceRepresentation()->straightLineIntersection(newpos,mom.Unit()).pathlength;
                                newpos = newpos + mom.Unit()*path;
                            }
                            else return (result);
                        }
                        else return (result);
                    }
                
                    ++counter;
                } //for
            } //pos direction
            else {
                std::reverse(fulllayers.begin(),fulllayers.end());
                int counter = 0;
                int k = fulllayers.size() - (binutil->bin(glopos)+1);
                //        Alg::Point2D locpos;
                //         if(fulllayers.at(k)->surfaceRepresentation()->globalToLocal(glopos,mom,locpos)) {
                //              Alg::Vector3D normal = fulllayers[k].normal(locpos);
                //             if (normal.Dot(mom)==0) {
                //                 return (result);
                //             }
                //         }
                for(auto it = (fulllayers.cbegin()+k); it!=fulllayers.cend();++it)
                {
                
                    double path = 0;
                    //navigationlayer
                    if ((*it)->type()==0) {
                        if (counter ==0) {
                            const Reco::Layer* nextlayer = ((*it)->getPreviousLayer());
                            if (nextlayer) {
                                path = (nextlayer)->surfaceRepresentation()->straightLineIntersection(newpos,mom.Unit()).pathlength;
                                newpos = newpos + mom.Unit()*path;
                            }
                            else return (result);
                        }
                    }
                    else {
                        if((*it)->onLayer(newpos)) {
                            result.push_back(*it);
                            const Reco::Layer* nextlayer = ((*it)->getPreviousLayer());
                            if (nextlayer) {
                                path = (nextlayer)->surfaceRepresentation()->straightLineIntersection(newpos,mom.Unit()).pathlength;
                                newpos = newpos + mom.Unit()*path;
                            }
                            else return (result);
                        }
                        else return (result);
                    }
                
                    ++counter;
                } //for

            }
        
        } //isInside volume
    }
    return (result);
}
