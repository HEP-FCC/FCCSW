///////////////////////////////////////////////////////////////////
// CurvilinearParametersT.icc, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


//Trk
#include "TrkEventPrimitives/ParamDefs.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>
#include <math.h>
#include <assert.h>

// Default constructor
template <int DIM, class T, class S> Trk::CurvilinearParametersT<DIM, T, S>::CurvilinearParametersT(
                                                                                                    Alg::AmgSymMatrix<DIM>* covariance) :
ParametersBase<DIM, T>(covariance),
m_associatedSurface(0),
m_cIdentifier(0)
{}


// Constructor with TP arguments
template <int DIM, class T, class S> Trk::CurvilinearParametersT<DIM, T, S>::CurvilinearParametersT (
                                                                                                     const Alg::AmgVector<DIM+2>& parameters, Alg::AmgSymMatrix<DIM>* covariance, unsigned int cIdentifier) :
ParametersBase<DIM, T>(Alg::Point3D(parameters[x],parameters[y],parameters[z]),
                       Alg::Vector3D(parameters[3],parameters[4],parameters[5]),
                       covariance),
m_associatedSurface(0),
m_cIdentifier(cIdentifier)
{
    // flip the charge according to qOverP
    if (parameters[6] < 0.) Trk::ParametersBase<DIM, T>::m_chargeDefinition.flipSign();
    // assign the parameters
    Trk::ParametersBase<DIM, T>::m_parameters[locX]   = 0.;
    Trk::ParametersBase<DIM, T>::m_parameters[locY]   = 0.;
    // get phi & theta from the momentum vector
    Trk::ParametersBase<DIM, T>::m_parameters[phi]    = Trk::ParametersBase<DIM, T>::m_momentum.phi();
    Trk::ParametersBase<DIM, T>::m_parameters[theta]  = Trk::ParametersBase<DIM, T>::m_momentum.theta();
    Trk::ParametersBase<DIM, T>::m_parameters[qOverP] = parameters[6]/Trk::ParametersBase<DIM, T>::m_momentum.mag();
    // determine the curvilinear frame
    m_curvilinearFrame = Trk::CurvilinearUVT(Trk::ParametersBase<DIM, T>::m_momentum.unit());
}

// Constructor with TP arguments
template <int DIM, class T, class S> Trk::CurvilinearParametersT<DIM, T, S>::CurvilinearParametersT (
                                                                                                     const Alg::Point3D& pos,
                                                                                                     double tphi,
                                                                                                     double ttheta,
                                                                                                     double tqOverP,
                                                                                                     Alg::AmgSymMatrix<DIM>* covariance,
                                                                                                     unsigned int cIdentifier) :
ParametersBase<DIM, T>(pos,covariance),
m_associatedSurface(0),
m_cIdentifier(cIdentifier)
{
    // flip the charge according to qOverP
    if (tqOverP < 0.) Trk::ParametersBase<DIM, T>::m_chargeDefinition.flipSign();
    // assign the parameters
    Trk::ParametersBase<DIM, T>::m_parameters[locX]   = 0.;
    Trk::ParametersBase<DIM, T>::m_parameters[locY]   = 0.;
    Trk::ParametersBase<DIM, T>::m_parameters[phi]    = tphi;
    Trk::ParametersBase<DIM, T>::m_parameters[theta]  = ttheta;
    Trk::ParametersBase<DIM, T>::m_parameters[qOverP] = tqOverP;
    // make sure that the position & momentum are calculated
    double p = fabs(1./tqOverP);
    Trk::ParametersBase<DIM, T>::m_momentum = Alg::Vector3D(p*cos(tphi)*sin(ttheta),p*sin(tphi)*sin(ttheta),p*cos(ttheta));
    // determine the curvilinear frame
    m_curvilinearFrame = Trk::CurvilinearUVT(Trk::ParametersBase<DIM, T>::m_momentum.unit());
}

// Full global constructor
template <int DIM, class T, class S> Trk::CurvilinearParametersT<DIM, T, S>::CurvilinearParametersT (
                                                                                                     const Alg::Point3D& pos,
                                                                                                     const Alg::Vector3D& mom,
                                                                                                     double charge,
                                                                                                     Alg::AmgSymMatrix<DIM>* covariance,
                                                                                                     unsigned int cIdentifier) :
ParametersBase<DIM, T>(pos, mom, covariance),
m_curvilinearFrame(mom.unit()),
m_associatedSurface(0),
m_cIdentifier(cIdentifier)

{
    // assign the parameters
    Trk::ParametersBase<DIM, T>::m_parameters[locX]   = 0.;
    Trk::ParametersBase<DIM, T>::m_parameters[locY]   = 0.;
    Trk::ParametersBase<DIM, T>::m_parameters[phi]    = mom.phi();
    Trk::ParametersBase<DIM, T>::m_parameters[theta]  = mom.theta();
    if (Trk::ParametersBase<DIM, T>::m_chargeDefinition.value() == 0.) charge = 1.; // such that below is 1./mom.mag()
    Trk::ParametersBase<DIM, T>::m_parameters[qOverP] = charge/mom.mag();
}

// Copy constructor
template <int DIM, class T, class S> Trk::CurvilinearParametersT<DIM, T, S>::CurvilinearParametersT (const Trk::CurvilinearParametersT<DIM, T, S>& rhs) :
ParametersBase<DIM, T>(rhs),
m_curvilinearFrame(rhs.momentum().unit()),
m_associatedSurface(0),
m_cIdentifier(rhs.m_cIdentifier)
{}

// Destructor
template <int DIM, class T, class S> Trk::CurvilinearParametersT<DIM, T, S>::~CurvilinearParametersT (void)
{
    // curvilinear parameters always own their surfarce
    delete m_associatedSurface;
}

// Assignment operator
template <int DIM, class T, class S> Trk::CurvilinearParametersT<DIM, T, S>& Trk::CurvilinearParametersT<DIM, T, S>::operator= (const Trk::CurvilinearParametersT<DIM, T, S>& rhs)
{
    if (this!=&rhs)
    {
        Trk::ParametersBase<DIM, T>::operator=(rhs);
        // delete the surface, can be recreated
        delete m_associatedSurface; m_associatedSurface = 0;
        // assign the UVT framce
        m_curvilinearFrame = rhs.m_curvilinearFrame;
        // and the curvilinear identifier
        m_cIdentifier = rhs.m_cIdentifier;
    }
    return(*this);
}

// Screen output dump
template <int DIM, class T, class S> MsgStream& Trk::CurvilinearParametersT<DIM, T, S>::dump( MsgStream& out ) const
{
    out << "CurvilinearParametersT parameters:"                         << std::endl;
    Trk::ParametersBase<DIM, T>::dump(out);
    return out;
}

// Screen output dump
template <int DIM, class T, class S> std::ostream& Trk::CurvilinearParametersT<DIM, T, S>::dump( std::ostream& out ) const
{
    out << "CurvilinearParametersT parameters:"                         << std::endl;
    Trk::ParametersBase<DIM, T>::dump(out);
    return out;
}

// Curvilinear Frame definition
template <int DIM, class T, class S>  const Trk::CurvilinearUVT& Trk::CurvilinearParametersT<DIM, T, S>::curvilinearFrame() const
{
    return m_curvilinearFrame;
}

// Surface return (with on demand construction)
template <int DIM, class T, class S> const S& Trk::CurvilinearParametersT<DIM, T, S>::associatedSurface() const
{
    if (!m_associatedSurface){
        // create the surface for global position and global rotation
        m_associatedSurface = new S(Trk::ParametersBase<DIM,T>::m_position, m_curvilinearFrame);
    }
    return (*m_associatedSurface);
}

// Dedicated update method for non-curvilinear parameters - private and controlled by friendship
template <int DIM, class T, class S> void Trk::CurvilinearParametersT<DIM, T, S>::updateParameter
(DefinedParameter updatedParameter,
 Alg::AmgSymMatrix<DIM>* updatedCovariance) const
{
    // update the covariance
    if (updatedCovariance){
        if (updatedCovariance !=ParametersBase<DIM,T>:: m_covariance)
            delete ParametersBase<DIM,T>::m_covariance;
        ParametersBase<DIM,T>::m_covariance = updatedCovariance;
    }
    // first check if momentum update is done
    if (updatedParameter.second != Trk::loc1 && updatedParameter.second != Trk::loc2){
        // update the parameters vector - only if not local coordinates
        ParametersBase<DIM,T>::m_parameters[updatedParameter.second] = updatedParameter.first;
        // momentum update is needed
        double phi   = ParametersBase<DIM,T>::m_parameters[Trk::phi];
        double theta = ParametersBase<DIM,T>::m_parameters[Trk::theta];
        double p     = ParametersBase<DIM,T>::charge()/ParametersBase<DIM,T>::m_parameters[Trk::qOverP];
        ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(phi)*sin(theta),
                                                          p*sin(phi)*sin(theta),
                                                          p*cos(theta));
        // new momentum needs a new curvilinear UVT frame
        m_curvilinearFrame = Trk::CurvilinearUVT(Trk::ParametersBase<DIM, T>::m_momentum.unit());
    } else {
        // global position update - this requires an actual "shift" in the UVT frame, leaves local coordinates at (0,0)
        ParametersBase<DIM,T>::m_position += ( updatedParameter.second == Trk::loc1 ) ?
        updatedParameter.first * m_curvilinearFrame.curvU() : updatedParameter.first * m_curvilinearFrame.curvV();
    }
    // in any case, the (eventually existing) surface is invalidated
    delete m_associatedSurface; m_associatedSurface = 0;
}

// Dedicated update method for non-curvilinear parameters - private and controlled by friendship
template <int DIM, class T, class S> void Trk::CurvilinearParametersT<DIM, T, S>::updateParameters
(const Alg::AmgVector<DIM>& updatedParameters,
 Alg::AmgSymMatrix<DIM>* updatedCovariance) const
{
    
    // valid to use != here, because value is either copied or modified,
    bool updateMomentum = (updatedParameters[Trk::phi] != ParametersBase<DIM,T>::m_parameters[Trk::phi]) ||
    (updatedParameters[Trk::theta] != ParametersBase<DIM,T>::m_parameters[Trk::theta]) ||
    (updatedParameters[Trk::qOverP] != ParametersBase<DIM,T>::m_parameters[Trk::qOverP]);
    // update the covariance
    if (updatedCovariance){
        if (updatedCovariance !=ParametersBase<DIM,T>:: m_covariance)
            delete ParametersBase<DIM,T>::m_covariance;
        ParametersBase<DIM,T>::m_covariance = updatedCovariance;
    }
    // momentum update is needed
    if (updateMomentum){
        double phi   = updatedParameters[Trk::phi];
        double theta = updatedParameters[Trk::theta];
        double p     = ParametersBase<DIM,T>::charge()/updatedParameters[Trk::qOverP];
        // assign them and update the momentum 3 vector
        ParametersBase<DIM,T>::m_parameters[Trk::phi]    = phi;
        ParametersBase<DIM,T>::m_parameters[Trk::theta]  = theta;
        ParametersBase<DIM,T>::m_parameters[Trk::qOverP] = updatedParameters[Trk::qOverP];
        ParametersBase<DIM,T>::m_momentum = Alg::Vector3D(p*cos(phi)*sin(theta),
                                                          p*sin(phi)*sin(theta),
                                                          p*cos(theta));
        // new momentum needs a new curvilinear UVT frame
        m_curvilinearFrame = Trk::CurvilinearUVT(Trk::ParametersBase<DIM, T>::m_momentum.unit());                                                         
    }
    // position update if needed -  loc1
    if (updatedParameters[Trk::loc1] != 0.) 
        ParametersBase<DIM,T>::m_position += updatedParameters[Trk::loc1] * m_curvilinearFrame.curvU();
    // position update if needed -  loc2
    if (updatedParameters[Trk::loc2] != 0.) 
        ParametersBase<DIM,T>::m_position += updatedParameters[Trk::loc2] * m_curvilinearFrame.curvV();
    // in any case, the (eventually existing) surface is invalidated
    delete m_associatedSurface; m_associatedSurface = 0;
}


