MagneticField package
===

Information about the magnetic field implementation in FCCSW and its usage.

##Contents
* [Overview](#overview)
* [ConstantBFieldSvc](#constBFieldSvc)
* [InterpolatedBFieldSvc](#interpolatedBFieldSvc)
* [BFieldWriter](#bFieldWriter)
* [Example](#example)
* [Magnetic field usage in Geant4 simulation](#geant4)

##Overview
For the magnetic field implementation in FCCSW a service interface `IBFieldSvc` was created which allows to access the magntic field value at a given position. The magnetic field implementation in FCCSW internally uses the [magnetic field implementations of ACTS](http://acts.web.cern.ch/ACTS/latest/doc/group__MagneticField.html). Two implementations of the `IBFieldSvc` exist:

* [ConstantbFieldSvc](#constBFieldSvc)
* [InterpolatedBFieldSvc](#interpolatedBFieldSvc)
* [Magnetic field usage in Geant4 simulation](#bFieldSim)

##ConstantBFieldSvc
The `ConstantBFieldSvc` is a wrapper of the [Acts::ConstantBField in Acts](http://acts.web.cern.ch/ACTS/latest/doc/classActs_1_1ConstantBField.html). It describes a constant magnetic field, returning the same magnetic field value for any position. The magnetic field value can be set by the job options. 

##InterpolatedBFieldSvc
The `InterpolatedBFieldSvc` is a wrapper of the [Acts::InterpolatedBfieldMap](http://acts.web.cern.ch/ACTS/latest/doc/classActs_1_1InterpolatedBFieldMap.html). It interpolates (using a linear approach) the magnetic field value from field values given in a magnetic field map file.

The field map file can either be given in 'root' or in 'txt/csv' file format, in cylinder coordinates. 
The current FCChh magnetic field maps can be found in `/eos/experiment/fcc/hh/simulation/MagneticField`.

##BFieldWriter
In order to write out the magnetic field for validation & visualization a service `BFieldWriter` was implemented. One needs to hand over the magnetic field service following the `IBFieldSvc` interface and the geometric boundaries in which the magnetic field should be printed out. The magnetic field will be printed onto a `root` file in `x`,`y`,`z`,`r` coordinates.

##Example 

An example to test the magnetic field service was implemented (`Examples/options/bFieldTest.py`). It sets up the magnetic field service and prints the magnetic field into a `root` `TTree` using the `BFieldWriter`.
The example can be run typing

~~~{.sh}
./run fccrun.py Examples/options/bFieldTest.py
~~~

in the command line.

To display the magnetic field maps please open a `root` session and type (replacing YOUR_FIELD_FILE.root with the name you chose as the output of `BFieldWriter`):

~~~{.sh}
.L rootMacros/printBField.cpp
printBField("YOUR_FIELD_FILE.root")
~~~

##Magnetic field usage in Geant4 simulation

In order to use the FCC magnetic field service in Geant4 simulation a wrapper (`bFieldG4.h` in `Sim/SimG4Common`), inheriting from `G4MagneticField`, which internally calls a member following the `IBFieldSvc` interface was created. However the class the user will interact with and hand over to the `SimG4Svc` is the configurable `SimG4BFieldTool` (in `Sim/SimG4Components`). This tool sets up the magnetic field in Geant4 with a given magnetic field service (following the `IBFieldSvc` interface).  