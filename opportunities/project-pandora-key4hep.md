---
layout: site
id: opportunities
---

Pandora integration in Key4hep

## Objective

Integrate `PandoraPFA` into `Key4hep` so that it can be used in studies based on `Key4hep`, e.g. the `FCC-ee` studies.


## Deliverable(s)

`GaudiPandora` algorithm (`Gaudi` algorithm implementing a wrapper around `PandoraPFA`, using EDM4hep as ‘language’ and DD4hep for geometry).
Automatic validation suite.


## Methodology

The idea is to develop the validation suite as tool to test the various steps of the development process, which are
1. Get familiar with running `PandoraPFA` in native `iLCSoft` (`DDMarlinPandora`, `Marlin` processor); results should be converted to `EDM4hep`. Implement the corresponding workflow to be run automatically.
2. Get familiar with running `DDMarlinPandora` in `Key4hep` through the `k4MarlinWrapper`. Implement the corresponding workflow to be run automatically.
3. Develop a validation strategy to automatically compare the results of 1 and 2 for a given process and detector (this should be part of a global `Key4hep` validation suite) . Validate 1 and 2 for a meaningful set of processes (electron gun, tau gun, ditau event, hadronic event, …) and a reference detector (`CLD`).
4. Implement a Gaudi algorithm interfacing `PandoraPFA` and using `DD4hep` for geometry (equivalent of `DDMarlinPandora`; output in `EDM4hep`). The algorithm should be added to the existing k4Pandora repository, which contains an implementation based on `GEAR` geometry.
5. Validate 4 wrt 1,2 using 3.


## Technology(ies)

Languages: `C++` for `Gaudi`; `Python` for control code
CI/CD: `GitHub` actions or similar

## Concerns also
`Key4hep`

## Contact(s)

J. Smiesko (`juraj.smiesko.AT.cern.ch`)
A. Sailer (`andre.philippe.sailer.AT.cern.ch`)
G. Ganis (`gerardo.ganis.AT.cern.ch`)

## History

16 January 2022 (published on http://fccsw.web.cern.ch)