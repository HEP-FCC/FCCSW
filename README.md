# FCC Software

High level repository for the common software of the Future Circular Collider.
Formerly it was a collection of components intended to be run in the Gaudi
framework. These days this repository contains little actual code, which can
instead be found in the following places:

* [`key4hep/k4FWCore`](https://github.com/key4hep/k4FWCore): Basic I/O components
* [`key4hep/k4Gen`](https://github.com/key4hep/k4gen): Generators and Particle Guns
* [`key4hep/k4SimDelphes`](https://github.com/key4hep/k4SimDelphes): Delphes Fast Sim
* [`key4hep/k4SimGeant4`](https://github.com/key4hep/k4SimGeant4): Geant4 Full Sim
* [`key4hep/k4geo`](https://github.com/key4hep/k4geo): DD4hep models of FCC-ee detector geometries for Full Sim
* [`hep-fcc/fccdetectors`](https://github.com/hep-fcc/fccdetectors): DD4hep models of FCC-hh detector geometries for Full Sim
* [`hep-fcc/k4RecCalorimeter`](https://github.com/hep-fcc/k4RecCalorimeter): Calorimeter Reconstruction Code
* [`key4hep/k4RecTracker`](https://github.com/key4hep/k4RecTracker): Tracker reconstruction code

FCCSW is a stakeholder of the [Key4hep](http://cern.ch/key4hep) software
ecosystem.


## Quickstart

```bash
# setup: works on LXPLUS or any Alma9/RHEL compatible OS with CVMFS
source /cvmfs/sw.hsf.org/key4hep/setup.sh
# or
source /cvmfs/fcc.cern.ch/sw/latest/setup.sh
# ^ currently a simple alias of the Key4hep setup script

# Run Pythia generator with EDM4hep output
k4run Examples/options/pythia.py

# Full simulation of FCC-hh, single particle
k4run Examples/options/geant_fullsim_fcchh_main.py

# Full simulation of FCC-ee CLD, single particle
k4run Examples/options/geant_fullsim_fccee_cld_pgun.py
```


## Documentation

Please consult the [FCCSW homepage](http://cern.ch/fccsw) for more links, and
the [fcc-tutorials](https://hep-fcc.github.io/fcc-tutorials/) for an
introduction to the software. The
[key4hep documentation](http://cern.ch/key4hep) also contains plenty of relevant
information.

In case of issues there is a [FCC Software Forum](https://fccsw-forum.web.cern.ch/).


## Building a Gaudi package

After fetching of the repository, do
```bash
source /cvmfs/sw.hsf.org/key4hep/setup.sh
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=install ..
make -j4
make install
cd ..
k4_local_repo  # will update all necessary environment variables
```
