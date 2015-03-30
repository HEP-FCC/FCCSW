///////////////////////////////////////////////////////////////////
// CurvilinerParametersT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_CURVILINEARPARAMETERS_T_H
#define TRKPARAMETERSBASE_CURVILINEARPARAMETERS_T_H

// Amg
//#include "GeoPrimitives/GeoPrimitives.h"
//#include "EventPrimitives/EventPrimitives.h"
// Tracking includes
#include "TrkParametersBase/ParametersBase.h"
#include "TrkGeometryUtils/DefinedParameter.h"
#include "TrkGeometryUtils/CurvilinearUVT.h"
//Alg
#include "Algebra/AlgPrimitives.h"

class MsgStream;

namespace Trk {

  /**
   @class CurvilinearParametersT

   The templated base class for charged and neutral representations:

   This class represents the track parameters as defined in the curvilinear frame.

   The method parameters() returns a vector of the track parameters, defined as:
   \f$\left(\begin{array}{c}
   locX\\locY\\\phi\\\theta\\q/p
   \end{array}\right)\f$

   The following is true per definition:
           - the local position is per definition (0.,0.)
           - the surface is per definition a plane surface normal to the track (UVT frame)

   The friend list is motivated by the most common manipulation needs

   @author andreas.salzburger@cern.ch

   */
   
template <int DIM, class T, class S> class CurvilinearParametersT : public ParametersBase<DIM, T>
{
  public:
      
    friend class MaterialEffectsEngine;  
      
    /**Default constructor for POOL. Do not use!*/
    CurvilinearParametersT(Alg::AmgSymMatrix<DIM>* covariance = 0);

    /** Create CurvilinearParametersT from DIM+2 parameters 
	  - these are: global position, momentum, charge, extension */
    CurvilinearParametersT(const Alg::AmgVector<DIM+2>& parameters, 
	                       Alg::AmgSymMatrix<DIM>* covariance = 0,
                           unsigned int cIdenfier = 0); 

    /**Create CurvilinearParametersT from mixed parameters: pos, local parameters*/
    CurvilinearParametersT (
        const Alg::Point3D& pos, 
        double tphi, 
        double ttheta, 
        double tqOverP,
        Alg::AmgSymMatrix<DIM>* covariance = 0,
        unsigned int cIdenfier = 0);
       
    /**Create CurvilinearParametersT from global parameters.
    -- it will throw a GaudiException if the position is not on surface
    */     
    CurvilinearParametersT (
        const Alg::Point3D& pos, 
        const Alg::Vector3D& mom,
        double charge,
        Alg::AmgSymMatrix<DIM>* covariance = 0,
        unsigned int cIdenfier = 0);
        
    /**Copy Constructor*/
    CurvilinearParametersT (const Trk::CurvilinearParametersT<DIM, T, S>&); 
   
    /**Destructor*/
    virtual ~CurvilinearParametersT (); 
                 
    /**Pseudo constructor*/             
    CurvilinearParametersT* clone() const { return new Trk::CurvilinearParametersT<DIM, T, S>(*this); }
                 
    /** Assignment operator*/
    Trk::CurvilinearParametersT<DIM, T, S> &operator= (const Trk::CurvilinearParametersT<DIM, T, S>&);

    /** return the curvilinear frame */
    const CurvilinearUVT& curvilinearFrame() const;

    /** return associated surface by pointer 
       - for curvilinear parameters it is a boundless planar surface*/
    virtual const S& associatedSurface() const ;

    /** the curvilinear parameters identifier */
    unsigned int cIdentifier() const;
    
    /**Dumps relevant information about the track parameters into the ostream.*/
    virtual MsgStream& dump( MsgStream& out ) const ;
    virtual std::ostream& dump( std::ostream& out ) const ;

    // ----------- POOL methods.
    /**Used by custom convertor to recreate the surface link.
       DO NOT USE THIS METHOD. It may be removed in the future*
       Senseless for Curvilinear parameters
     */
    void setAssociatedSurface(const Surface*) {}      

    /** Return the ParametersType enum */
    virtual ParametersType type() const { return Curvilinear; }

  private:
    /** --- PRIVATE METHOD: access is controlled via friend list 
        Update method for single defined parameters parameters,
        this deletes the covariance and overwrites if the pointer value differs  */ 
    void updateParameter(DefinedParameter updatedParameter,
                         Alg::AmgSymMatrix<DIM>* updatedCovariance = 0) const;
                                 
    /** ---- PRIVATE METHOD: access is controlled via friend list 
        Update method for measurement parameter update and material effects update,
        this deletes the covariance and overwrites if the pointer value differs  */ 
    void updateParameters(const Alg::AmgVector<DIM>& updatedParameters,
                          Alg::AmgSymMatrix<DIM>* updatedCovariance = 0) const;

    /** helper method for the curvilinear frame */
    mutable CurvilinearUVT  m_curvilinearFrame;  

    /** the plane surface where these parameters are defined at */
    mutable const S*        m_associatedSurface;

    /** the curvilinear parameters identifier */
    unsigned int m_cIdentifier;
  };
  
  template <int DIM, class T, class S> unsigned int CurvilinearParametersT<DIM,T,S>::cIdentifier() const 
  { return m_cIdentifier; }
  
}//end of namespace Trk

#include "TrkParametersBase/CurvilinearParametersT.ipp.h"
#endif
