
"""Beam Parameters Common to all FCC scenarios. Taken from
Michael Benedikt, Daniel Schulte, and Frank Zimmermann: 
"Optimizing integrated luminosity of future hadron colliders." 
Physical Review Special Topics-Accelerators and Beams 18.10 (2015): 101002.
http://dx.doi.org/10.1103/PhysRevSTAB.18.101002
"""

from GaudiKernel import SystemOfUnits as units


_CommonFCCPileupConf = {
          "xVertexMean": 0 * units.mm,
          "xVertexSigma" = 0.5 * units.mm
          "yVertexMean": 0 * units.mm,
          "yVertexSigma": 0.5 * units.mm,
          "zVertexMean": 0 * units.mm,
          "zVertexSigma": 70 * units.mm }
          "tVertexMean": 0 * units.ps,
          "tVertexSigma": 120 * units.ps }


# for flat distributions
_CommonFCCPileupConf['xVertexMin'] = xVertexMean - 2 * xVertexSigma
_CommonFCCPileupConf['xVertexMax'] = xVertexMean + 2 * xVertexSigma
_CommonFCCPileupConf['yVertexMin'] = yVertexMean - 2 * yVertexSigma
_CommonFCCPileupConf['yVertexMax'] = yVertexMean + 2 * yVertexSigma
_CommonFCCPileupConf['zVertexMin'] = zVertexMean - 2 * zVertexSigma
_CommonFCCPileupConf['zVertexMax'] = zVertexMean + 2 * zVertexSigma
_CommonFCCPileupConf['tVertexMin'] = tVertexMean - 2 * tVertexSigma
_CommonFCCPileupConf['tVertexMax'] = tVertexMean + 2 * tVertexSigma


FCCPhase1PileupConf = _CommonFCCPileupConf.copy()
FCCPhase1PileupConf['numPileUpEvents'] = 180
    
FCCPhase2PileupConf = _CommonFCCPileupConf.copy()
FCCPhase2PileupConf['numPileUpEvents'] = 1020

