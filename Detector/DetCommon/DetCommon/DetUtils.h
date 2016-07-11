#ifndef DETECTOR_DETUTILS_H
#define DETECTOR_DETUTILS_H

#include "DD4hep/DetFactoryHelper.h"

namespace det {
DD4hep::XML::Component getNodeByStrAttr(const DD4hep::XML::Handle_t mother, const char* nodeName, const char* attrName,
                                        const std::string attrValue);

double getDoubleWithDefault(const DD4hep::XML::Component node, const char* attrName, const double defaultValue);
}
#endif
