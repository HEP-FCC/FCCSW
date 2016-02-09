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

* options/geant_fullsim.py
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * geometry parsed from XML by DD4hep using `GeoSvc`
  * Geant4 configured (`G4SimSvc`) with:
    * DD4hep geometry,
    * FTFP_BERT physics list,
    * `sim::FullSimActions` action initialization list.
  * running simulation algorithm (`G4SimAlg`), saving the output (tracker and HCal hits)
  * writing the EDM output using PODIO

* options/geant_pgun_fullsim.py
  * generating single particle events from a given list of types,
    with momentum, phi and theta from a given range,
    saving to HepMC
  * converting `HepMC::GenEvent` to EDM
  * geometry parsed from XML by DD4hep using `GeoSvc`
  * Geant4 configured (`G4SimSvc`) with:
    * DD4hep geometry,
    * FTFP_BERT physics list,
    * `sim::FullSimActions` action initialization list.
  * running simulation algorithm (`G4SimAlg`), saving the output (tracker and HCal hits)
  * writing the EDM output using PODIO

* options/geant_fastsim.py
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * geometry parsed from XML by DD4hep using `GeoSvc`
  * Geant4 configured (`G4SimSvc`) with:
    * DD4hep geometry,
    * FTFP_BERT physics list with `sim::FastSimPhysics` attached,
    * `G4ParticleSmearSimple` smearing tool for tracker,
    * `sim::FastSimActions` action initialization list.
  * running simulation algorithm (`G4SimAlg`), saving the output (smeared particles)
  * writing the EDM output using PODIO

* options/PythiaDelphes_config.py





* Sim/SimG4Components/tests/geant_fullsim_hcal.py
  * different xml
* Sim/SimG4Components/tests/geant_fullsim_gdml.py
  * geo from GDML and example .gdml

* Generation/config/particleGun.py
* Reconstruction/options/genJetClustering.py
