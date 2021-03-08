# FCCSW

Common software for all FCC experiments. FCCSW is a collection of components intended to be run in the Gaudi framework.
Because of the modular nature of the software, this repository contains little actual code, which can instead be found in the following places:

* [`key4hep/k4FWCore`](https://github.com/key4hep/k4FWCore): Basic I/O components
* [`key4hep/k4Gen`](https://github.com/hep-fcc/k4gen): Generators and Particle Guns
* [`key4hep/k4SimDelphes`](https://github.com/key4hep/k4SimDelphes): Delphes Fast Sim
* [`key4hep/k4SimGeant4`](https://github.com/key4hep/k4SimGeant4): Geant4 Full Sim
* [`hep-fcc/fccdetectors`](https://github.com/hep-fcc/fccdetectors): DD4hep models of FCC detector geometries for Full Sim
* [`hep-fcc/k4RecCalorimeter`](https://github.com/hep-fcc/k4RecCalorimeter): Calorimeter Reconstruction Code
* [`hep-fcc/dual-readout`](https://github.com/hep-fcc/dual-readout): DD4hep model of the DREAM dual readout calorimeter

FCCSW is part of the [Key4hep](http://cern.ch/key4hep) software collaboration.


## Quickstart

```bash
# setup: works on lxplus/centos7 with cvmfs
source /cvmfs/sw.hsf.org/key4hep/setup.sh

# run pythia generator with edm4hep output
fccrun Examples/options/pythia.py

# full simulation of FCC-hh, single particle
fccrun Examples/options/geant_fullsim.py

# full simulation of FCC-ee IDEA, single particle
fccrun Examples/options/geant_fullsim_fccee_pgun.py
```


## Documentation

Please consult the [FCCSW homepage](http://cern.ch/fccsw) for more links, and the [fcc-tutorials](https://hep-fcc.github.io/fcc-tutorials/) for a gentle introduction to the software. 
The [key4hep documentation](http://cern.ch/key4hep) also contains relevant information.
