Detector studies
==

### Table of contents:

* [Calorimetry](#calorimeter-studies)
  * [Sampling fraction](#how-to-calculate-a-sampling-fraction)
  * [Dead material correction](#how-to-correct-for-dead-material)

Calorimeter Studies
--

### How to calculate a sampling fraction

Sampling fraction of the calorimeter is defined as the energy deposited in the active material to the total deposited energy (including the absorbers, readout).

If the sampling calorimeter has a constant ratio of the amount of the active to passive material, the sampling fraction can be expressed as one constant. Energy deposited in the active volume of the calorimeter, divided by this constant, calculates the energy that the particle deposited in the calorimeter.
In this case the calculation can be done with the single particle simulation with only one detector, and by taking the known particle energy as the total energy (assuming the total containment). [Example analysis script](https://github.com/zaborowska/FCC_calo_analysis_cpp/blob/withJanasChanges/scripts/test_macro_profiles.py#L25).

For the calorimeters that have a changing ratio of the amount of the active to passive material, the sampling fraction can be calculated for each cell individually. Assuming the azimuthal angle symmetry of a cylindriacal detector, the sampling fraction can be extracted for each of the longitudinal layers of the detector. This can be achived with `SamplingFractionInLayers` algorithm. It requires a dedicated simulation, run with saving of all the deposits within the calorimeter (including the passive material). The sampling fraction is calculated for each event as the ratio of the energy deposited in the active material, to the total deposited energy, within one layer. 

Example [DetStudies/tests/options/samplingFraction_inclinedEcal.py](../DetStudies/tests/options/samplingFraction_inclinedEcal.py):

~~~{.py}
from Configurables import SamplingFractionInLayers
hist = SamplingFractionInLayers("hists",
                                 energyAxis = 50,
                                 readoutName = "ECalBarrelEta",
                                 layerFieldName = "layer",
                                 numLayers = 8,
                                 activeFieldName = "type",
                                 activeFieldValue = 0,
                                 OutputLevel = INFO)
hist.deposits.Path="ECalBarrelPositionedHits"

THistSvc().Output = ["rec DATAFILE='histSF_inclined_e50GeV_eta0_1events.root' TYP='ROOT' OPT='RECREATE'"]
~~~

- *energyAxis* indicates the range of the energy plotted on the histogram
- *readoutName* is the name of the bitfield readout as defined in detector XML definition
- *layerFieldName* defines the name in the bitfield for which the sampling fraction changes
- *numLayers* defines the number of existing values for *layerFieldName*
- *activeFieldName* defines the name in the bitfield that allows to distinguish the active material from passive
- *activeFieldValue* defines the value of *activeFieldName* in the bitfield that corresponds to the active material

Example of the [analysis script](https://github.com/zaborowska/FCC_calo_analysis_cpp/blob/withJanasChanges/scripts/plot_samplingFraction.py).

### How to correct for dead material

Calorimeters may be surrounded by dead material (cryostat, services), in which particles may deposit energy. In order to investigate the dependence of the energy depoisted in the dead material on the energy deposited in the detector (in each layer), algorithm `UpstreamMaterial` can be used. It additionally creates a debug histogram of azimuthal angle dependence of the deposited energy in the detector. The algorithm requires a dedicated simulation, run with saving deposits within the dead material. The distinction between the dead and active material is made looking into bitfield 'cryo'. 'cryo'==0 for active, ==1 for dead material.

Example [DetStudies/tests/options/upstreamMaterial_inclinedEcal.py](../DetStudies/tests/options/upstreamMaterial_inclinedEcal.py):

~~~{.py}
from Configurables import UpstreamMaterial
hist = UpstreamMaterial("hists",
                        energyAxis=50,
                        phiAxis=0.1,
                        readoutName = "ECalBarrelEta",
                        layerFieldName = "layer",
                        numLayers = 8,
                        # sampling fraction is given as the upstream correction will be applied on calibrated cells
                        samplingFraction = [0.12125] + [0.14283] + [0.16354] + [0.17662] + [0.18867] + [0.19890] + [0.20637] + [0.20802],
                        OutputLevel = VERBOSE)
hist.deposits.Path="ECalBarrelCells"
hist.particle.Path="GenParticles"

THistSvc().Output = ["det DATAFILE='histUpstream_hits_e50GeV_eta0_Bfield1_10events_8layers.root' TYP='ROOT' OPT='RECREATE'"]
~~~

- *energyAxis* indicates the range of the energy plotted on the histogram
- *phiAxis* indicates the range of the azimuthal angle plotted on the histogram
- *readoutName* is the name of the bitfield readout as defined in detector XML definition
- *layerFieldName* defines the name in the bitfield for which the sampling fraction changes
- *numLayers* defines the number of existing values for *layerFieldName*
- *samplingFraction* is used for calibration of the deposits, useful is correction for the upstream material is used after the cell calibration (default case)

Example of the [analysis script](https://github.com/zaborowska/FCC_calo_analysis_cpp/blob/withJanasChanges/scripts/plot_upstreamCorrecton.py).