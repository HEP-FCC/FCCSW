///////////////////////////////////////////////////////////////////
// ParametersT.icc, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometryUtils/ParamDefs.h"
#include "TrkParametersBase/ParametersBase.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>

// default constructor
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT():
ParametersBase<DIM,T>(),
m_surface(nullptr)
{}

// Constructor with local arguments - uses global <-> local for parameters
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT
(double loc1, double loc2, double phi, double theta, double qop,
 const S& surface,
 Alg::AmgSymMatrix<DIM>* covariance) :
ParametersBase<DIM,T>(covariance),
 m_surface (&surface)
//m_surface (surface.clone())
//m_surface( surface.isFree() ? surface.clone() : &surface )
{
    // fill the parameters
    ParametersBase<DIM,T>::m_parameters[Trk::loc1] = loc1;
    ParametersBase<DIM,T>::m_parameters[Trk::loc2] = loc2;
    ParametersBase<DIM,T>::m_parameters[Trk::phi] = phi;
    ParametersBase<DIM,T>::m_parameters[Trk::theta] = theta;
    ParametersBase<DIM,T>::m_parameters[Trk::qOverP] = qop;
   
    //<< loc1, loc2, phi, theta, qop;
    // decide the sign of the charge
    if (qop < 0.) ParametersBase<DIM,T>::m_chargeDefinition.flipSign();
    // fill momentum & then position using the surface
    double p = (qop != 0.) ? fabs(1./qop) : 10e5;
    // now calculate teh momentum
    ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(phi)*sin(theta),
                                                      p*sin(phi)*sin(theta),
                                                      p*cos(theta));
    m_surface->localToGlobal(ParametersBase<DIM,T>::localPosition(),
                             ParametersBase<DIM,T>::m_momentum,
                             ParametersBase<DIM,T>::m_position);
}

//Constructor with local arguments - uses global <-> local for parameters
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT
(const Alg::AmgVector<DIM>& parameters,
 const S& surface,
 Alg::AmgSymMatrix<DIM>* covariance) :
ParametersBase<DIM,T>(covariance),
m_surface(&surface)
//m_surface (surface.clone())
//m_surface( surface.isFree() ? surface.clone() : &surface )
{
    // decide the sign of the charge
    ParametersBase<DIM,T>::m_parameters = parameters;
    if (ParametersBase<DIM,T>::m_parameters[Trk::qOverP])
        ParametersBase<DIM,T>::m_chargeDefinition.flipSign();
    // fill momentum & then position using the surface
    double p = (ParametersBase<DIM,T>::m_parameters[Trk::qOverP] != 0.) ?  fabs(1./ParametersBase<DIM,T>::m_parameters[Trk::qOverP]) : 10e5;
    ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(ParametersBase<DIM,T>::m_parameters[Trk::phi])*sin(ParametersBase<DIM,T>::m_parameters[Trk::theta]),
                                                      p*sin(ParametersBase<DIM,T>::m_parameters[Trk::phi])*sin(ParametersBase<DIM,T>::m_parameters[Trk::theta]),
                                                      p*cos(ParametersBase<DIM,T>::m_parameters[Trk::theta]));
    m_surface->localToGlobal(ParametersBase<DIM,T>::localPosition(),
                             ParametersBase<DIM,T>::m_momentum,
                             ParametersBase<DIM,T>::m_position);
}

// Constructor with mixed arguments 1 - uses global <-> local for parameters
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT
(const Alg::Point3D& position,
 double phi, double theta, double qop,
 const S& surface,
 Alg::AmgSymMatrix<DIM>* covariance) :
ParametersBase<DIM,T>(position, covariance),
m_surface (&surface)
//m_surface (surface.clone())
//m_surface( surface.isFree() ? surface.clone() : &surface )
{
    // decide the sign of the charge
    if (qop) ParametersBase<DIM,T>::m_chargeDefinition.flipSign();
    // fill momentum & then position using the surface
    double p = (qop != 0.) ?  fabs(1./qop) : 10e5;
    ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(phi)*sin(theta),
                                                      p*sin(phi)*sin(theta),
                                                      p*cos(theta));
    // get the local parameters via the surface
    Alg::Point2D lPosition;
    m_surface->globalToLocal(ParametersBase<DIM,T>::m_position,
                             ParametersBase<DIM,T>::m_momentum, lPosition);
    // fill the vector now
    //ParametersBase<DIM,T>::m_parameters << lPosition.X(), lPosition.Y(), phi, theta, qop;
    
    ParametersBase<DIM,T>::m_parameters[Trk::loc1]  = lPosition.X();
    ParametersBase<DIM,T>::m_parameters[Trk::loc2]  = lPosition.Y();
    ParametersBase<DIM,T>::m_parameters[Trk::phi]   = phi;
    ParametersBase<DIM,T>::m_parameters[Trk::theta] = theta;
    ParametersBase<DIM,T>::m_parameters[Trk::qOverP]= qop;
}

// Constructor with mixed arguments 2 - uses global <-> local for parameters
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT
(double loc1, double loc2,
 const Alg::Vector3D& momentum,
 double charge,
 const S& surface,
 Alg::AmgSymMatrix<DIM>* covariance) :
ParametersBase<DIM,T>(covariance),
m_surface(&surface)
//m_surface (surface.clone())
//m_surface( surface.isFree() ? surface.clone() : &surface )
{
    // decide the sign of the charge
    if (charge < 0.) ParametersBase<DIM,T>::m_chargeDefinition.flipSign();
    // fill momentum & then position using the surface
    ParametersBase<DIM,T>::m_momentum = momentum;
    m_surface->localToGlobal(Alg::Point2D(loc1,loc2),
                             ParametersBase<DIM,T>::m_momentum,
                             ParametersBase<DIM,T>::m_position);
    // fill the vector now
    ParametersBase<DIM,T>::m_parameters[Trk::loc1]  = loc1;
    ParametersBase<DIM,T>::m_parameters[Trk::loc2]  = loc2;
    ParametersBase<DIM,T>::m_parameters[Trk::phi]   = ParametersBase<DIM,T>::m_momentum.phi();
    ParametersBase<DIM,T>::m_parameters[Trk::theta] = ParametersBase<DIM,T>::m_momentum.theta();
    
    double norm = (ParametersBase<DIM,T>::m_momentum.X())*(ParametersBase<DIM,T>::m_momentum.X());
    norm += (ParametersBase<DIM,T>::m_momentum.Y())*(ParametersBase<DIM,T>::m_momentum.Y());
    norm += (ParametersBase<DIM,T>::m_momentum.Z())*(ParametersBase<DIM,T>::m_momentum.Z());
    
    ParametersBase<DIM,T>::m_parameters[Trk::qOverP]= double(ParametersBase<DIM,T>::m_chargeDefinition)/sqrt(norm);
   // ParametersBase<DIM,T>::m_parameters << loc1, loc2, ParametersBase<DIM,T>::m_momentum.phi(), ParametersBase<DIM,T>::m_momentum.theta(), double(ParametersBase<DIM,T>::m_chargeDefinition)/ParametersBase<DIM,T>::m_momentum.norm();
}

// Constructor with global arguments - uses global <-> local for parameters */
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT
(const Alg::Point3D& position,
 const Alg::Vector3D& momentum,
 double charge,
 const S& surface,
 Alg::AmgSymMatrix<DIM>* covariance) :
ParametersBase<DIM,T>(position, momentum, covariance),
m_surface(&surface)
//m_surface (surface.clone())
//m_surface( surface.isFree() ? surface.clone() : &surface )
{
    // decide the sign of the charge
    if (charge < 0.) ParametersBase<DIM,T>::m_chargeDefinition.flipSign();
    // get the local parameters via the surface
    Alg::Point2D lPosition;
    m_surface->globalToLocal(ParametersBase<DIM,T>::m_position,
                             ParametersBase<DIM,T>::m_momentum, lPosition);
    // fill the vector now
    ParametersBase<DIM,T>::m_parameters[Trk::loc1] = lPosition.X();
    ParametersBase<DIM,T>::m_parameters[Trk::loc2] = lPosition.Y();
    ParametersBase<DIM,T>::m_parameters[Trk::phi] = ParametersBase<DIM,T>::m_momentum.phi();
    ParametersBase<DIM,T>::m_parameters[Trk::theta] = ParametersBase<DIM,T>::m_momentum.theta();
    
    double norm = (ParametersBase<DIM,T>::m_momentum.X())*(ParametersBase<DIM,T>::m_momentum.X());
    norm += (ParametersBase<DIM,T>::m_momentum.Y())*(ParametersBase<DIM,T>::m_momentum.Y());
    norm += (ParametersBase<DIM,T>::m_momentum.Z())*(ParametersBase<DIM,T>::m_momentum.Z());
    
    ParametersBase<DIM,T>::m_parameters[Trk::qOverP] = double(ParametersBase<DIM,T>::m_chargeDefinition)/sqrt(norm);
    //ParametersBase<DIM,T>::m_parameters << lPosition.X(), lPosition.Y(), ParametersBase<DIM,T>::m_momentum.phi(), ParametersBase<DIM,T>::m_momentum.theta(), double(ParametersBase<DIM,T>::m_chargeDefinition)/ParametersBase<DIM,T>::m_momentum.norm();
}

// Copy constructor
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT (const ParametersT& rhs):
ParametersBase<DIM,T>(rhs),
m_surface(rhs.m_surface){}
//m_surface (rhs.m_surface->clone()) {}
//m_surface(rhs.m_surface->isFree() ? rhs.m_surface->clone() : rhs.m_surface) {}

template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>& Trk::ParametersT<DIM, T, S>::operator=
(const Trk::ParametersT<DIM, T, S>& rhs)
{
    if (this!=&rhs)
    {
        //if ( m_surface->isFree() ) delete m_surface;
        m_surface = 0;
        // call the base class assignment operator
        Trk::ParametersBase<DIM,T>::operator=(rhs);
        m_surface             = rhs.m_surface;
        //m_surface           = ( rhs.m_surface->isFree() ? rhs.m_surface->clone() : rhs.m_surface );
    }
    return(*this);
}

// Destructor
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::~ParametersT ()
{
    //if ( m_surface && m_surface->isFree() ) delete m_surface;
   // m_surface = 0;
}

// PROTECTED Constructor with local arguments - persistency only, ownersip of surface given
template <int DIM, class T, class S> Trk::ParametersT<DIM, T, S>::ParametersT
(const Alg::AmgVector<DIM>& parameters,
 const S* surface,
 Alg::AmgSymMatrix<DIM>* covariance) :
ParametersBase<DIM,T>(covariance),
m_surface(surface)
{
    // decide the sign of the charge
    ParametersBase<DIM,T>::m_parameters = parameters;
    if (ParametersBase<DIM,T>::m_parameters[Trk::qOverP])
        ParametersBase<DIM,T>::m_chargeDefinition.flipSign();
    // fill momentum & then position using the surface
    double p = ((double)ParametersBase<DIM,T>::m_chargeDefinition)/ParametersBase<DIM,T>::m_parameters[Trk::qOverP];
    ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(ParametersBase<DIM,T>::m_parameters[Trk::phi])*sin(ParametersBase<DIM,T>::m_parameters[Trk::theta]),
                                                      p*sin(ParametersBase<DIM,T>::m_parameters[Trk::phi])*sin(ParametersBase<DIM,T>::m_parameters[Trk::theta]),
                                                      p*cos(ParametersBase<DIM,T>::m_parameters[Trk::theta]));
    m_surface->localToGlobal(ParametersBase<DIM,T>::localPosition(),
                             ParametersBase<DIM,T>::m_momentum,
                             ParametersBase<DIM,T>::m_position);
}

// Dedicated update method for non-curvilinear parameters - private and controlled by friendship
template <int DIM, class T, class S> void Trk::ParametersT<DIM, T, S>::updateParameter
(DefinedParameter updatedParameter,
 Alg::AmgSymMatrix<DIM>* updatedCovariance) const
{
    // update the parameters vector
    ParametersBase<DIM,T>::m_parameters[updatedParameter.second] = updatedParameter.first;
    // update the covariance
    if (updatedCovariance){
        if (updatedCovariance !=ParametersBase<DIM,T>:: m_covariance)
            delete ParametersBase<DIM,T>::m_covariance;
        ParametersBase<DIM,T>::m_covariance = updatedCovariance;
    }
    //position or momentum update needed
    if (updatedParameter.second == Trk::loc1 || updatedParameter.second == Trk::loc2){
        m_surface->localToGlobal(ParametersBase<DIM,T>::localPosition(),
                                 ParametersBase<DIM,T>::m_momentum,
                                 ParametersBase<DIM,T>::m_position);
    } else {
        // momentum update is needed
        double phi   = ParametersBase<DIM,T>::m_parameters[Trk::phi];
        double theta = ParametersBase<DIM,T>::m_parameters[Trk::theta];
        double p     = ParametersBase<DIM,T>::charge()/ParametersBase<DIM,T>::m_parameters[Trk::qOverP];
        ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(phi)*sin(theta),
                                                          p*sin(phi)*sin(theta),
                                                          p*cos(theta));
    }
}

// Dedicated update method for non-curvilinear parameters - private and controlled by friendship
template <int DIM, class T, class S> void Trk::ParametersT<DIM, T, S>::updateParameters
(const Alg::AmgVector<DIM>& updatedParameters,
 Alg::AmgSymMatrix<DIM>* updatedCovariance) const
{
    // valid to use != here, because value is either copied or modified,
    bool updatePosition = (updatedParameters[Trk::loc1] != ParametersBase<DIM,T>::m_parameters[Trk::loc1]) ||
    (updatedParameters[Trk::loc2] != ParametersBase<DIM,T>::m_parameters[Trk::loc2]);
    bool updateMomentum = (updatedParameters[Trk::phi] != ParametersBase<DIM,T>::m_parameters[Trk::phi]) ||
    (updatedParameters[Trk::theta] != ParametersBase<DIM,T>::m_parameters[Trk::theta]) ||
    (updatedParameters[Trk::qOverP] != ParametersBase<DIM,T>::m_parameters[Trk::qOverP]);
    // update the parameters vector
    ParametersBase<DIM,T>::m_parameters = updatedParameters;
    // update the covariance
    if (updatedCovariance){
        if (updatedCovariance !=ParametersBase<DIM,T>:: m_covariance)
            delete ParametersBase<DIM,T>::m_covariance;
        ParametersBase<DIM,T>::m_covariance = updatedCovariance;
    }
    //position or momentum update needed
    if (updatePosition){
        m_surface->localToGlobal(ParametersBase<DIM,T>::localPosition(),
                                 ParametersBase<DIM,T>::m_momentum,
                                 ParametersBase<DIM,T>::m_position);
    }
    // momentum update is needed
    if (updateMomentum){
        double phi   = ParametersBase<DIM,T>::m_parameters[Trk::phi];
        double theta = ParametersBase<DIM,T>::m_parameters[Trk::theta];
        double p     = ParametersBase<DIM,T>::charge()/ParametersBase<DIM,T>::m_parameters[Trk::qOverP];
        ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(phi)*sin(theta),
                                                          p*sin(phi)*sin(theta),
                                                          p*cos(theta));
    }
}


