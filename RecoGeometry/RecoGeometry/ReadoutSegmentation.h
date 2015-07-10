//
//  ReadoutSegementation.h
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#ifndef RECO_READOUTSEGMENTATION_H
#define RECO_READOUTSEGMENTATION_H

#include "Algebra/AlgPrimitives.h"

namespace Reco {
    
    class ReadoutSegmentation {
    
    public:
        //copy constructor
        ReadoutSegmentation(const ReadoutSegmentation& seg);
        //destructor
        virtual ~ReadoutSegmentation() {}
        //clone method
        virtual ReadoutSegmentation* clone() const = 0;
        //assignment operator
        ReadoutSegmentation& operator=(const ReadoutSegmentation& seg);
        //returns the number of bins
        virtual size_t bins() const = 0;
        //every position on the surface corresponds to a unique bin of this sensitive surface
        //calculates the unique bin in the local 2D grid of the surface corresponding to this local position locpos
        virtual unsigned long bin(const Alg::Point2D& locpos) const = 0;
        //returns the corresponding local position to this local bin
        virtual Alg::Point2D binToLocpos(unsigned long bin) const = 0;
        //returns the local error in x and y in a 2D Point at the local position locpos
        //- width x und width y
        //returns the bin at this position plus the surrounding bins
        virtual const std::vector<unsigned long> compatibleBins(const Alg::Point2D& locpos) const = 0;
        //width of the bin (an stelle eins oder zwei)
        virtual float binwidth(const Alg::Point2D& locpos, size_t ba=0) const = 0;
        
    protected:
        //default constructor
        ReadoutSegmentation() {}
    };
}


#endif //RECO_READOUTSEGMENTATION_H
