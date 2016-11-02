# Quick start

~~~{.sh}
./run gaudirun.py Examples/options/geant_pgun_fullsim.py
~~~

1. Generation of single particle events (**100 GeV pi-**)
2. Creation of FCC-hh baseline detector (**tracker, EMCal, HCal**)
3. Simulation with **FTFP_BERT** physics list

#### Output

- generated particles
- generated vertices
- hits in the sensitive parts of the tracker (barrel + endcaps), the EMCal (barrel) , the HCal (barrel)


For the full explanation of the example [see](Geant4fullsim.md#2-example)


# How to ...

* [change event generator](#event-generators)
* [understand the geometry](../../Detector/doc/DD4hepInFCCSW.md)
* change simulation configuration:
  - [change the physics list](Geant4fullsim.md#how-to-use-different-physics-list)
  - [add user action](Geant4fullsim.md#how-to-add-a-user-action)
  - [use fast simulation for trackers](Geant4fastsim.md#2-example)


For the full documentation of the simulation in Geant [see](Geant4fullsim.md)

## Event generators

Different event generators are used in verious examples. Please consult them for any details:

- particle gun:

  ~~~{.sh}
  ./run gaudirun.py Examples/options/geant_pgun_fullsim.py
  ~~~

  or in case of e.g. geantino, the particle gun from Geant4 may be used:

  ~~~{.sh}
  ./run gaudirun.py Examples/options/geant_fullsim_pgun_geantino.py
  ~~~

- using Pythia8:

  ~~~{.sh}
  ./run gaudirun.py Examples/options/simple_pythia.py
  ~~~

- reading from HepMC file:

  ~~~{.sh}
  ./run gaudirun.py Examples/options/geant_fullsim.py
  ~~~

- reading from EDM file:

  ~~~{.sh}
  ./run gaudirun.py Examples/options/read_podio_input.py
  ~~~
