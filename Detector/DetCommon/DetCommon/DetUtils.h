#ifndef DETECTOR_DETUTILS_H
#define DETECTOR_DETUTILS_H

#include "DD4hep/DetFactoryHelper.h"

namespace det {
DD4hep::XML::Component getNodeByStrAttr(DD4hep::XML::Handle_t mother, const char* nodeName, const char* attrName,
                                        std::string attrValue);

double getDoubleWithDefault(DD4hep::XML::Component node, const char* attrName, double defaultValue);
}
#endif
