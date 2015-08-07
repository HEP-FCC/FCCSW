from Configurables import Generation, MinimumBias, Pythia8Production
from Configurables import Inclusive, SignalPlain, SignalRepeatedHadronization
from Configurables import Special

Pythia8TurnOffFragmentation = [ "HadronLevel:all = off" ]

gen = Generation()
gen.addTool( MinimumBias , name = "MinimumBias" )
gen.MinimumBias.ProductionTool = "Pythia8Production"
gen.MinimumBias.addTool( Pythia8Production , name = "Pythia8Production" )
gen.MinimumBias.Pythia8Production.Tuning = "LHCbDefault_CT09MCS.cmd"

gen.addTool( Inclusive , name = "Inclusive" )
gen.Inclusive.ProductionTool = "Pythia8Production"
gen.Inclusive.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Inclusive.Pythia8Production.Tuning = "LHCbDefault_CT09MCS.cmd"

gen.addTool( SignalPlain , name = "SignalPlain" )
gen.SignalPlain.ProductionTool = "Pythia8Production"
gen.SignalPlain.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalPlain.Pythia8Production.Tuning = "LHCbDefault_CT09MCS.cmd"

gen.addTool( SignalRepeatedHadronization , name = "SignalRepeatedHadronization" )
gen.SignalRepeatedHadronization.ProductionTool = "Pythia8Production"
gen.SignalRepeatedHadronization.addTool( Pythia8Production , name = "Pythia8Production" )
gen.SignalRepeatedHadronization.Pythia8Production.Tuning = "LHCbDefault_CT09MCS.cmd"
gen.SignalRepeatedHadronization.Pythia8Production.Commands += Pythia8TurnOffFragmentation

gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "Pythia8Production"
gen.Special.addTool( Pythia8Production , name = "Pythia8Production" )
gen.Special.Pythia8Production.Tuning = "LHCbDefault_CT09MCS.cmd"
