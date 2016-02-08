List of examples
====

* options/example_options.py
  * reading events from a HepMC file
  * dumping `HepMC::GenEvent`
  * using dummy algorithm `CreateSampleJet` that creates EDM `fcc::JetCollection`
  * writing the EDM output using PODIO

* options/simple_workflow.py
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * using sample (jet clustering) algorithm
  * writing the EDM output using PODIO

* options/simple_pythia.py
  * genereting PYTHIA events and saving them to HepMC
  * converting `HepMC::GenEvent` to EDM
  * filtering MC particles
  * using sample (jet clustering) algorithm
  * writing the EDM output using PODIO

* options/geant_pgun_fullsim.py

* options/geant_fullsim.py

* options/PythiaDelphes_config.py

* options/geant_fastsim.py


* Sim/SimG4Components/tests/geant_fullsim_hcal.py
* Sim/SimG4Components/tests/geant_fullsim_gdml.py
* Sim/SimG4Components/tests/geant_fullsim_moreEvents.py

* Generation/config/particleGun.py
* Examples/options/delphes_simulation.py
* Examples/options/dummy_simulation.py
* Examples/options/compare_material.py
* Reconstruction/options/genJetClustering.py
