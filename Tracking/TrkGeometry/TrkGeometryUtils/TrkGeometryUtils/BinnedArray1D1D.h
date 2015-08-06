///////////////////////////////////////////////////////////////////
// BinnedArray1D1D.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNEDARRAY1D1D_H
#define TRKDETDESCRUTILS_BINNEDARRAY1D1D_H

#include "TrkGeometryUtils/BinnedArray.h"
#include "TrkGeometryUtils/BinUtility.h"

//STL
#include <vector>

class MsgStream;

namespace Trk {

/** @class BinnedArray1D1D

    2D dimensional binned array, where the binning grid is
    not symmetric.
    One steering bin utility finds the associated array of the other.

    @author Andreas.Salzburger@cern.ch
 */

  template <class T> class BinnedArray1D1D : public BinnedArray<T> {

    public:
      /** Default Constructor - needed for inherited classes */
      BinnedArray1D1D() throw (GaudiException)
      : BinnedArray<T>(),
        m_array(0),
        m_arrayObjects(0),
        m_steeringBinUtility(0),
        m_singleBinUtilities(0)
      {}

      /** Constructor with std::vector and a  BinUtility - reference counted, will delete objects at the end,
          if this deletion should be turned off, the boolean deletion should be switched to false
          the global position is given by object! */
      BinnedArray1D1D(const std::vector< std::pair< std::shared_ptr<const T>, Alg::Vector3D > >& tclassvector,
                     BinUtility* steeringBinGen1D,
                     std::vector<BinUtility*>* singleBinGen) throw (GaudiException)
      : BinnedArray<T>(),
        m_array(0),
        m_arrayObjects(0),
        m_steeringBinUtility(steeringBinGen1D),
        m_singleBinUtilities(singleBinGen)
      {

	// prepare the binned Array
	if (steeringBinGen1D) {
	  m_array = new std::vector< std::vector< std::shared_ptr<const T> >* >(steeringBinGen1D->bins(),0);
	  for (size_t i = 0; i < steeringBinGen1D->bins(); ++i) {
	    size_t sizeOfSubBin = ((*m_singleBinUtilities)[i])->bins();
	    (*m_array)[i] = new std::vector< std::shared_ptr<const T> >(sizeOfSubBin);
	  }
	  
	  // fill the Volume vector into the array
	  int vecsize(tclassvector.size());
	  for (int ivec = 0; ivec < vecsize; ++ivec) {
	    const Alg::Vector3D currentGlobal((tclassvector[ivec]).second);
	    if (steeringBinGen1D->inside(currentGlobal)) {
	      int steeringBin                 = steeringBinGen1D->bin(currentGlobal, 0);
	      int singleBin                   = ((*m_singleBinUtilities)[steeringBin])->bin(currentGlobal,0);
	      std::vector< std::shared_ptr<const T> >* curVec = (*m_array)[steeringBin];
	      (*curVec)[singleBin] = ((tclassvector)[ivec]).first;
	    } else
	      throw GaudiException("BinnedArray1D1D", "Object outside bounds", StatusCode::FAILURE);
	      }
	    }
      }

      /** Copy Constructor - copies only pointers! */
      BinnedArray1D1D(const BinnedArray1D1D& barr) throw (GaudiException)
      : BinnedArray<T>(),
        m_array(0),
        m_arrayObjects(0),
        m_steeringBinUtility(0),
        m_singleBinUtilities(0)
      {
          // prepare the binUtilities
          m_steeringBinUtility = (barr.m_steeringBinUtility) ? barr.m_steeringBinUtility->clone() : 0;
          m_singleBinUtilities = new std::vector<BinUtility*>;
          std::vector<BinUtility*>::iterator singleBinIter = barr.m_singleBinUtilities->begin();
          for ( ; singleBinIter != barr.m_singleBinUtilities->end(); ++singleBinIter )
              m_singleBinUtilities->push_back((*singleBinIter)->clone());

          // prepare the binned Array
          if (m_steeringBinUtility && m_singleBinUtilities->size()) {
              // prepare the array
              m_array = new std::vector< std::vector< std::shared_ptr<const T> >* >(m_steeringBinUtility->bins(0));
              for (size_t i=0; i < m_steeringBinUtility->bins(0); ++i) {
                  size_t sizeOfSubBin = ((*m_singleBinUtilities)[i])->bins(0);
                  (*m_array)[i] = new std::vector< std::shared_ptr<const T> >(sizeOfSubBin,0);
              }

              // assign the items
              for (size_t isteer=0; isteer<m_steeringBinUtility->bins(0); ++isteer)
                  for (size_t isingle =0; isingle<(*m_singleBinUtilities)[isteer]->bins(0); ++isteer)
                      (*((*m_array)[isteer]))[isingle] = (*((*barr.m_array)[isteer]))[isingle];

          }
      }

      /** Assignment operator */
      BinnedArray1D1D& operator=(const BinnedArray1D1D& barr)
      {
          if (this != &barr) {

              int arrsize = m_array->size();
              // first deleting everything
              for ( int ivec=0 ; ivec < arrsize; ++ivec) delete (*m_array)[ivec];
              delete m_array;
              delete m_arrayObjects; m_arrayObjects = 0;
              delete m_steeringBinUtility;

              std::vector<BinUtility*>::iterator singleBinIter = m_singleBinUtilities->begin();
              for ( ; singleBinIter != m_singleBinUtilities->end(); ++singleBinIter )
                  delete *singleBinIter;
              delete m_singleBinUtilities; m_singleBinUtilities = new std::vector<BinUtility*>;

              // now assign the stuff
              m_steeringBinUtility = (barr.m_steeringBinUtility) ? (barr.m_steeringBinUtility)->clone() : 0;

              singleBinIter = barr.m_singleBinUtilties->begin();
              for ( ; singleBinIter != barr.m_singleBinUtilities->end(); ++singleBinIter )
                  m_singleBinUtilities->push_back((*singleBinIter)->clone());

              // prepare the binned Array
              if (m_steeringBinUtility && m_singleBinUtilities->size()) {
                  // prepare the array
                  m_array = new std::vector< std::vector< std::shared_ptr<const T> >* >(m_steeringBinUtility->bins(0));
                  for (int i=0; i < m_steeringBinUtility->bins(0); ++i) {
                      unsigned int sizeOfSubBin = ((*m_singleBinUtilities)[i])->bins(0);
                      (*m_array)[i] = new std::vector< std::shared_ptr<const T> >(sizeOfSubBin);
                  }

                  // assign the items
                  for (int isteer=0; isteer<m_steeringBinUtility->bins(0); ++isteer)
                      for (int isingle =0; isingle<(*m_singleBinUtilities)[isteer]->bins(0); ++isteer)
                          (*((*m_array)[isteer]))[isingle] = (*((*barr.m_array)[isteer]))[isingle];
              }
          }
          return *this;
      }

      /** Implicit Constructor */
      BinnedArray1D1D* clone() const
      { return new BinnedArray1D1D(*this); }

      /** Virtual Destructor */
      virtual ~BinnedArray1D1D()
      {
          int arrsize = m_array->size();
          for ( int ivec=0 ; ivec < arrsize; ++ivec) delete (*m_array)[ivec];
          delete m_array;
          delete m_arrayObjects;
          delete m_steeringBinUtility;
          if (m_singleBinUtilities) {
              std::vector<BinUtility*>::iterator binIter = m_singleBinUtilities->begin();
              for ( ; binIter != m_singleBinUtilities->end(); ++binIter)
                  delete *binIter;
          }
          delete m_singleBinUtilities;

      }

      /** Returns the pointer to the templated class object from the BinnedArray,
          it returns 0 if not defined
       */
      const T* object(const Alg::Vector2D& lp) const
      {
          int steerBin  = m_steeringBinUtility->bin(lp, 0);
          int singleBin = (*m_singleBinUtilities)[steerBin]->bin(lp,0);
          return ((*((*m_array)[steerBin]))[singleBin]).getPtr();
      }

      /** Returns the pointer to the templated class object from the BinnedArray,
          it returns 0 if not defined
       */
      const T* object(const Alg::Vector3D& gp) const
      {
          int steerBin  = m_steeringBinUtility->bin(gp, 0);
          int singleBin = (*m_singleBinUtilities)[steerBin]->bin(gp, 0);
          return ((*((*m_array)[steerBin]))[singleBin]).getPtr();
      }

      /** Returns the pointer to the templated class object from the BinnedArray - entry point */
      const T* entryObject(const Alg::Vector3D& gp) const
      { 
          int steerBin  = m_steeringBinUtility->entry(gp, 0);
          int singleBin = (*m_singleBinUtilities)[steerBin]->entry(gp,0);
          return ((*((*m_array)[steerBin]))[singleBin]).getPtr();
            
      }

      /** Returns the pointer to the templated class object from the BinnedArray */
      const T* nextObject(const Alg::Vector3D&, const Alg::Vector3D&, bool) const
      { return 0; }

      /** Return all objects of the Array */
      const std::vector< const T* >& arrayObjects() const {
	if (!m_arrayObjects){
	  m_arrayObjects = new std::vector<const T*>;
	  for (size_t isteer=0; isteer<m_steeringBinUtility->bins(); ++isteer)
	    {
	      for (size_t isingle = 0; isingle< (*m_singleBinUtilities)[isteer]->bins(); ++isingle)
		{ m_arrayObjects->push_back(((*((*m_array)[isteer]))[isingle]).getPtr()); }
	    }
	}
	return (*m_arrayObjects);
      }

      /** Number of Entries in the Array */
      unsigned int arrayObjectsNumber() const { return arrayObjects().size(); }

      /** Return the BinUtility - returns the steering binUtility in this case*/
      const BinUtility* binUtility() const { return (m_steeringBinUtility); }

    private:
      std::vector< std::vector< std::shared_ptr<const T> >* >*     m_array;               //!< vector of pointers to the class T
      mutable std::vector< const T* >*                          m_arrayObjects;        //!< forced 1D vector of pointers to class T
      BinUtility*                                               m_steeringBinUtility;  //!< binUtility for retrieving and filling the Array
      std::vector<BinUtility*>*                                 m_singleBinUtilities;  //!< single bin utilities
  };


} // end of namespace Trk

#endif // TRKSURFACES_BINNEDARRAY1D1D_H

