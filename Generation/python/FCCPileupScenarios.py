
"""Beam Parameters Common to all FCC scenarios. Taken from
Michael Benedikt, Daniel Schulte, and Frank Zimmermann: 
"Optimizing integrated luminosity of future hadron colliders." 
Physical Review Special Topics-Accelerators and Beams 18.10 (2015): 101002.
http://dx.doi.org/10.1103/PhysRevSTAB.18.101002
"""

from GaudiKernel import SystemOfUnits as units


_CommonFCCPileupConf = {
          "xVertexMean": 0 * units.mm,
          "xVertexSigma" : 0.5 * units.mm,
          "yVertexMean": 0 * units.mm,
          "yVertexSigma": 0.5 * units.mm,
          "zVertexMean": 0 * units.mm,
          "zVertexSigma": 70 * units.mm,
          "tVertexMean": 0 * units.picosecond,
          "tVertexSigma": 120 * units.picosecond,
           }


# for flat distributions
_CommonFCCPileupConf['xVertexMin'] = _CommonFCCPileupConf['xVertexMean'] - 2 * _CommonFCCPileupConf['xVertexSigma']
_CommonFCCPileupConf['xVertexMax'] = _CommonFCCPileupConf['xVertexMean'] + 2 * _CommonFCCPileupConf['xVertexSigma']
_CommonFCCPileupConf['yVertexMin'] = _CommonFCCPileupConf['yVertexMean'] - 2 * _CommonFCCPileupConf['yVertexSigma']
_CommonFCCPileupConf['yVertexMax'] = _CommonFCCPileupConf['yVertexMean'] + 2 * _CommonFCCPileupConf['yVertexSigma']
_CommonFCCPileupConf['zVertexMin'] = _CommonFCCPileupConf['zVertexMean'] - 2 * _CommonFCCPileupConf['zVertexSigma']
_CommonFCCPileupConf['zVertexMax'] = _CommonFCCPileupConf['zVertexMean'] + 2 * _CommonFCCPileupConf['zVertexSigma']
_CommonFCCPileupConf['tVertexMin'] = _CommonFCCPileupConf['tVertexMean'] - 2 * _CommonFCCPileupConf['tVertexSigma']
_CommonFCCPileupConf['tVertexMax'] = _CommonFCCPileupConf['tVertexMean'] + 2 * _CommonFCCPileupConf['tVertexSigma']


FCCPhase1PileupConf = _CommonFCCPileupConf.copy()
FCCPhase1PileupConf['numPileUpEvents'] = 180
    
FCCPhase2PileupConf = _CommonFCCPileupConf.copy()
FCCPhase2PileupConf['numPileUpEvents'] = 1020

