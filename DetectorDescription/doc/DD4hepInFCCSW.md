DD4hep Detector Description in FCCSW
==

General File Structure
--
To first order two files are involved to describe a detector in DD4hep.
- The xml-based detector description that defines the dimensions and materials of the detector
- The macro that implements the factory method to construct the detector from the xml-description

Visualization
--
DD4hep comes with `geoDisplay` that can be used to display any detector geometry you implement. To use this display in the FCCSW context prepend the command with the `run` script in the FCCSW directory, e.g.:
~~~~~~~~~~~~~~~~~~~~~~~~{.sh}
./run geoDisplay DetectorDescription/Detectors/compact/TestTracker.xml
~~~~~~~~~~~~~~~~~~~~~~~~

N.b.: When running geoDisplay through a X-Session, by default OpenGL may not be used. If you see the warning:
~~~~~~~~~~~~~~~~~~~~~~~~
libGL error: No matching fbConfigs or visuals found
libGL error: failed to load driver: swrast
~~~~~~~~~~~~~~~~~~~~~~~~
You can try to set the following environment variable to try and enable OpenGL:
~~~~~~~~~~~~~~~~~~~~~~~~{.sh}
export LIBGL_ALWAYS_INDIRECT=1
~~~~~~~~~~~~~~~~~~~~~~~~

Constructing Detector Geometry
--
Similarly to Geant4 itself, DD4hep tries to re-use detector elements without having to keep every instance in memory. I.e. many `DD4hep::Geometry::PlacedVolume` are used that refer to one `DD4hep::Geometry::Volume`. To exploit this feature as much as possible:
- Come up with the largest component that is repeatable (e.g. a detector wedge)
  - Usually this component is similar or equal to the components that are built when physically building the detector
- Construct this component once and then place it several times to construct the whole detector
- This should minimize the number of fully unique volumes you create

Converting from Geant4 / GDML
--
A minimal running example may be found *add link*
**Factory method**: The minimal part to define a factory method for a sub-detector is:
~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
static DD4hep::Geometry::Ref_t create_element(
  DD4hep::Geometry::LCDD& lcdd,                 // dd4hep's main detector description interface
  xml_h xmlElement,                             // the xml-tree that describes this detector
  DD4hep::Geometry::SensitiveDetector sensDet   // dd4hep's interface to the sensitive detector
  ) {
  // implementation
}
DECLARE_DETELEMENT(DetName, detector::create_element) // factory method
~~~~~~~~~~~~~~~~~~~~~~~~
**Detector description**: To use this sub-detector implementation the corresponding detector description XML can refer to the defined factory method:
~~~~~~~~~~~~~~~~~~~~~~~~{.xml}
  <detector id="0" name="Name" type="DetName">
    <!-- the variables used here need to be defined inside <define> and <display> tags. -->
    <dimensions rmin="det_rmin" rmax="det_rmax" dz="det_dz" vis="visdef" />
  </detector>
~~~~~~~~~~~~~~~~~~~~~~~~

Coordinate conventions
----------------------
- Position vector points to center of volume
- Sizes are defined as the offset position vector
- Local coordinates start at position vector



