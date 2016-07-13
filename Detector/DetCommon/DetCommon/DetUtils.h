#ifndef DETECTOR_DETUTILS_H
#define DETECTOR_DETUTILS_H

#include "DD4hep/DetFactoryHelper.h"

namespace det {
DD4hep::XML::Component getNodeByStrAttr(const DD4hep::XML::Handle_t& mother, const std::string& nodeName, const std::string& attrName,
                                        const std::string& attrValue);

double getDoubleWithDefault(const DD4hep::XML::Component& node, const std::string& attrName, const double& defaultValue);
}
#endif
