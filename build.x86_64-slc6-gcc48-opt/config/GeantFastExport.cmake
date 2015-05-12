# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(GeantFast SHARED IMPORTED)
set_target_properties(GeantFast PROPERTIES
  REQUIRED_INCLUDE_DIRS "G4Tree;G4FR;G4GMocren;G4visHepRep;G4RayTracer;G4VRML;G4vis_management;G4modeling;G4interfaces;G4persistency;G4analysis;G4error_propagation;G4readout;G4physicslists;G4run;G4event;G4tracking;G4parmodels;G4processes;G4digits_hits;G4track;G4particles;G4geometry;G4materials;G4graphics_reps;G4intercoms;G4global;G4clhep;G4zlib;G4geomUSolids;GeantFast;Geant4;HepMC;CLHEP;ROOT;Pythia8"
  REQUIRED_LIBRARIES "G4Tree;G4FR;G4GMocren;G4visHepRep;G4RayTracer;G4VRML;G4vis_management;G4modeling;G4interfaces;G4persistency;G4analysis;G4error_propagation;G4readout;G4physicslists;G4run;G4event;G4tracking;G4parmodels;G4processes;G4digits_hits;G4track;G4particles;G4geometry;G4materials;G4graphics_reps;G4intercoms;G4global;G4clhep;G4zlib;G4geomUSolids;${LCG_releases}/HepMC/2.06.08/x86_64-slc6-gcc48-opt/lib/libHepMC.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Cast-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Evaluator-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Exceptions-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-GenericFunctions-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Geometry-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Random-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-RandomObjects-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-RefCount-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Vector-2.1.4.1.so;${LCG_releases}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib/libCLHEP-Matrix-2.1.4.1.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;${LCG_releases}/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/lib/libpythia8.so"
  IMPORTED_SONAME "libGeantFast.so"
  IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/libGeantFast.so"
  )
set(GeantFast_VERSION v1r0)
