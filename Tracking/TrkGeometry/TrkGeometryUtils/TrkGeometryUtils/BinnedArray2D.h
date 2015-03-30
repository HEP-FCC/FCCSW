///////////////////////////////////////////////////////////////////
// BinnedArray2D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY2D_H
#define TRKDETDESCRUTILS_BINNEDARRAY2D_H

#include "TrkGeometryUtils/BinnedArray.h"
#include "TrkGeometryUtils/BinUtility.h"
//#include "TrkDetDescrUtils/SharedObject.h"
#include "GaudiKernel/GaudiException.h"

//STL
#include <vector>
//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class MsgStream;

namespace Trk {

/** @class BinnedArray2D

   Avoiding a map search, the templated BinnedArray class can help
   ordereing geometrical objects by providing a dedicated BinUtility.

   dedicated for 2-dim aequidistant binning

   @author Andreas.Salzburger@cern.ch
   */

  template <class T> class BinnedArray2D : public BinnedArray<T> {

    public:
     /**Default Constructor - needed for inherited classes */
     BinnedArray2D() throw (GaudiException) :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(0) 
     {}

     /**Constructor with std::vector and a BinUtility - reference counted, will delete objects at the end,
        if this deletion should be turned off, the boolean deletion should be switched to false
        the global position is given by object! */
     BinnedArray2D(const std::vector< std::pair< std::shared_ptr<const T>, Alg::Point3D > >& tclassvector, BinUtility* bingen)  throw (GaudiException) :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(bingen) 
      {
        if (bingen){
          m_array = new std::vector< std::vector< std::shared_ptr<const T> >* >(bingen->bins(1));
          for (size_t i=0; i < bingen->bins(1); ++i)
              (*m_array)[i] = new std::vector< std::shared_ptr<const T> >(bingen->bins(0));

          // fill the Volume vector into the array
          size_t vecsize = tclassvector.size();
          for (size_t ivec = 0; ivec < vecsize; ++ivec) {
            const Alg::Point3D currentGlobal(((tclassvector[ivec]).second));
            if (bingen->inside(currentGlobal)) {
              std::vector< std::shared_ptr<const T> >* curVec = (*m_array)[bingen->bin(currentGlobal,1)];
              (*curVec)[bingen->bin(currentGlobal,0)] = ((tclassvector)[ivec]).first;
            } else {
                 throw GaudiException("BinnedArray2D", "Object outside bounds", StatusCode::FAILURE);
              }
          }
        }
      }

     /**Copy Constructor - copies only pointers !*/
     BinnedArray2D(const BinnedArray2D& barr) throw (GaudiException) :
      BinnedArray<T>(),
      m_array(0),
      m_arrayObjects(0),
      m_binUtility(0) 
      {
          m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;
          // copy over
          m_array = new std::vector< std::vector< std::shared_ptr<const T> > *>(barr.m_array->size());
          for (size_t ihl=0; ihl< barr.m_array->size(); ++ihl){
            (*m_array)[ihl] = new std::vector< std::shared_ptr<const T> >(((*barr.m_array)[0])->size());
            for (size_t ill =0; ill< ((*barr.m_array)[0])->size(); ++ill) 
                (*((*m_array)[ihl]))[ill] = (*((*barr.m_array)[ihl]))[ill];
          }
      }
     /**Assignment operator*/
     BinnedArray2D& operator=(const BinnedArray2D& barr)
      {
        if (this != &barr){
          size_t arrsize = m_array->size();
          // first deleting everything
          for ( size_t ivec=0 ; ivec < arrsize; ++ivec) 
              delete (*m_array)[ivec];
          delete m_array;
          delete m_arrayObjects; m_arrayObjects = 0;
          delete m_binUtility;
          // now refill
          m_binUtility = (barr.m_binUtility) ? barr.m_binUtility->clone() : 0;
          // assign over
          m_array = new std::vector< std::vector< std::shared_ptr<const T> >*>(barr.m_array->size());
          for (size_t ihl=0; ihl< barr.m_array->size(); ++ihl){
            (*m_array)[ihl] = new std::vector< std::shared_ptr<const T> >(((*barr.m_array)[0])->size());
            for (size_t ill=0; ill< ((*barr.m_array)[0])->size(); ++ill) 
                (*((*m_array)[ihl]))[ill] = (*((*barr.m_array)[ihl]))[ill];
          }

        }
        return *this;
      }
     /** Implizit Constructor */
     BinnedArray2D* clone() const
     { return new BinnedArray2D(*this); }

     /**Virtual Destructor*/
     ~BinnedArray2D()
      {
        // these are SharedObjects - memory management internally handled  
        for ( size_t ivec=0 ; ivec < m_array->size(); ++ivec) 
            delete (*m_array)[ivec];
        delete m_array;
        delete m_arrayObjects;
        delete m_binUtility;
      }

     /** Returns the pointer to the templated class object from the BinnedArray,
         it returns 0 if not defined;
      */
     const T* object(const Alg::Point2D& lp) const
     {
      if (m_binUtility->inside(lp)) 
          return ((*((*m_array)[m_binUtility->bin(lp, 1)]))[m_binUtility->bin(lp, 0)]).get();
      return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray
         it returns 0 if not defined;
      */
     const T* object(const Alg::Point3D& gp) const
     {
       if (m_binUtility->inside(gp))
           return ((*((*m_array)[m_binUtility->bin(gp, 1)]))[m_binUtility->bin(gp, 0)]).get();
      return 0;
     }

     /** Returns the pointer to the templated class object from the BinnedArray - entry point*/
     const T* entryObject(const Alg::Point3D& pos) const
     { 
         return ((*((*m_array)[m_binUtility->entry(pos, 1)]))[m_binUtility->entry(pos, 0)]).get();
     }
     
     /** Returns the pointer to the templated class object from the BinnedArray
      */
     const T* nextObject(const Alg::Point3D& gp, const Alg::Vector3D& mom, bool associatedResult=true) const
     {
      // direct access to associated one
      if (associatedResult) return object(gp);
      size_t nextFirst  = m_binUtility->next(gp, mom, 0);
      size_t nextSecond = m_binUtility->next(gp, mom, 1);
      return ( nextFirst > 0 && nextSecond > 0 ) ? ((*((*m_array)[nextSecond]))[nextFirst]).get() : 0;
     }

     /** Return all objects of the Array */
     const std::vector< const T* >& arrayObjects() const {
      if (!m_arrayObjects){
        m_arrayObjects = new std::vector<const T*>;
        m_arrayObjects->reserve(arrayObjectsNumber());
          for (size_t ihl=0; ihl< (m_binUtility->bins(1)); ++ihl)
            for (size_t ill=0; ill< (m_binUtility->bins(0)); ++ill)
               m_arrayObjects->push_back( ((*((*m_array)[ihl]))[ill]).get() );
       }
       return (*m_arrayObjects);
     }

     /** Number of Entries in the Array */
     unsigned int arrayObjectsNumber() const { return (m_array->size())*((*m_array)[0]->size()); }

     /** Return the BinUtility*/
     const BinUtility* binUtility() const { return(m_binUtility); }

    private:
     std::vector< std::vector< std::shared_ptr<const T> >* >*     m_array;        //!< vector of pointers to the class T
     mutable std::vector< const T* >*                          m_arrayObjects; //!< forced 1D vector of pointers to class T
     BinUtility*                                               m_binUtility;   //!< binUtility for retrieving and filling the Array
  };


} // end of namespace Trk

#endif // TRKSURFACES_BINNEDARRAY2D_H

