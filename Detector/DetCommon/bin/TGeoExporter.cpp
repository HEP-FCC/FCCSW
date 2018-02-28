
#include "DD4hep/Detector.h"
#include "TGeoManager.h"

int main(int argc, char *argv[]) {
  std::string filename = argv[1];
  auto lcdd = &(dd4hep::Detector::getInstance());
  lcdd->fromCompact(filename);
  gGeoManager->Export((filename + ".root").c_str());
  
  }
