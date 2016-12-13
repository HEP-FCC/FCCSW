

Visualization using DDEve in FCCSW
===================================


[DDEve][1] is a DD4hep package that wraps around the Root package [Eve][2], used by several HEP experiments.
FCCSW uses DD4hep for its geometry description, so the detector geometries can be visualized using DD4hep tools, e.g. the standalone executable `geoDisplay`.
Many visualization settings, such as visibility, color, ... can be set in the xml configuration files of the detectors, found in the `Detector` subfolder of FCCSW.
Eve also provides functionalities that enhance the view and help with visual debugging.

A configuration xml file for DDEve, like the default one provided in the visualization package under `Visualization/compact/DDEve_FCCSW.xml`, can be specified on the command line (it defaults to the one just mentioned).
Here is a short list of the views and some configuration variables:

* View3D
    - 3D model of the Detector (as in `geoDisplay`) with clusters superimposed as points. The points can be configured via the `PointSet` options of each `collection`    
* RhoPhiProjection
    * the content of View3D is projected along z
* RhoZProjection
    * the content of View3D is projected along phi
* Calo3DProjection
    * 3D Histogram of the energy deposits, projected on a cylinder surrounding the detector
* Calo2DProjection
    * 2D version of Calo3DProjection, projected along z
* CaloLego
    * Lego-type version of Calo3DProjection
* MultiView
    * composite view of Calo3DProjection, RhoPhiProjection and RhoZProjection


There is currently no support for displaying truth information about particle tracks or any other kind of event data than clusters.
Clusters are represented in Eve in two ways: The first is a pointset, which indicates the position of each cluster as a point.
The relevant options for the points are `color` , `size` and `type`. Their values can be any valid option for [TAttMarker](https://root.cern.ch/doc/master/classTAttMarker.html).
Clusters can also be represented as a histogram in eta and phi. The relevant options are
* `n_eta`, `n_phi`: number of bins in eta/phi
* `eta_min`, `phi_min`: smallest value for eta/phi
* `eta_max`, `phi_max`: largest value for eta/phi

In addition, the Calo3DProjection and derived views use the following configurables:
* `rmin`: radius of the wireframe projection cylinder
* `dz`: length of the wireframe projection cylinder
* `towerH`: outer radius of the histogram bars
* `emax`: maximum energy deposit displayed


All the capabilities and limitations of DDEve can be seen in the CliC example distributed with DD4hep (`DD4hep/examples/CLICSiD/eve` in the DD4hep source directory). 
DDEve is still under development, and the integration in FCCSW is very rudimentary, so feedback regarding needed features and bugs is highly appreciated by the FCCSW- and DD4hep developers.

The viewer can be started using the `eventDisplay.py` script, e.g. `./run python Visualization/python/eventDisplay.py`.
Information about command line options is displayed  when the last command is invoked with the `-h` flag.
The root file containing the event data can also be specified on the command line (defaults to the FCCSW default `output.root`).






[1]: http://frankm.web.cern.ch/frankm/DD4hep/DDEveManual.pdf
[2]: https://root.cern.ch/eve
