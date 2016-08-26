

Visualization using DDEve in FCCSW
===================================


DDEve [1] is a DD4hep package that wraps around the Root package Eve [2], used by several HEP experiments.
FCCSW uses DD4hep for its geometry description, so the detector geometries can be visualized using DD4hep tools, e.g. the standalone executable `geoDisplay`.
Many visualization settings, such as visibility, color, ... can be set in the xml configuration files of the detectors, found in the `Detector` subfolder of FCCSW.
Eve also provides functionalities that enhance the view and help with visual debugging [2].

There is currently some limited support for the event data display capabilities of DDEve (currently only display of Tracker and Calorimeter Clusters as points).
All the capabilities and limitations of DDEve can be seen in the CliC example distributed with DD4hep (`DD4hep/examples/CLICSiD/eve` in the DD4hep source directory). 
DDEve is still under development, and the integration in FCCSW is very rudimentary, so feedback regarding needed features and bugs is highly appreciated by the FCCSW- and DD4hep developers.

The viewer can be started using the `eventDisplay.py` script, e.g. `./run python Visualization/python/eventDisplay.py`.
Information about command line options is displayed  when the last command is invoked with the `-h` flag.
A configuration xml file for DDEve, like the default one provided in the visualization package under `Visualization/compact/DDEve_FCCSW.xml`, can be specified on the command line (it defaults to the one just mentioned).
The root file containing the event data can also be specified on the command line (defaults to the FCCSW default `output.root`).






[1] http://frankm.web.cern.ch/frankm/DD4hep/DDEveManual.pdf
[2] https://root.cern.ch/eve
