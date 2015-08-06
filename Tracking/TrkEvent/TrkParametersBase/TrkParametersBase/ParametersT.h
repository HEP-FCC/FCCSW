///////////////////////////////////////////////////////////////////
// ParametersT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPARAMETERSBASE_PARAMETERS_T_H
#define TRKPARAMETERSBASE_PARAMETERS_T_H

// Trk includes
#include "TrkParametersBase/ParametersBase.h"
#include "TrkGeometryUtils/DefinedParameter.h"
// Amg
//#include "GeoPrimitives/GeoPrimitives.h"
//#include "EventPrimitives/EventPrimitives.h"
//Alg
#include "Algebra/AlgPrimitives.h"

#include <assert.h>
#include <cmath>

class MsgStream;

namespace Trk {
  /**
  @class ParametersT

  The templated base class for charged and neutral representations,
  template argumetns are the type and the surface
    
    - the parameters vector
	- charge
	- position
	- momentum
	- local position (ponter to)

	Which can be returned by the parameters() method as a Alg::Vector(DIM):
	\left(\begin{array}{c}
	x\\y\\z\\p_{x}\\p_{y}\\p_{z}\end{array}\right)
	\f$

	The standard dimension for both charged and neutral parameters
	is of dimension 5, extended parameters will have dimension 6.
    
    The friend list is motivated by the most common manipulation needs

	@author edward.moyse@cern.ch, andreas.salzburger@cern.ch
  */
    
  template <int DIM, class T, class S> class ParametersT : public ParametersBase<DIM, T> {
  
    public:
         
      friend class MaterialEffectsEngine;
          
        
  	  /** default constructor */
      ParametersT ();
  
      /** Constructor with local arguments - uses global <-> local for parameters */
      ParametersT (double loc1, double loc2, double phi, double theta, double qop,
                   const S& surface,
  				   Alg::AmgSymMatrix<DIM>* covariance = 0);
  
      /** Constructor with local arguments - uses global <-> local for parameters */
      ParametersT (const Alg::AmgVector<DIM>& parameters,
                   const S& surface,
                   Alg::AmgSymMatrix<DIM>* covariance = 0);
  
      /** Constructor with mixed arguments 1 - uses global <-> local for parameters */
      ParametersT (const Alg::Point3D& position,
                   double phi, double theta, double qop,
                   const S& surface,
                   Alg::AmgSymMatrix<DIM>* covariance = 0);
      
      /** Constructor with mixed arguments 2 - uses global <-> local for parameters */
      ParametersT (double loc1, double loc2,
                   const Alg::Vector3D& momentum,
                   double charge,
                   const S& surface,
                   Alg::AmgSymMatrix<DIM>* covariance = 0);
          
      /** Constructor with global arguments - uses global <-> local for parameters */
      ParametersT (const Alg::Point3D& position,
                   const Alg::Vector3D& momentum,
  				   double charge,
  				   const S& surface,
  				   Alg::AmgSymMatrix<DIM>* covariance = 0);
  
  	  /** Copy constructor */
      ParametersT (const ParametersT& rhs);
      
      /** Assignment operator */
      ParametersT& operator= (const ParametersT& rhs);
      
      //**Destructor*/
      virtual ~ParametersT ();
      
      /** Virtual constructor. Allows copying of vector of ParametersT*/
      virtual ParametersT* clone() const { return new ParametersT<DIM,T,S>(*this); }
  
      /** Access to the Surface method */
      virtual const S& associatedSurface() const {return (*m_surface.get());}
  
      /** Return the ParametersType enum */
      virtual ParametersType type() const { return AtaSurface; }

    private:
	  /** --- PRIVATE CONSTRUCTOR : for persistency purpose only */
      ParametersT (const Alg::AmgVector<DIM>& parameters,
                   const S* surface,
                   Alg::AmgSymMatrix<DIM>* covariance = 0);

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
  
     // mutable const S*                m_surface;          //!< surface template
      mutable std::shared_ptr<const S> m_surface;
      
          
  };

} //end of namespace Trk

#include "TrkParametersBase/ParametersT.ipp.h"
#endif // TRKTRACKPARAMETERS_H

