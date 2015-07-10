//
//  ReadoutSegmentation2D.h
//  
//
//  Created by Julia Hrdinka on 24/04/15.
//
//

#ifndef RECO_READOUTSEGMENTATION2D_H
#define RECO_READOUTSEGMENTATION2D_H

#include "RecoGeometry/ReadoutSegmentation.h"
#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class ReadoutSegmentation2D : public ReadoutSegmentation {
    
    public:
        //constructor from BinUtility
        ReadoutSegmentation2D(Trk::BinUtility* binutil);
        //copy constructor
        ReadoutSegmentation2D(const ReadoutSegmentation2D& seg);
        //destructor
        virtual ~ReadoutSegmentation2D();
        //clone method
        virtual ReadoutSegmentation2D* clone() const override;
        //assignmentoperator
        ReadoutSegmentation2D& operator=(const ReadoutSegmentation2D& seg);
        //returns the number of bins
        virtual size_t bins() const override;
        //every position on the surface corresponds to a unique bin of this sensitive surface
        //calculates the unique bin in the local 2D grid of the surface corresponding to this local position locpos
        virtual unsigned long bin(const Alg::Point2D& locpos) const override;
        //returns the corresponding local position to this local bin
        virtual Alg::Point2D binToLocpos(unsigned long bin) const override;
        //returns the local error in x and y in a 2D Point at the local position locpos
        //- width x und width y
        //returns the bin at this position plus the surrounding bins
        virtual const std::vector<unsigned long> compatibleBins(const Alg::Point2D& locpos) const override;
        //width of the bin
        virtual float binwidth(const Alg::Point2D& locpos, size_t ba=0) const override;
        
    private:
        
        Trk::BinUtility* m_binutility;
    };
}

#endif
