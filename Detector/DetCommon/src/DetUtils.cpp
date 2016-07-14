#include "DetCommon/DetUtils.h"

DD4hep::XML::Component det::getNodeByStrAttr(const DD4hep::XML::Handle_t& mother, const std::string& nodeName, const std::string& attrName,
                                             const std::string& attrValue) {
  for (DD4hep::XML::Collection_t xCompColl(mother, nodeName.c_str()); nullptr != xCompColl; ++xCompColl) {
    if (xCompColl.attr<std::string>(attrName.c_str()) == attrValue) {
      return static_cast<DD4hep::XML::Component>(xCompColl);
    }
  }
  // in case there was no xml daughter with matching name
  return DD4hep::XML::Component(nullptr);
}

double det::getAttrValueWithFallback(const DD4hep::XML::Component& node, const std::string& attrName, const double& defaultValue) {
  if (node.hasAttr(attrName.c_str())) {
    return node.attr<double>(attrName.c_str());
  } else {
    return defaultValue;
  }
}
