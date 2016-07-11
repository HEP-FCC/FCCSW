#include "DetCommon/DetUtils.h"

/// Given a XML element with several daughters with the same name, e.g.
/// <detector> <layer name="1" /> <layer name="2"> </detector>
/// this method returns the first daughter of type nodeName whose attribute has a given value
/// e.g. returns <layer name="2"/> when called with (detector, "layer", "name", "1")
/// @todo: runtime checks that attribute is present
DD4hep::XML::Component det::getNodeByStrAttr(const DD4hep::XML::Handle_t mother, const char* nodeName, const char* attrName,
                                             const std::string attrValue) {
  for (DD4hep::XML::Collection_t xCompColl(mother, nodeName); nullptr != xCompColl; ++xCompColl) {
    if (xCompColl.attr<std::string>(attrName) == attrValue) {
      return static_cast<DD4hep::XML::Component>(xCompColl);
    }
  }
  // in case there was no xml daughter with matching name
  return DD4hep::XML::Component(nullptr);
}

/// try to get attribute with double value, return defaultValue if attribute not found
double det::getDoubleWithDefault(const DD4hep::XML::Component node, const char* attrName, const double defaultValue) {
  if (node.hasAttr(attrName)) {
    return node.attr<double>(attrName);
  } else {
    return defaultValue;
  }
}
