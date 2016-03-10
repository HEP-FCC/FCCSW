


class FCCPileupScenarioBase(object):
    """Beam Parameters Common to all FCC scenarios. Taken from
    Benedikt, Michael, Daniel Schulte, and Frank Zimmermann: 
    "Optimizing integrated luminosity of future hadron colliders." 
    Physical Review Special Topics-Accelerators and Beams 18.10 (2015): 101002.
    http://dx.doi.org/10.1103/PhysRevSTAB.18.101002
    """
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
        self.PileupToolConfig['numPileUpEvents'] = 180

class FCCPhase2Pileup(FCCPileupScenarioBase):
    def __init__(self, name):
        FCCPileupScenarioBase.__init__(self, name)
        self.PileupToolConfig['numPileUpEvents'] = 940
