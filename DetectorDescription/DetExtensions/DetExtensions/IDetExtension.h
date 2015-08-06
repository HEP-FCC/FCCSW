//
//  IExtension.h
//  
//
//  Created by Julia Hrdinka on 12/12/14.
//
//

#ifndef DET_IDETEXTENSION_H
#define DET_IDETEXTENSION_H


namespace Det {
    
    enum ExtensionType {
        
        None           = 0,
        CylinderVolume = 1,
        DiscVolume     = 2,
        CylinderLayer  = 3,
        DiscLayer      = 4,
        Module         = 5,
        SensComponent  = 6
    };
    
    class IDetExtension {
    
    public:
        
        virtual ~IDetExtension()
        {}
        virtual ExtensionType type() = 0;
        
    protected:
        
        IDetExtension()
        {}
    };
}

#endif //DET_IDETEXTENSION_H
