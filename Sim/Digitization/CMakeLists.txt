################################################################################
# Package: Digitization
################################################################################
gaudi_subdir(Digitization v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg FWCore Reconstruction/RecInterface Detector/DetInterface Reconstruction/RecTracker)

find_package(DD4hep COMPONENTS DDG4 DDSegmentation DDRec REQUIRED)

find_package(ACTS COMPONENTS Core)
#workaround for the lone -lGeom flag exported by ACTS
link_directories(${ROOT_INCLUDE_DIR}/../lib)
find_package(ROOT COMPONENTS MathCore GenVector Geom Tree REQUIRED)

gaudi_install_headers(Digitization)

gaudi_add_module(DigitizationPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS FWCore RecInterface DetInterface DD4hep RecTracker ROOT
                 LINK_LIBRARIES GaudiAlgLib FWCore ACTS::ACTSCore DD4hep ${DD4hep_COMPONENT_LIBRARIES} ROOT)
