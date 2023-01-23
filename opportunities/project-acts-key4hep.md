---
layout: site
id: opportunities
---

# CLD track reconstruction with ACTS in Key4hep

## Objective
Reconstruct CLD charged particle tracks with `ACTS` within `Key4hep` so that it can be used in studies based on `Key4hep`, e.g. the `FCC-ee` studies.


## Deliverable(s)
First concrete version of the module `k4ActsTracking`, implementing a `Gaudi` integration of `ACTS`, using EDM4hep as ‘language’ and DD4hep for geometry.
Automatic validation suite.

## Methodology / Roadmap

1. Develop a `Delphes` plugin for ACTS to create parameterizations to be used in developing testing the interface
    * Pavel Larionov, ALICE, might be contacted (`pavel.larionov.AT.cern.ch`)
    * Should check also with Michele Selvaggi, CMS (`michele.selvaggi.AT.cern.ch`)
2. Run ACTS on `CLD` inner and outer tracker hits.
   1. Translate (`DD4hep`) geometry into ACTS geometry.
   2. Save reconstructed tracks in `EDM4hep` output
   3. Compare with `iLCSoft` based tracking

The implementation examples of CEPC and EIC might also be investigated.

## Technology(ies)
Languages: `C++` for `Gaudi`; `Python` for control code
CI/CD: `GitHub` actions or similar

## Concerns also
`Key4hep`

## Links
1. `Delphes`: <a href="https://github.com/delphes/delphes"> GitHub>
2. `Key4hep`: <a href="https://key4hep.github.io/key4hep-doc/"> Documentation>, <a href="https://github.com/key4hep/"> GitHub>
3. `k4ActsTracking`: <a href="https://github.com/key4hep/k4ActsTracking"> GitHub>

## Contact(s)
B. Francois (`brieuc.francois.AT.cern.ch`), 
A. Salzburger (`Andreas.Salzburger.AT.cern.ch`), 
G. Ganis (`gerardo.ganis.AT.cern.ch`)

## History
_17 January 2022 (published on <a href="http://fccsw.web.cern.ch"> http://fccsw.web.cern.ch </a>)_
