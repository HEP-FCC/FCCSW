///////////////////////////////////////////////////////////////////
// BinUtility.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_GENERICBINUTILITY1D_H
#define TRKDETDESCRUTILS_GENERICBINUTILITY1D_H

// Gaudi
#include "GaudiKernel/GaudiException.h"
//Eigen
//#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkGeometryUtils/BinningData.h"
#include "TrkGeometryUtils/BinningType.h"

#include <vector>

class MsgStream;

namespace Trk {

/** @class BinUtility
    
    A generic symmetric BinUtility, for fully symmetric binning in terms
    of binning grid and binning type

    - for asymmetric binning in the binning grid use the BinnedArray1D1D 
    - building up a multidimensional BinUtility has to be done with the 
      operator +=
        
    @author Andreas.Salzburger@cern.ch 
  */
   
   class BinUtility {
        
      public:
       /** Constructor for equidistant */
        BinUtility() :
         m_binningData()
       {
           m_binningData.reserve(3);
       }
      
        /** Constructor for equidistant - the substep is for phi binning offsets  */
        BinUtility(size_t bins, float min, float max, BinningOption opt = open, BinningValue value = binR, float sStep = 0.) :
          m_binningData()
        {
            m_binningData.reserve(3);
            std::vector<float> bValues;
            float step = (max-min)/bins;
            for (size_t ib = 0; ib <= bins; ++ib) bValues.push_back(min + ib*step);
            m_binningData.push_back(Trk::BinningData(Trk::equidistant, 
                                                     opt, 
                                                     value,
                                                     bins, 
                                                     min, 
                                                     max, 
                                                     step,
                                                     sStep,
                                                     bValues) );
        }

        /** Constructor for bi-equidistant */
        BinUtility(size_t subbins, float substep, float min, float max, BinningOption opt = open, BinningValue value = binR) :
          m_binningData()
        {
            m_binningData.reserve(3);
            std::vector<float> bValues;
            float step = (max-min)/(subbins+1);
            bValues.push_back(min);
            for (size_t isb = 1; isb <= subbins; ++isb){
                bValues.push_back(min+isb*step-substep);
                bValues.push_back(min+isb*step);
            }
            bValues.push_back(max);
            m_binningData.push_back(Trk::BinningData(Trk::biequidistant, 
                                                 opt,
                                                 value,
                                                 2*subbins+1,
                                                 min, 
                                                 max, 
                                                 step,
                                                 substep, 
                                                 bValues));
        }
	  
    /** Constructor for arbitrary */
    BinUtility(std::vector<float>& bValues, BinningOption opt = closed, BinningValue value = binPhi) :
	  m_binningData()
	{
	  m_binningData.reserve(3);
	  size_t nBins =  opt==0 ? bValues.size()-1 : bValues.size();

	  m_binningData.push_back(Trk::BinningData(Trk::arbitrary, 
						   opt,
						   value,
						   nBins,
						   bValues[0],
						   bValues.back(), 
						   (bValues.back()-bValues[0])/(nBins),
						   0.,
						   bValues ));
	}

	/** Constructor for binH */
	  BinUtility(float phiRef, std::vector<std::pair<int,float> >& bValues) :
	    m_binningData()
        {
            m_binningData.reserve(3);
            m_binningData.push_back(Trk::BinningData(Trk::open,phiRef,bValues));
        }
        
        /** Copy constructor */
        BinUtility(const BinUtility& sbu ) :
         m_binningData(sbu.m_binningData)
       {}
        
        /** Assignment operator Constructor */
        BinUtility& operator=(const BinUtility& sbu ){
            if ( this != &sbu )
                m_binningData = sbu.m_binningData;
            return (*this);
        }
        
        /** Operator++ to make multidimensional BinUtility */
        BinUtility& operator+= ( const BinUtility& gbu) throw (GaudiException) {
            const std::vector<BinningData>& bData = gbu.binningData(); 
            if (m_binningData.size() + bData.size() > 3) 
                throw GaudiException("BinUtility does not support dim > 3", "FATAL", StatusCode::FAILURE);
            m_binningData.insert(m_binningData.end(), bData.begin(), bData.end());
            return (*this);
        }
        
        /** Virtual Destructor */
        virtual ~BinUtility(){}
          
        /** Implizit Constructor */
        virtual BinUtility* clone() const { return new BinUtility(*this); }
        
        /** return the binning data */
        const std::vector<BinningData>& binningData() const { return m_binningData; }
          
        /** Bin from a 3D vector (already in binning frame) */
        size_t bin(const Alg::Point3D& position, size_t ba=0) const throw (GaudiException)
        {
          if (ba >= m_binningData.size()) 
                throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
          size_t bEval = m_binningData[ba].searchGlobal(position);          
          return ( bEval > bins(ba)-1 ? bins(ba)-1 : bEval );     // ST additional protection : DEBUG source
        }
        
        /** Bin from a 3D vector (already in binning frame) */
        size_t entry(const Alg::Point3D& position, size_t ba=0) const throw (GaudiException)
        { if (ba >= m_binningData.size()) 
                throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
          return m_binningData[ba].entry(position);
        }       
        
        
        /** Bin from a 3D vector (already in binning frame) */
        size_t next(const Alg::Point3D& position, const Alg::Vector3D& direction, size_t ba=0) const throw (GaudiException)
        { if (ba >= m_binningData.size()) 
                throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
          return m_binningData[ba].next(position, direction);
        }       

        /** Distance estimate to next bin  */
	    std::pair<size_t,float> distanceToNext(const Alg::Point3D& position, const Alg::Vector3D& direction, size_t ba=0) const throw (GaudiException)
        { if (ba >= m_binningData.size()) 
                throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
          return m_binningData[ba].distanceToNext(position, direction);
        }       
        
        /** Return the oder direciton for fast interlinking */
        LayerOrder orderDirection(const Alg::Point3D& position, const Alg::Vector3D& direction, size_t ba=0) const throw (GaudiException)  {
             if (ba >= m_binningData.size()) 
                    throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
              return m_binningData[ba].orderDirection(position, direction);
        }     
        
        /** Bin from a 2D vector (following local parameters defintitions)
            - USE WITH CARE !!
              You need to check if your local position is actually in the binning frame of the BinUtility
        */
        size_t bin(const Alg::Point2D& lposition, size_t ba=0) const throw (GaudiException) 
        {    
            if (ba >= m_binningData.size()) 
                    throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
              return m_binningData[ba].searchLocal(lposition);
        }
        
        /** Check if bin is inside from Vector3D */
        bool inside(const Alg::Point3D& position ) const {
	        std::vector<BinningData>::const_iterator bdIter = m_binningData.begin();
	        for ( ; bdIter != m_binningData.end(); ++bdIter) {
	          if ( !(*bdIter).inside(position)) return false;
		}
	        return true;
        }
        
        /** Check if bin is inside from Vector3D */
        bool inside(const Alg::Point2D& lposition) const {
            return true;
            std::vector<BinningData>::const_iterator bdIter =  m_binningData.begin();
            for ( ; bdIter != m_binningData.end(); ++bdIter)
              if ( !(*bdIter).inside(lposition)) return false;
            return true;
        }
        
        /** First bin maximal value */
        size_t dimensions() const {
	        return m_binningData.size();
        }
        
        /** First bin maximal value */
        size_t max(size_t ba=0) const {
	        if (ba >= m_binningData.size()) return 0; 
	        return (m_binningData[ba].bins-1);
        }
        
        /** Number of bins */
        size_t bins(size_t ba=0) const {
	        if (ba >= m_binningData.size()) return 0; 
	        return (m_binningData[ba].bins);
        }

        /** The type/value of the binning */
        BinningValue binningValue(size_t ba=0) const throw (GaudiException) {
            if (ba >= m_binningData.size()) 
                throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
            return (m_binningData[ba].binvalue);
         }
 
        /** bin->BinningValue navigation : pos=+-1. edges/ 0. bin center */
        float binPosition( size_t bin, float pos, size_t ba=0 ) const {
            if (ba >= m_binningData.size()) 
                throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE); 
            return (m_binningData[ba].binPosition( bin, pos ));
	}
                  
        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const {
           sl << "BinUtility for " << m_binningData.size() << "-dimensional array:" << endreq;
           std::vector<BinningData>::const_iterator bdIter = m_binningData.begin();
           for (size_t ibd = 0 ; bdIter != m_binningData.end(); ++bdIter, ++ibd ){
               sl << "dimension     : " << ibd << endreq                                << endreq;
               sl << " - type       : " << size_t((*bdIter).type)                       << endreq;
               sl << " - option     : " << size_t((*bdIter).option)                     << endreq;
               sl << " - value      : " << size_t((*bdIter).binvalue)                   << endreq;
               sl << " - bins       : " << (*bdIter).bins                               << endreq;
               sl << " - min/max    : " << (*bdIter).min << " / " << (*bdIter).max      << endreq;
               sl << " - step/sub   : " << (*bdIter).step << " / " << (*bdIter).subStep << endreq;
               sl << " - boundaries : | ";
               std::vector<float>::const_iterator bIter = (*bdIter).boundaries.begin();
               for ( ; bIter != (*bdIter).boundaries.end(); ++bIter )
                   sl << (*bIter) << " | ";
               sl << endreq;
           }
           return sl;
       }
       
       /** Output Method for std::ostream, to be overloaded by child classes */
       std::ostream& dump(std::ostream& sl) const {
            sl << "BinUtility for " << m_binningData.size() << "-dimensional array:" << std::endl;
            std::vector<BinningData>::const_iterator bdIter = m_binningData.begin();
            for (size_t ibd = 0 ; bdIter != m_binningData.end(); ++bdIter, ++ibd ){
                sl << "dimension     : " << ibd                                          << std::endl;
                sl << " - type       : " << size_t((*bdIter).type)                       << std::endl;
                sl << " - option     : " << size_t((*bdIter).option)                     << std::endl;
                sl << " - value      : " << size_t((*bdIter).binvalue)                   << std::endl;
                sl << " - bins       : " << (*bdIter).bins                               << std::endl;
                sl << " - min/max    : " << (*bdIter).min << " / " << (*bdIter).max      << std::endl;
                sl << " - step/sub   : " << (*bdIter).step << " / " << (*bdIter).subStep << std::endl;
                sl << " - boundaries : | ";
                std::vector<float>::const_iterator bIter = (*bdIter).boundaries.begin();
                for ( ; bIter != (*bdIter).boundaries.end(); ++bIter )
                    sl << (*bIter) << " | ";
                sl << std::endl;
            }
            return sl;
       }
       
       float bincenter (int i, size_t ba=0) {
       
           if (ba >= m_binningData.size())
               throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE);
           return (m_binningData[ba].bincenter(i));
       }
       
       float binhigh (int i, size_t ba=0) {
           
           if (ba >= m_binningData.size())
               throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE);
           return (m_binningData[ba].binhigh(i));
       }
       
       float binlow (int i, size_t ba=0) {
           
           if (ba >= m_binningData.size())
               throw GaudiException("BinUtility", "dimension out of bounds", StatusCode::FAILURE);
           return (m_binningData[ba].binlow(i));
       }

     private :
        std::vector<BinningData>    m_binningData;
  };    

/**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
MsgStream& operator << ( MsgStream& sl, const BinUtility& bgen); 
std::ostream& operator << ( std::ostream& sl, const BinUtility& bgen);

} // end of namespace Trk

#endif // TRKDETDESCRUTILS_GENERICBINUTILITY1D_H

