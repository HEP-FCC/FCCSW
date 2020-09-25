Noble liquid calorimeter
==

### Table of contents:

* [Description of the ECAL geometry] (#description-of-ecal)
* [Where is relevant code] (#code-where)
* [How to run Geant4 full simulations] (#run-simulations)
* [Optimisation of the geometry] (#optimisation)
  * [Which parameters are tunable] (#parameters)
  * [Sampling fraction calculation] (#sampling-fraction)
* [Reconstruction in the calorimeters] (#reconstruction)

Description of the ECAL geometry
--

Highly granular noble liquid calorimeter is composed of absorber plates (lead-steel) inclined with respect to the radial direction. The active material between the plates is liquid argon (LAr). Readout is performed with straight multilayer electrodes realised by printed circuit boards (PCBs). The readout planes are placed in between the absorber plates. The calorimeter is placed in the cryostat to cool down the noble liquid.

The detector collects the signal in individual cells that divide the detector in azimuthal angle, depth and pseudorapidity. The division of the calorimeter in the azimuthal angle is defined by the number of absorbers. The elementary cell, which is the smallest area from which the signal can be collected is the gap in between the absorber and the readout. The elementary cells can be combined, creating a cell of a desired size.

Special layers
- first layer (presample): The absorbers do not contain lead in the middle in the presampler layer
- second layer (strip layer): The granularity in pseudorapidity is 4x higher compared to the rest of the layers

Details about the design of the calorimeter are [here] (https://arxiv.org/abs/1912.09962)

Where is relevant code
--

How to run Geant4 full simulations with noble liquid calorimeter
--

Optimisation of the geometry
--
### Which parameters could be tunned

### Optimisation circle


Reconstruction in the calorimeters
--


