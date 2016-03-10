


class FCCPileupScenarioBase(object):
    """Beam Parameters Common to all FCC scenarios"""
    def __init__(self, name):
        self._name = name
        self.VertexSmearingConfig = {
          "xVertexMin": -0.5,
          "xVertexMax": 0.5,
          "yVertexMin": -0.5,
          "yVertexMax": 0.5,
          "zVertexMin": -35,
          "zVertexMax": 35
        }
        self.PileupToolConfig = {
            "Filename":  "/afs/cern.ch/exp/fcc/sw/0.6/testsamples/example_MyPythia.dat",
          }

class FCCPhase1Pileup(FCCPileupScenarioBase):
    def __init__(self, name):
        FCCPileupScenarioBase.__init__(self, name)
        self.PileupToolConfig['numPileUpEvents'] = 1

class FCCPhase2Pileup(FCCPileupScenarioBase):
    def __init__(self, name):
        FCCPileupScenarioBase.__init__(self, name)
        self.PileupToolConfig['numPileUpEvents'] = 940
