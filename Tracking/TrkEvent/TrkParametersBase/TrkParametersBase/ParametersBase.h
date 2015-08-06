///////////////////////////////////////////////////////////////////
// ParametersBase.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERSBASE_H
#define TRKPARAMETERSBASE_PARAMETERSBASE_H

// Trk
#include "TrkGeometryUtils/ParamDefs.h"
#include "TrkGeometryUtils/DefinedParameter.h"
// Amg
//#include "EventPrimitives/EventPrimitives.h"
//#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi
#include "GaudiKernel/GaudiException.h"
//Alg
#include "Algebra/AlgPrimitives.h"



#ifndef TRKPARAMSDIM
#define TRKPARAMSDIM 5
#endif

#ifndef TRKEXTENDEDPARAMSDIM
#define TRKEXTENDEDPARAMSDIM 6
#endif

class MsgStream;
namespace Reco {
    class Surface;
}


namespace Trk {

  /** 
  @enum ParametersType
    
    Only two fundamental parameters types exist:
    AtaSurface & Curvilinear.
  
    From AtaSurface one can go directly to the Surface::type()
    enumeration.
  
  */
  enum ParametersType {
      AtaSurface =  0,
      Curvilinear = 1
  };
    
  /*enum _parameters
    {
        loc1    = 0,
        loc2    = 1,
        phi     = 2,
        theta   = 3,
        qOverP  = 4,
        trkMass = 5
    };*/

  /**
  @class ParametersBase

  The Base class for Neutral and Charged Track parameters.
  It represents the free state of a trajectory, represented by

  The position and the momentum are both given in the tracking
  reference frame.

  @author Andreas.Salzburger@cern.ch
  */

  template <int DIM, class T> class ParametersBase {
    public:
        
      friend class MaterialEffectsEngine;
        
      /** Default constructor */
      ParametersBase(Alg::AmgSymMatrix<DIM>* covariance = 0);
      
      /** Constructor with global position */
      ParametersBase(const Alg::Point3D& pos,
                     Alg::AmgSymMatrix<DIM>* covariance = 0);
      
      /** Constructor with glboal arguments */
      ParametersBase(const Alg::Point3D& pos,
                     const Alg::Vector3D& mom,
                     Alg::AmgSymMatrix<DIM>* covariance = 0);
      
      /** Copy constructor */
      ParametersBase(const ParametersBase& pbase);
      
      /** Desctructor */
      virtual ~ParametersBase();
      
      /** Assignmnet operator */
      ParametersBase& operator=(const ParametersBase& pbase);
      
      /** Access method for the parameters */
      const Alg::AmgVector<DIM>& parameters() const { return m_parameters; }             
      
      /** Access method for the covariance matrix - 0 if no covariance matrix is given */
      const Alg::AmgSymMatrix<DIM>* covariance() const { return m_covariance; }
      
      /** Access to the Surface method */
      virtual const Reco::Surface& associatedSurface() const = 0;
      
      /** Access method for the position */
      const Alg::Point3D& position() const  { return m_position; }
      
      /** Access method for the momentum */
      const Alg::Vector3D& momentum() const { return m_momentum; } 
      
      /** Access method for the local coordinates, \f$(loc1,loc2)\f$ 
          local parameter definitions differ for each surface type. */
      const Alg::Point2D localPosition() const 
        { return Alg::Point2D(m_parameters[0],m_parameters[1]); }
      
      /* Access method for transverse momentum */
      double pT() const { return sqrt(m_momentum.Perp2()); }
      
      /** Access method for pseudorapidity - from momentum */
      double eta() const { return m_momentum.Eta(); }
      
      /** Return number of parameters currently created - EDM monitoring */
      static unsigned int numberOfInstantiations() { return s_numberOfInstantiations; }
      
      /**Dumps relevant information about the track parameters into the ostream*/
      MsgStream&    dump( MsgStream& out ) const; 
      std::ostream& dump( std::ostream& out ) const ;
      
      /** Pseudo constructor - avoids excessive type-casting.
      @return new object copied from the concrete type of this object. 
      Ownership is passed (i.e. you must take care of deletion of resulting object)*/
      virtual ParametersBase* clone() const = 0;
      
      /** Return the ParametersType enum */
      virtual ParametersType type() const = 0;
      
      /** Returns charge of concrete type (i.e. must be implemented in inheriting classes)
         defined 0. if 1/pt=0*/
      double charge () const { return (double)m_chargeDefinition; };

    protected:
      /** --- PROTECTED METHOD: access is controlled via fiend list 
          Checkout method for covariance matrix,
          this sets the pointer to 0, but does not delete it, ownership is passed back */
      Alg::AmgSymMatrix<DIM>* checkoutCovariance() const;

      /** --- PROTECTED METHOD: access is controlled via friend list 
          Update method for single defined parameters parameters,
          this deletes the covariance and overwrites if the pointer value differs  */ 
      virtual void updateParameter(DefinedParameter updatedParameter,
                                   Alg::AmgSymMatrix<DIM>* updatedCovariance = 0) const = 0;
                                   
      /** ---- PROTECTED METHOD: access is controlled via friend list 
          Update method for measurement parameter update and material effects update,
          this deletes the covariance and overwrites if the pointer value differs  */ 
      virtual void updateParameters(const Alg::AmgVector<DIM>& updatedParameters,
                                    Alg::AmgSymMatrix<DIM>* updatedCovariance = 0) const = 0;
       
      mutable Alg::AmgVector<DIM>       m_parameters;       //!< contains the n parameters
      mutable Alg::AmgSymMatrix<DIM>*   m_covariance;       //!< contains the n x n covariance matrix
      mutable T                         m_chargeDefinition; //!< charge definition template
        
      mutable Alg::Point3D             m_position;         //!< point on track
      mutable Alg::Vector3D             m_momentum;         //!< momentum at this point on track

      /** number of objects of this type in memory */
      static unsigned int               s_numberOfInstantiations;
  };

      /**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
      template <int DIM, class T> MsgStream& operator << ( MsgStream& sl, const ParametersBase<DIM,T>& pbas);
      template <int DIM, class T> std::ostream& operator << ( std::ostream& sl, const ParametersBase<DIM,T>& pbas); 
} // end of namespace

#include "TrkParametersBase/ParametersBase.ipp.h"

#endif
