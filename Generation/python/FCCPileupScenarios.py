
from GaudiKernel import SystemOfUnits as units


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
          "xVertexMin": -0.5 * units.mm,
          "xVertexMax": 0.5 * units.mm,
          "yVertexMin": -0.5 * units.mm,
          "yVertexMax": 0.5 * units.mm,
          "zVertexMin": -35 * units.mm,
          "zVertexMax": 35 * units.mm
        }
        self.PileupToolConfig = {}

class FCCPhase1Pileup(FCCPileupScenarioBase):
    def __init__(self, name):
        FCCPileupScenarioBase.__init__(self, name)
        self.PileupToolConfig['numPileUpEvents'] = 180

class FCCPhase2Pileup(FCCPileupScenarioBase):
    def __init__(self, name):
        FCCPileupScenarioBase.__init__(self, name)
        self.PileupToolConfig['numPileUpEvents'] = 940
