///////////////////////////////////////////////////////////////////
// BinUtility.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRUTILS_BINNINGDATA_H
#define TRKDETDESCRUTILS_BINNINGDATA_H

// Gaudi
#include "GaudiKernel/GaudiException.h"
//Algebra
#include "Algebra/AlgPrimitives.h" 
//Tracking
#include "TrkGeometryUtils/BinningType.h"

#include <vector>
#include <utility>

class MsgStream;

namespace Trk {
    
    /** @enum LayerOrder
     This enum is used to declare Layers as previous/next in respect of
     a 1-dimensional binned array */
    enum LayerOrder { previous = -1, next = 1 };
    
    /** @class BinningData
     
     This class holds all the data necessary for the bin calculation
     
     phi has a very particular behaviour:
     - there's the change around +/- PI
     
     @author Andreas.Salzburger @ cern.ch, Sharka.Todorova @ cern.ch
     */
    
    class BinningData {
        public :
        /** holding all the data for binning calculatuion */
        BinningType                         type;
        BinningOption                       option;
        BinningValue                        binvalue;
        size_t                              bins;
        float                               min;
        float                               max;
        float                               step;
        float                               subStep;
        std::vector<float>                  boundaries;
        float                               refphi;    // ref Phi for binH
        std::vector<std::pair<int,float> >  hbounds;   // boundary for binH
        
        /** Constructor */
        BinningData ( BinningType bType,
                     BinningOption bOption,
                     BinningValue bValue,
                     size_t bBins,
                     float  bMin,
                     float  bMax,
                     float  bStep,
                     float  bSubStep = 0,
                     std::vector<float> bBoundaries = std::vector<float>() ) :
        type(bType),
        option(bOption),
        binvalue(bValue),
        bins(bBins),
        min(bMin),
        max(bMax),
        step( bStep!= 0. ? bStep : 1.),
        subStep(bSubStep),
        boundaries(bBoundaries),
        refphi(0.),
        hbounds(std::vector<std::pair<int,float> >()),
        mixPtr( nullptr )
        {
            if (bType == Trk::equidistant) functionPtr = &searchEaquidstantWithBoundary;
            else if (bType == Trk::biequidistant) functionPtr = &searchBiequidistantWithBoundary;
            else functionPtr = bins < 50 ? &searchInVectorWithBoundary : &binarySearchWithBoundary;
        }
        
        /** Constructor for binH type : non-equidistant binning assumed */
        BinningData ( BinningOption bOption,
                     float  bRefPhi,
                     const std::vector< std::pair<int,float> >& bBoundaries) :
        type(Trk::arbitrary),
        option(bOption),
        binvalue(Trk::binH),
        bins(bOption==Trk::open? bBoundaries.size()-1 : bBoundaries.size()),
        min(bBoundaries.front().second),
        max(bBoundaries.back().second),
        step(1.),                        // non-zero value needed for next()
        subStep(0.),
        boundaries(std::vector<float>()),
        refphi(bRefPhi),
        hbounds(bBoundaries),
        functionPtr( nullptr ),
        mixPtr( &searchInVectorWithMixedBoundary )
        {}
        
        /** take the right float value - assumes the correct local position expression */
        float value(const Alg::Point2D& lposition) const {
            // ordered after occurence
            if ( binvalue == Trk::binR || binvalue == Trk::binRPhi || binvalue == Trk::binX || binvalue == Trk::binH ) return lposition.X();
            if ( binvalue == Trk::binPhi ) return gaugePhi(lposition.Y());
            return lposition.Y();
        }
        
        /** take the right float value */
        float value(const Alg::Point3D& position) const {
            // ordered after occurence
            if ( binvalue == Trk::binR || binvalue == Trk::binH ) return (sqrt(position.Perp2()));
            if ( binvalue == Trk::binRPhi ) return  (sqrt(position.Perp2())*position.phi());
            if ( binvalue == Trk::binEta ) return (position.eta());
            if ( binvalue == Trk::binX) return position.X();
            if ( binvalue == Trk::binY) return position.Y();
            if ( binvalue == Trk::binZ) return position.Z();
            // phi gauging
            return gaugePhi(position.phi());
        }
        
        /** gauge phi */
        float gaugePhi(float phi) const {
            if (max > M_PI && phi < min && phi < 0.){
                phi = M_PI+phi;
                phi += M_PI;
            }
            return phi;
        }
        
        /** take float values for binH */
        std::pair<float,float> valueH(const Alg::Point2D& lposition ) const {
            return (std::pair<double,double> (lposition.X(),lposition.X()*cos(fabs(refphi-lposition.Y()))));
        }
        
        /** take float values for binH */
        std::pair<float,float> valueH(const Alg::Point3D& position ) const {
            return (std::pair<double,double> (sqrt(position.Perp2()),sqrt(position.Perp2())*cos(fabs(position.phi()-refphi))));
        }
        
        /** Check if bin is inside from Vector3D */
        bool inside(const Alg::Point3D& position) const {
            // closed one is always inside
            if (option == Trk::closed) return true;
            // all other options except value H
            if ( binvalue != Trk::binH ) {
                float val = value(position);
                return ( val > min-0.001 && val < max+0.001);
            }
            // value H case
            std::pair<double,double> valH = valueH(position);
            float valmin = hbounds.front().first==0 ? valH.first : valH.second;
            float valmax = hbounds.back().first==0 ?  valH.first : valH.second;
            return ( valmin > min-0.001 && valmax < max+0.001);
        }
        
        /** Check if bin is inside from Vector2D */
        bool inside(const Alg::Point2D& lp) const  {
            if (option == Trk::closed) return true;
            if ( binvalue != Trk::binH ) {
                float val = value(lp);
                return ( val > min-0.001 && val < max+0.001);
            }
            std::pair<double,double> valH = valueH(lp);
            float valmin = hbounds.front().first==0 ? valH.first : valH.second;
            float valmax = hbounds.back().first==0 ?  valH.first : valH.second;
            return ( valmin > min-0.001 && valmax < max+0.001);
        }
        
        /** generic search from a 2D position --- corresponds to local coordinate schema */
        size_t searchLocal(const Alg::Point2D& lposition) const {
            return (binvalue==Trk::binH) ? searchH(valueH(lposition)) : search(value(lposition));
        }
        
        /** generic search from a 3D position */
        size_t searchGlobal(const Alg::Point3D& position) const {
            return (binvalue==Trk::binH) ? searchH(valueH(position)) : search(value(position));
        }
        
        /** generic search - forwards to correct function pointer */
        size_t search(float value) const { assert(functionPtr != nullptr); return (*functionPtr)(value, *this); }
        
        /** generic search - forwards to correct function pointer */
        size_t searchH(std::pair<double,double> value) const { assert(mixPtr != nullptr); return (*mixPtr)(value, *this); }
        
        /** the entry bin */
        size_t entry(const Alg::Point3D& position ) const {
            size_t bin = (binvalue==Trk::binH) ? searchH(valueH(position)) : search(value(position));
            return (bin < bins-bin) ? bin : bins-1;
        }
        
        /** the next bin : gives -1 if the next one is outside */
        size_t next(const Alg::Point3D& position, const Alg::Vector3D& dir) const {
            float   val    = value(position);
            Alg::Point3D probe = position+0.5*step*dir.Unit();
            float  nextval = value( probe);
            int bin        = (binvalue==Trk::binH) ? searchH(valueH(position)) : search(val);
            bin = (nextval > val && bin != int(bins-1)) ? bin+1 : (bin) ? bin-1 : 0;
            // closed setup
            if ( option == closed)
                return ( bin < 0 || bin+1 > int(bins) ) ? ( (bin < 0 ) ? bins-1 : 0  ) : bin;
            // open setup
            return bin;
        }
        
        /** distance to the next bin : gives -1 if the next one is outside */
        std::pair<size_t, float>  distanceToNext(const Alg::Point3D& position, const Alg::Vector3D& dir) const {
            // current value
            float   val    = (binvalue==Trk::binH) ? valueH(position).first : value(position);
            // probe value
            Alg::Point3D probe = position+0.5*step*dir.Unit();
            float  nextval = (binvalue==Trk::binH) ? valueH(probe).first : value(probe);
            // current bin
            int bin0        = (binvalue==Trk::binH) ? searchH(valueH(position)) : search(val);
            // next bin
            int bin =  (nextval-val) >0. ? bin0+1 : bin0-1;
            if (bin > int(bins)-1) bin = (option==closed) ? 0 : bin0;
            if (bin < 0) bin = (option==closed) ? bins-1 : 0;
            
            // boundary value
            float bval = 0.;
            if  (binvalue==Trk::binH) {
                bval = (nextval >val) ? hbounds[bin0+1].second : hbounds[bin0].second;    // non-equidistant
                
                // may need to recalculate current value and probe
                if (nextval>val) {
                    if ( hbounds[bin0+1].first>0 ) {
                        val = valueH(position).second;
                        nextval = valueH(probe).second;
                    }
                } else {
                    if ( hbounds[bin0].first>0 ) {
                        val = valueH(position).second;
                        nextval = valueH(probe).second;
                    }
                }
            } else {
                bval = (nextval >val) ? boundaries[bin0+1] : boundaries[bin0];    // non-equidistant
                if (type == Trk::equidistant ) bval = min+bin0*step;
            }
            // differential
            float dd = 2*(nextval-val)/step;
            // distance estimate
            float dist = fabs(dd)>1.e-06 ? (bval-val)/dd : 1.e06 ;
            return std::pair<size_t, float> (bin,dist);
        }
        
        /** layer order is needed for value H binning */
        LayerOrder orderDirection(const Alg::Point3D& position, const Alg::Vector3D& dir) const {
            float     val  = (binvalue==Trk::binH) ? valueH(position).first : value(position);
            Alg::Point3D probe = position+0.5*step*dir.Unit();
            float  nextval = (binvalue==Trk::binH) ? valueH(probe).first : value( probe );
            return (nextval > val ) ? Trk::next : Trk::previous;
        }
        
        /** bin->BinningValue navigation : pos=+-1. edges/ 0. bin center */
        float binPosition( size_t bin, float pos ) const {
            
            if (type == Trk::equidistant ) return ( min+(2.*bin+pos+1.)*step/2.) ;
            
            float bmin = (binvalue==Trk::binH) ? hbounds[bin].second : boundaries[bin] ;
            float bmax = (binvalue==Trk::binH) ? hbounds[bin+1].second :
            bin+1<boundaries.size() ? boundaries[bin+1] : boundaries[bin]+step  ;
            
            return ( bmin + 0.5*(pos+1.)*(bmax-bmin) );
            
        }
        //added
        float bincenter(int i) {
            
            return (0.5*(binlow(i)+binhigh(i)));
        }
        
        float binlow(int i) {
            
            return (boundaries[i]);
        }
        
        float binhigh(int i) {
            
            return (boundaries[i+1]);
        }
        
        
    private:
        /** the pointer to the function to be used */
        size_t (*functionPtr) (float, const BinningData&);
        size_t (*mixPtr) (std::pair<float,float>, const BinningData&);
        
        /** Equidistant search : equidist 0 */
        static size_t searchEaquidstantWithBoundary(float value, const BinningData& bData) {
            int bin = ((value-bData.min)/bData.step);
            // special treatment of the 0 bin for closed
            if ( bData.option == closed){
                if (value < bData.min) return (bData.bins-1);
                if (value > bData.max) return 0;
            }
            // if outside boundary : return boundary for open, opposite bin for closed
            bin = bin < 0 ? ( ( bData.option == Trk::open) ? 0 : (bData.bins-1) ) : bin ;
            return size_t((bin <= int(bData.bins-1)) ? bin : ( (bData.option == open) ? (bData.bins-1) : 0 ));
        }
        
        
        /** Biequidistant search : biequidist 1 */
        static size_t searchBiequidistantWithBoundary(float value, const BinningData& bData) {
            // the easy exits (first / last)
            if (value < bData.min ) return ( bData.option==closed ) ? (bData.bins-1) : 0;
            if (value > bData.max ) return ( bData.option==closed ) ? 0 : (bData.bins-1);
            // special treatment for first and last bin
            if (value > bData.max - bData.step ) return bData.bins - 1;
            // decide the leading bin number (low leading bin)
            size_t leadbin = int((value-bData.min)/bData.step);
            float bDist = value - ( bData.min + (leadbin+1) * bData.step );
            int addon = int(bDist/bData.subStep) ? 0 : 1;
            // return the bin
            return leadbin*2 +addon;
        }
        
        
        /** Linear search in vector - superior in O(10) searches: arbitraty 2*/
        static size_t searchInVectorWithBoundary(float value, const BinningData& bData)
        {
            if (bData.binvalue == binPhi) while (value<bData.boundaries[0]) value += 2*M_PI;
            if (bData.binvalue == binPhi) while (value>bData.max) value -= 2*M_PI;
            // lower boundary
            if ( value <= bData.boundaries[0] ) {
                return ( bData.option==closed ) ? (bData.bins-1) : 0;
            }
            // higher boundary
            if ( value >= bData.max ) return ( bData.option==closed ) ? 0 : (bData.bins-1);
            // search
            std::vector<float>::const_iterator vIter = bData.boundaries.begin();
            size_t bin = 0;
            for ( ; vIter !=  bData.boundaries.end() ;  ++vIter, ++bin )
                if ((*vIter) > value) break;
            return (bin-1);
        }
        
        /** A binary search with underflow/overflow - faster than vector search for O(50) objects*/
        static size_t binarySearchWithBoundary(float value, const BinningData& bData)
        {
            // Binary search in an array of n values to locate value
            if (bData.binvalue == binPhi) while (value<bData.boundaries[0]) value+= 2*acos(-1.);
            if (bData.binvalue == binPhi) while (value>bData.max) value-= 2*acos(-1.);
            // underflow
            if ( value <= bData.boundaries[0] ) return ( bData.option==closed ) ? (bData.bins-1) : 0;
            size_t nabove, nbelow, middle;
            // overflow
            nabove = bData.boundaries.size()+1;
            if ( value >=  bData.max) return ( bData.option==closed ) ? 0 : nabove-2;
            // binary search
            nbelow = 0;
            while(nabove-nbelow > 1) {
                middle = (nabove+nbelow)/2;
                if (value == bData.boundaries[middle-1]) return middle-1;
                if (value  < bData.boundaries[middle-1]) nabove = middle;
                else nbelow = middle;
            }
            return nbelow-1;
        }
        
        /** Search in mixed vector - linear in O-10 bins, otherwise binary */
        static size_t searchInVectorWithMixedBoundary(std::pair<float,float> val, const BinningData& bData)
        {
            if ( (bData.hbounds[0].first==0 ? val.first:val.second) < bData.hbounds[0].second ) return ( bData.option==closed ) ? (bData.bins-1) : 0;
            if ( (bData.hbounds.back().first==0 ? val.first:val.second)  >= bData.max ) return ( bData.option==closed ) ? 0 : (bData.bins-1);
            
            if ( bData.hbounds.size()<10 ) {
                std::vector< std::pair<int, float> >::const_iterator vBeg = bData.hbounds.begin();
                std::vector< std::pair<int, float> >::const_iterator vIter = vBeg+1;
                for (  ; vIter !=  bData.hbounds.end() ;  ++vIter )
                    if ((*vIter).second > ((*vIter).first==0 ? val.first:val.second) ) break; 
                return ( vIter!= bData.hbounds.end() ? vIter-vBeg-1 : bData.bins-1 ) ; 
            }
            
            // Binary search in an array of n values to locate value 
            size_t nabove, nbelow, middle;
            nabove = bData.hbounds.size();
            // binary search
            nbelow = 0;
            while(nabove-nbelow > 1) {
                middle = (nabove+nbelow)/2;
                float valm = bData.hbounds[middle].first==0 ? val.first:val.second;
                if (valm == bData.hbounds[middle].second) { nbelow = middle; break;}
                if (valm  < bData.hbounds[middle].second) nabove = middle;
                else nbelow = middle;
            }
            
            if (nbelow > bData.bins-1 ) return bData.bins-1; 
            return nbelow;
            
        }
        
    };
    
}

#endif
