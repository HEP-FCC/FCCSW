
"""Beam Parameters Common to all FCC scenarios. Taken from
Michael Benedikt, Daniel Schulte, and Frank Zimmermann: 
"Optimizing integrated luminosity of future hadron colliders." 
Physical Review Special Topics-Accelerators and Beams 18.10 (2015): 101002.
http://dx.doi.org/10.1103/PhysRevSTAB.18.101002
"""

from GaudiKernel import SystemOfUnits as units


_CommonFCCPileupConf = {
          "xVertexMin": -0.5 * units.mm,
          "xVertexMax": 0.5 * units.mm,
          "yVertexMin": -0.5 * units.mm,
          "yVertexMax": 0.5 * units.mm,
          "zVertexMin": -35 * units.mm,
          "zVertexMax": 35 * units.mm }


FCCPhase1PileupConf = _CommonFCCPileupConf.copy()
FCCPhase1PileupConf['numPileUpEvents'] = 180
    
FCCPhase2PileupConf = _CommonFCCPileupConf.copy()
FCCPhase2PileupConf['numPileUpEvents'] = 940

