from Gaudi.Configuration import *
from Configurables import ApplicationMgr, HepMCReader, HepMCConverter

hepmc_reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
hepmc_reader.Outputs.hepmc.Path = "hepmc_reader"

hepmc_converter = HepMCConverter("Converter")
hepmc_converter.Inputs.hepmc.Path="hepmc_reader"

ApplicationMgr(EvtSel='NONE',
               EvtMax=1,
               TopAlg=[hepmc_reader,
                       hepmc_converter])
