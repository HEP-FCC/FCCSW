

Visualization using DDEve in FCCSW
===================================


DDEve [1] is a DD4hep package that wraps around the Root package Eve [2], used by several HEP experiments.
FCCSW uses DD4hep for its geometry description, so the detector geometries can be visualized using DD4hep tools, e.g. the standalone executable `geoDisplay`.
Many visualization settings, such as visibility, color, ... can be set in the xml configuration files of the detectors, found in the `Detector` subfolder of FCCSW.
Eve also provides functionalities that enhance the view and help with visual debugging [2].

There is currently some limited support for the event data display capabilities of DDEve (currently only display of Tracker and Calorimeter Clusters as points).
For this, the FCCSW output currently needs to be converted into DDG4 format using a program such as <https://github.com/vvolkl/fcc2dd4vis>.
This is temporary -- in the future, there will be a tighter integration with FCCSW.

DDEve can be started using the macro `$DD4hepINSTALL/examples/DDEve/DDEve.C` in your DD4hep installation (the environment variable `DD4hepINSTALL` is required by the macro).
The macro will ask for an xml configuration file describing the detector and event data used for display. An example can be found in this subfolder `Visualization/compact/DDEve_FCCSW.xml`.

All the capabilities and limitations of DDEve can be seen in the CliC example distributed with DD4hep (`DD4hep/examples/CLICSiD/eve` in the DD4hep source directory). 
DDEve is still under development, and the integration in FCCSW is very rudimentary, so feedback regarding needed features and bugs is highly appreciated by the FCCSW- and DD4hep developers.



[1] http://frankm.web.cern.ch/frankm/DD4hep/DDEveManual.pdf
[2] https://root.cern.ch/eve
