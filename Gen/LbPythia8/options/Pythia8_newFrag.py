from Configurables import Generation, MinimumBias, Pythia8Production
from Configurables import Inclusive, SignalPlain, SignalRepeatedHadronization
from Configurables import Special

Pythia8TurnOffFragmentation = [ "HadronLevel:all = off" ]

# this is only a preliminary tuning that should probably need to be updated
Pythia8NewFrag = [  "StringZ:useNonstandardB = on" ,
                    "StringZ:aNonstandardB = 1.01" ,
                    "StringZ:bNonstandardB = 0.46" ,
                    "StringZ:rFactB = 0",
                    "StringZ:useNonstandardC = on" ,
                    "StringZ:aNonstandardC = 1.075" ,
                    "StringZ:bNonstandardC = 0.694" ,
                    "StringZ:rFactC = 0"]

gen = Generation()
gen.addTool( MinimumBias , name = "MinimumBias" )
gen.MinimumBias.ProductionTool = "Pythia8Production"
gen.MinimumBias.addTool( Pythia8Production , name = "Pythia8Production" )
gen.MinimumBias.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.MinimumBias.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool( Inclusive , name = "Inclusive" )
gen.Inclusive.ProductionTool = "Pythia8Production"
gen.Inclusive.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Inclusive.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.Inclusive.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool( SignalPlain , name = "SignalPlain" )
gen.SignalPlain.ProductionTool = "Pythia8Production"
gen.SignalPlain.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalPlain.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalPlain.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool( SignalRepeatedHadronization , name = "SignalRepeatedHadronization" )
gen.SignalRepeatedHadronization.ProductionTool = "Pythia8Production"
gen.SignalRepeatedHadronization.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalRepeatedHadronization.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8TurnOffFragmentation
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8NewFrag

gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "Pythia8Production"
gen.Special.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Special.Pythia8Production.Tuning = "LHCbDefault.cmd"
gen.Special.Pythia8Production.Commands += Pythia8NewFrag
