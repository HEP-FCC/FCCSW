

# Adding Pileup in FCCSW in Generation

Some of the FCC scenarios have a large number of pileup collisions in each event.
A full simulation of the full event is very expensive, but some studies may nevertheless require the full information.
For this purpose, FCCSW includes infrastructure to add pileup at generator level.


By default, the particle sources in the Generator package of FCCSW can handle pileup, but are set to a default of no pileup.
The standard way of adding pileup is to add a `Pileup` tool, which mainly configures the number of pileup events. There are different implemenations of this tool in FCCSW, depending on the desired distribution (Constant or Poisson-distributed number of pileup events mixed in each event)


## Pileup parameters

To access FCC-hh pileup parameters, FCCSW includes some small Python dictionaries with the relevant values in `Generation/python/FCCPileupScenarios.py` which can be used in configuration files by, p.ex.,

```python
from FCCPileupScenarios import FCCPhase1PileupConf
```
Consult the file `FCCPileupScenarios.py` for more information on values in the dictionaries.


## Adding Minimum Bias events in Pythia

Using the standard event generator `Pythia`, minimum bias events can be generated and added on the fly using the `PythiaInterface` algorithm in the Generation package. A pythia `.cmd` file for FCC-hh pileup is provided in the Generation package under `Generation/data/Pythia_minbias_pp_100TeV.cmd`.
Example usage can be seen in `Examples/options/simple_pythia.py`. The vertices are smeared according to the parameters in the pythia configuration file.


## Adding Pileup from HepMC file

Pileup particles can also be pre-generated and read from disc in HepMC format. The `GenAlg`-algorithm can again be configured with a `PileupTool`, taking the average number of pileup events to be read and a second `HepMCProvider` for the pileup. An example can be found in `Generation/options/pileup_hepmcreader.py`. 

To prepare Minimum Bias HepMC files, any suitable generator may be used. For Pythia8, consult example [main42](http://home.thep.lu.se/~torbjorn/pythia81html/SampleMainPrograms.html) how to do this. The fcc-physics repository  also contains an example how to use Pythia8 together with the FCC event data model.


