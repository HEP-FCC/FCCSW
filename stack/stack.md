---
layout: site
---
FCC Software Stack
================================================


<p><img width="80%" src="https://vavolkl.web.cern.ch/vavolkl/img/fccsw_deps_zoom.png" alt="A simplified diagram showing the interdependencies of HEP/FCC software packages" /></p>

FCC software is an ecosystem of many packages and communities. 
Below is an (almost) exhaustive list
of HEP-specific software packages used in FCC software.
To ensure that all packages are compatible and to reduce the time spent compiling and building we use LCG
releases (<http://lcginfo.cern.ch/>) whenever possible, and integrate as much as possible in the
framework FCCSW.
Many FCC specific packages are tested, built and distributed by the FCC software group. Detailed
informations on version numbers are available in machine-readable format here:
<https://github.com/HEP-FCC/fcc-spi/tree/master/config>


Package | Homepage | Description
 -----         | -----     | -----
 hep-acts-core | <http://acts.web.cern.ch> |  ACTS is a common tracking software for HEP experiments.
 hep-clhep | <https://proj-clhep.web.cern.ch/proj-clhep/> |  CLHEP is a class library for High Energy Physics.
 hep-dag | <https://github.com/HEP-FCC/dag> |  DAG is a C++ library for directed acyclic graphs.
 hep-dd4hep | <https://cern.ch/dd4hep> |  DD4hep is a detector description toolkit for HEP.
 hep-delphes | <https://cp3.irmp.ucl.ac.be/projects/delphes> |  Delphes, a framework for fast simulation of a generic collider experiment.
 hep-fastjet | <http://fastjet.fr/> |  Fastjet, a software package for jet finding in pp and e+e− collisions.
 hep-fcc-physics | <https://github.com/HEP-FCC/fcc-physics> |  Command line tools for physics studies at the FCC.
 hep-fccedm | <https://github.com/HEP-FCC/fcc-edm> |  FCC-edm is the common event data model library for the Future Circular Collider.
 hep-fccedmlegacy | <https://github.com/HEP-FCC/fcc-edm-legacy> |  FCC-EDM datatypes needed for legacy data.
 hep-fcchhanalyses | <https://github.com/FCC-hh-framework/FCChhAnalyses> |  Physics analyses for the FCC-hh and HE-LHC, written in heppy.
 hep-fccrepoconfig | <https://cern.ch/fccsw> |  Metapackage configuring the ubuntu repository for FCC software.
 hep-fccsw-crmc | <https://gitlab.cern.ch/vavolkl/fcchh-study-SimCRCM> |  Geant components for the use of crmc in FCCSW.
 hep-fccsw | <https://github.com/HEP-FCC/FCCSW> |  FCCSW is the software framework for the future circular collider.
 hep-flattreeanalyzer | <https://github.com/FCC-hh-framework/FlatTreeAnalyzer> |  FlatTreeAnalyzer is a set of scripts for precessing flat trees produced with the Heppy framework.
 hep-gaudi | <https://gitlab.cern.ch/gaudi/gaudi> |  Gaudi, a event processing framework for HEP experiments.
 hep-geant4 | <https://geant4.web.cern.ch/> |  Geant4 is a toolkit for the simulation of the passage of particles through matter.
 hep-geant4data | <https://geant4.web.cern.ch/> |  Required data sets needed to run Geant4.
 hep-hepmc | <https://lcgapp.cern.ch/project/simu/HepMC/> |  HepMC is a C++ Event Record for Monte Carlo Generators.
 hep-heppdt | <https://lcgapp.cern.ch/project/simu/HepPDT/> |  HepPDT is a C++ Particle Data Table.
 hep-heppy | <https://github.com/HEP-FCC/heppy> |  Heppy is a python analysis framework for high energy physics
 hep-madgraph | <https://cp3.irmp.ucl.ac.be/projects/madgraph/> |  Madgraph is a framework for computations of cross sections, the generation of hard events and their matching with event generators.
 hep-podio | <https://github.com/HEP-FCC/podio> |  PODIO is a generator for plain-old-data-based data models.
 hep-pythia8 | <http://home.thep.lu.se/Pythia/> |  Pythia is a standard tool for the generation of events in high-energy collisions.
 hep-root | <https://root.cern.ch> |  ROOT: a modular scientific software toolkit.
 hep-tricktrack | <https://github.com/HSF/TrickTrack> |  HEP track seeding library for pattern recognition with Cellular Automata.
 hep-vc | <https://github.com/VcDevel/Vc> |  SIMD Vector Classes for C++
 hep-veccore | <https://gitlab.cern.ch/VecGeom/VecCore> |  SIMD Vectorization Library for VecGeom and GeantV.
 hep-vecgeom | <https://gitlab.cern.ch/VecGeom/VecGeom> |  VecGeom, the vectorized geometry library for particle-detector simulation (toolkits).
