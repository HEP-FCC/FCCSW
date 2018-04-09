FCCSW Simulation Documentation
==

~~~{.sh}
./run fccrun.py Examples/options/geant_fullsim.py
~~~

1. Reading a generated event from HepMC file (example from Pythia)
2. Creation of FCC-hh baseline detector (**tracker**)
3. Simulation with **FTFP_BERT** physics list (1 event)

#### Output ROOT file (**output.root**)

- generated particles
- generated vertices
- hits in the sensitive parts of the tracker (barrel + endcaps)

For the full explanation of the example [see](FullSimulation.md#example)


# How to ...

* [change event generator](#event-generators)
* [change the geometry](../../Detector/doc/DD4hepInFCCSW.md)
* [use fast simulation](FastSimulationUsingGeant.md)
  - [example for tracker and calorimeter](FastSimulationUsingGeant.md#example)

For the full documentation of the simulation in Geant [see](FullSimulation.md)

## Event generation

Different event generators are used in verious examples. Please consult them for any details:

- particle gun (default):

  ~~~{.sh}
  ./run fccrun.py Examples/options/geant_pgun_fullsim.py
  ~~~

  or in case of e.g. geantino, the particle gun from Geant4 may be used (it skips the generation in EDM):

  ~~~{.sh}
  ./run fccrun.py Examples/options/geant_fullsim_pgun_geantino.py
  ~~~

- using Pythia8:

  ~~~{.sh}
  ./run fccrun.py Examples/options/simple_pythia.py
  ~~~

- reading from HepMC file:

  ~~~{.sh}
  ./run fccrun.py Examples/options/geant_fullsim.py
  ~~~

- reading from EDM file:

  ~~~{.sh}
  ./run fccrun.py Examples/options/read_podio_input.py
  ~~~
