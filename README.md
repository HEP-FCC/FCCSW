# FCCSW

Common software for all FCC experiments. FCCSW is a collection of components intended to be run in the Gaudi framework.
Because of the modular nature of the software, this repository contains little actual code, which can instead be found in the following places:

* [`key4hep/k4FWCore`](https://github.com/key4hep/k4FWCore): Basic I/O components
* [`key4hep/k4Gen`](https://github.com/hep-fcc/k4gen): Generators and Particle Guns
* [`key4hep/k4SimDelphes`](https://github.com/key4hep/k4SimDelphes): Delphes Fast Sim
* [`hep-fcc/k4SimGeant4`](https://github.com/hep-fcc/k4SimGeant4): Geant4 Full Sim
* [`key4hep/k4geo`](https://github.com/key4hep/k4geo): DD4hep models of FCC detector geometries for Full Sim
* [`hep-fcc/k4RecCalorimeter`](https://github.com/hep-fcc/k4RecCalorimeter): Calorimeter Reconstruction Code
* [`key4hep/k4RecTracker`](https://github.com/key4hep/k4RecTracker): Tracker reconstruction code
* [`hep-fcc/dual-readout`](https://github.com/hep-fcc/dual-readout): DD4hep model of the DREAM dual readout calorimeter

FCCSW is a stakeholder of the [Key4hep](http://cern.ch/key4hep) software collaboration.


## Quickstart

```bash
# setup: works on lxplus/alma9 with cvmfs
source /cvmfs/sw.hsf.org/key4hep/setup.sh

# run pythia generator with EDM4hep output
fccrun Examples/options/pythia.py

# full simulation of FCC-hh, single particle
fccrun Examples/options/geant_fullsim_fcchh_main.py

# full simulation of FCC-ee CLD, single particle
fccrun Examples/options/geant_fullsim_fccee_cld_pgun.py
```


## Documentation

Please consult the [FCCSW homepage](http://cern.ch/fccsw) for more links, and
the [fcc-tutorials](https://hep-fcc.github.io/fcc-tutorials/) for a gentle
introduction to the software. The
[key4hep documentation](http://cern.ch/key4hep) also contains relevant
information.

In case of issues there is [FCCSW Forum](https://fccsw-forum.web.cern.ch/).


## Building

After fetching the repository, do
```
source /cvmfs/sw.hsf.org/key4hep/setup.sh
mkdir build install
cd build
cmake -DCMAKE_INSTALL_PREFIX=install ..
make -j4
make install
cd ..
k4_local_repo  # will update all necessary environment variables
```
