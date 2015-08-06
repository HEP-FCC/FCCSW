///////////////////////////////////////////////////////////////////
// BinnedArray.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY_H
#define TRKDETDESCRUTILS_BINNEDARRAY_H

#include "TrkGeometryUtils/BinUtility.h"
//#include "TrkDetDescrUtils/SharedObject.h"
// GaudiKernel
#include "GaudiKernel/GaudiException.h"
// Eigen
//#include "GeoPrimitives/GeoPrimitives.h"
//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinnedArray

   Binned Array for avoiding map searches/

   @author Andreas.Salzburger@cern.ch
   */

  template <class T> class BinnedArray {

    public:
     /**Default Constructor - needed for inherited classes */
     BinnedArray() throw (GaudiException)
      {}

     /**Virtual Destructor*/
     virtual ~BinnedArray(){}

     /** Implicit constructor */
     virtual BinnedArray* clone() const = 0;

     /** Returns the pointer to the templated class object from the BinnedArray,
         it returns 0 if not defined, takes local position */
     virtual const T* object(const Alg::Point2D& lp) const = 0;

     /** Returns the pointer to the templated class object from the BinnedArray
         it returns 0 if not defined, takes global position */
     virtual const T* object(const Alg::Point3D& gp) const = 0;

     /** Returns the pointer to the templated class object from the BinnedArray - entry point*/
     virtual const T* entryObject(const Alg::Point3D&) const = 0;

     /** Returns the pointer to the templated class object from the BinnedArray, takes 3D position & direction */
     virtual const T* nextObject(const Alg::Point3D& gp, const Alg::Vector3D& mom, bool associatedResult=true) const = 0;

     /** Return all objects of the Array */
     virtual const std::vector< const T* >& arrayObjects() const = 0;

     /** Number of Entries in the Array */
     virtual unsigned int arrayObjectsNumber() const = 0;

     /** Return the BinUtility*/
     virtual const BinUtility* binUtility() const = 0;

  private:

  };


} // end of namespace Trk

#endif // TRKDETDESCRUTILS_BINNEDARRAY_H


