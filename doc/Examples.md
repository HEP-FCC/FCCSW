List of examples
====


## BASICS examples

* [example_options.py](../Examples/options/example_options.py)
  * reading events from a HepMC file
  * dumping `HepMC::GenEvent`
  * using dummy algorithm that creates EDM `fcc::JetCollection`
  * writing the EDM output to ROOT file using PODIO

* [simple_workflow.py](../Examples/options/simple_workflow.py)
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * using sample jet clustering algorithm
  * writing the EDM output to ROOT file using PODIO

* [simple_pythia.py](../Examples/options/simple_pythia.py)
  * generating PYTHIA events and saving them to HepMC
  * converting `HepMC::GenEvent` to EDM
  * filtering MC particles
  * using sample jet clustering algorithm
  * writing the EDM output to ROOT file using PODIO

* [particleGun.py](../Generation/options/particleGun.py)
  * generating single particle events from a given list of types,
    with momentum, phi and theta from a given range,
    saving to HepMC
  * dumping `HepMC::GenEvent`
  * creating the histograms for HepMC
  * writing the histograms to ROOT file

* [genJetClustering.py](../Reconstruction/options/genJetClustering.py)
  * reading events from a HepMC file
  * creating the histograms for HepMC
  * using sample jet clustering algorithm
  * creating the histograms for jets
  * writing the histograms to ROOT file


## SIMULATION examples

* [PythiaDelphes_config.py](../Sim/SimDelphesInterface/options/PythiaDelphes_config.py)
  * generating PYTHIA events and saving them to HepMC
  * running Delphes simulation algorithm
  * writing the EDM output to ROOT file using PODIO

* [geant_fullsim.py](../Examples/options/geant_fullsim.py)
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * geometry parsed from XML ([TestTracker.xml](../DetectorDescription/Detectors/compact/TestTracker.xml)) by DD4hep using `GeoSvc`
  * Geant4 configured (`G4SimSvc`) with:
    * DD4hep geometry,
    * FTFP_BERT physics list,
    * (empty) action initialisation list.
  * running Geant4 simulation algorithm (`G4SimAlg`), saving the output (tracker and HCal hits)
  * writing the EDM output to ROOT file using PODIO

* [geant_pgun_fullsim.py](../Examples/options/geant_pgun_fullsim.py)
  * generating single particle events from a given list of types,
    with momentum, phi and theta from a given range,
    saving to HepMC
  * converting `HepMC::GenEvent` to EDM
  * geometry parsed from XML ([TestHCal.xml](../DetectorDescription/Detectors/compact/TestHCal.xml)) by DD4hep using `GeoSvc`
  * Geant4 configured (`G4SimSvc`) with:
    * DD4hep geometry,
    * FTFP_BERT physics list,
    * (empty) action initialisation list.
  * running Geant4 simulation algorithm (`G4SimAlg`), saving the output (tracker and HCal hits)
  * writing the EDM output to ROOT file using PODIO

* [geant_fullsim_hcal.py](../Sim/SimG4Components/tests/geant_fullsim_hcal.py)
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * geometry parsed from XML ([TestHCal.xml](../DetectorDescription/Detectors/compact/TestHCal.xml)) by DD4hep using `GeoSvc`
  * Geant4 configured (`G4SimSvc`) with:
    * DD4hep geometry,
    * FTFP_BERT physics list,
    * (empty) action initialisation list.
  * running Geant4 simulation algorithm (`G4SimAlg`), saving the output (HCal hits)
  * writing the EDM output to ROOT file using PODIO

* [geant_fullsim_gdml.py](../Sim/SimG4Components/tests/geant_fullsim_gdml.py)
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * Geant4 configured (`G4SimSvc`) with:
    * geometry taken from [GDML file](../Sim/SimG4Common/gdml/example.xml) (no sensitive detectors!),
    * FTFP_BERT physics list,
    * (empty) action initialisation list.
  * running Geant4 simulation algorithm (`G4SimAlg`)
  * writing the EDM output to ROOT file using PODIO

* [geant_fastsim.py](../Examples/options/geant_fastsim.py)
  * reading events from a HepMC file
  * converting `HepMC::GenEvent` to EDM
  * geometry parsed from XML ([ParametricSimTracker.xml](../DetectorDescription/Detectors/compact/ParametricSimTracker.xml) by DD4hep using `GeoSvc`
  * Geant4 configured (`G4SimSvc`) with:
    * DD4hep geometry,
    * FTFP_BERT physics list with `sim::FastSimPhysics` attached,
    * `G4ParticleSmearSimple` smearing tool for tracker,
    * (empty) action initialisation list.
  * running Geant4 simulation algorithm (`G4SimAlg`), saving the output (smeared particles)
  * writing the EDM output to ROOT file using PODIO
