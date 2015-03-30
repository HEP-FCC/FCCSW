///////////////////////////////////////////////////////////////////
// ParametersBase.icc, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>

template <int DIM, class T> unsigned int Trk::ParametersBase<DIM, T>::s_numberOfInstantiations=0;

template <int DIM, class T> Trk::ParametersBase<DIM, T>::ParametersBase(Alg::AmgSymMatrix<DIM>* covariance) :
m_parameters(),
m_covariance(covariance),
m_chargeDefinition(T()),
m_position(),
m_momentum()
{
    for (int i = 0; i < DIM; i++) m_parameters[i] = 0.;
    //m_parameters.setZero();
    
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Surface, so increment total count
#endif
}

template <int DIM, class T> Trk::ParametersBase<DIM, T>::ParametersBase(const Alg::Point3D& pos, Alg::AmgSymMatrix<DIM>* covariance) :
m_parameters(),
m_covariance(covariance),
m_chargeDefinition(T()),
m_position(pos),
m_momentum()
{
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Surface, so increment total count
#endif
}


template <int DIM, class T> Trk::ParametersBase<DIM, T>::ParametersBase(const Alg::Point3D& pos, const Alg::Vector3D& mom, Alg::AmgSymMatrix<DIM>* covariance) :
m_parameters(),
m_covariance(covariance),
m_chargeDefinition(T()),
m_position(pos),
m_momentum(mom)
{
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Surface, so increment total count
#endif
}

template <int DIM, class T> Trk::ParametersBase<DIM, T>::ParametersBase(const Trk::ParametersBase<DIM, T>& pbas) :
m_parameters(pbas.m_parameters),
m_covariance(pbas.m_covariance ? new Alg::AmgSymMatrix<DIM>(*pbas.m_covariance) : 0),
m_chargeDefinition(pbas.m_chargeDefinition),
m_position(pbas.m_position),
m_momentum(pbas.m_momentum)
{
#ifndef NDEBUG
    s_numberOfInstantiations++; // new Surface, so increment total count
#endif
}

template <int DIM, class T> Trk::ParametersBase<DIM, T>::~ParametersBase()
{
    delete m_covariance;
#ifndef NDEBUG
    --s_numberOfInstantiations; // new Surface, so increment total count
#endif
}

template <int DIM, class T> Trk::ParametersBase<DIM, T>& Trk::ParametersBase<DIM, T>::operator=(const Trk::ParametersBase<DIM, T>& pbas)
{
    if (this != &pbas){
        // delete the covariance if tehre
        delete m_covariance; m_covariance = 0;
        // now assign
        m_parameters = pbas.m_parameters;
        m_covariance = pbas.m_covariance ? new Alg::AmgSymMatrix<DIM>(*pbas.m_covariance) : 0;
        m_position   = pbas.m_position;
        m_momentum   = pbas.m_momentum;
        m_chargeDefinition = pbas.m_chargeDefinition;
    }
    return(*this);
}


// private method for parameters update (controlled via friend list)
template <int DIM, class T>  Alg::AmgSymMatrix<DIM>* Trk::ParametersBase<DIM, T>::checkoutCovariance() const
{
    Alg::AmgSymMatrix<DIM>* coCovariance = m_covariance;
    m_covariance = 0;
    return coCovariance;
}

// overload of ostream operator
template <int DIM, class T> MsgStream& Trk::ParametersBase<DIM, T>::dump( MsgStream& sl ) const
{
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << " * TrackParameters on Surface"          << std::endl;
    sl << " * loc1  : "<< m_parameters[0]       << std::endl;
    sl << " * loc2  : "<< m_parameters[1]       << std::endl;
    sl << " * phi   : "<< m_parameters[2]        << std::endl;
    sl << " * Theta : "<< m_parameters[3]      << std::endl;
    sl << " * q/p   : "<< m_parameters[4]     << std::endl;
    if (m_parameters.Dim() > 5 )
        sl << " * mass  : "<< m_parameters[trkMass]     << " (extended parameters)" << std::endl;
    sl << " * corresponding global parameters:"    << std::endl;
    sl << " *    position  (x,  y,  z ) = (" << m_position.x() << ", "
    << m_position.y() << ", "
    << m_position.z() << ")" << std::endl;
    sl << " *    momentum  (px, py, pz) = (" << m_momentum.x() << ", "
    << m_momentum.y() << ", "
    << m_momentum.z() << ")" << std::endl;
    sl << std::setprecision(-1);
    return sl;
}

// overload of ostream operator
template <int DIM, class T> std::ostream& Trk::ParametersBase<DIM, T>::dump( std::ostream& sl ) const
{
    sl << std::setiosflags(std::ios::fixed);
    sl << std::setprecision(7);
    sl << " * TrackParameters on Surface"          << std::endl;
    sl << " * loc1  : "<< m_parameters[0]       << std::endl;
    sl << " * loc2  : "<< m_parameters[1]       << std::endl;
    sl << " * phi   : "<< m_parameters[2]        << std::endl;
    sl << " * Theta : "<< m_parameters[3]      << std::endl;
    sl << " * q/p   : "<< m_parameters[4]     << std::endl;
    if (m_parameters.Dim() > 5 )
        sl << " * mass  : "<< m_parameters[trkMass]     << " (extended parameters)" << std::endl;
    sl << " * corresponding global parameters:"    << std::endl;
    sl << " *    position  (x,  y,  z ) = (" << m_position.x() << ", "
    << m_position.y() << ", "
    << m_position.z() << ")" << std::endl;
    sl << " *    momentum  (px, py, pz) = (" << m_momentum.x() << ", "
    << m_momentum.y() << ", "
    << m_momentum.z() << ")" << std::endl;
    sl << std::setprecision(-1);
    return sl;
}

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/
template <int DIM, class T> MsgStream& Trk::operator << ( MsgStream& sl, const Trk::ParametersBase<DIM, T>& pbas)
{ return pbas.dump(sl); }

template <int DIM, class T> std::ostream& Trk::operator << ( std::ostream& sl, const Trk::ParametersBase<DIM, T>& pbas)
{ return pbas.dump(sl); }
