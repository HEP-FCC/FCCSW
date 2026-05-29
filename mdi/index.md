---
layout: site
id: mdi
---

# MDI (Machine-Detector Interface)

[contact egroup](mailto:FCC-PED-SoftwareAndComputing-MDI_NOSPAM@cern.ch) &#124; [Mandate](Mandate) &#124; [Intro slides @PSC](https://indico.cern.ch/event/1668596/contributions/7014766/attachments/3259323/5818928/BIB-MDI%20coordination.pdf)

Initial page to gather recipes for FCCee MDI-related software. You will find information about the following set of procedures:
- MDI geometry definition & recommendations on how to use it in the simulation
- Beam-Induce Background (BIB) sample production info & where to find the files
- How to simulate a BIB file through a detector
- How to extract occupancy numbers for a specific sub-detector

---

* This will become a table of contents (this text will be scrapped).
{:toc}

## MDI geometry (mdi_v2)

[aciarma/k4geo/tree/mdi_v2](https://github.com/aciarma/k4geo/tree/mdi_v2)

Second iteration of the cryostat model. Details on [MDI#79](https://indico.cern.ch/event/1679641/).
- Starts at 1400mm from IP Max radius 220mm Profile follows 100mrad
- 5mm Tungsten layer shielding.
- Quadrupoles and 2 SR masks for L*=2.4m
- Not an engineered model.

<img width="802" height="555" alt="Screenshot 2026-05-28 alle 16 58 25" src="https://github.com/user-attachments/assets/bba52f4e-f7b8-4c2f-9a55-1d7154dd9a0c" />

Field map for IR antisolenoid produced by B. Parker for the non-local compensation scheme 
- only screening anti-solenoid
- The field map was produced starting from a realistic field map for IDEA. The k4geo description instead uses a flat 2T field. Thus when overlapping the two fields we get some non-zero field inside the screening solenoids.
- This is not 100% correct but still better then having 2T.

Similar behaviour for the other detector concepts. Once every concept provide (and use in k4geo) a realistic field map, a dedicated antisolenoid field map can be produced.

<img width="1085" height="226" alt="Screenshot 2026-05-28 alle 16 59 15" src="https://github.com/user-attachments/assets/61a5ca9f-adf2-41ac-a765-115adb1960d1" />

The use of a field map for the IR antisolenoids allows us to have the fringe fields (Br component) due to the transition between detector field and screening antisolenoid field.

Fringes are important for the transport of low pT low angle particles that can spiralize along the field lines and enter the detectors.

In the non-local compensation scheme this is much reduced w.r.t. the local scheme (which foresees a -5T compensation antisolenoid in front of the screening antisolenoid).

<img width="578" height="356" alt="Screenshot 2026-05-28 alle 17 00 04" src="https://github.com/user-attachments/assets/5b17697e-4559-41ec-a064-e91e2ccc13a8" />


The xml files to be used to produce samples are the following:

- aciarma/k4geo/tree/mdi_v2:k4geo/FCCee/ALLEGRO/compact/ALLEGRO_o1_v03/ALLEGRO_o1_v03_CAD.xml
- aciarma/k4geo/tree/mdi_v2:k4geo/FCCee/IDEA/compact/IDEA_o1_v03/IDEA_o1_v03_CAD.xml
- aciarma/k4geo/tree/mdi_v2:k4geo/FCCee/ILD_FCCee/compact/ILD_FCCee_v01/ILD_FCCee_v01_CAD.xml
- aciarma/k4geo/tree/mdi_v2:k4geo/FCCee/CLD/compact/CLD_o2_v08/CLD_o2_v08_CAD.xml

Please remember that the IR antisolenoid field map does not match perfectly the flat 2T field which is currently in the detectors k4geo description.

More accurate maps will be produced once the detectors provide and implement a realistic field map for their own solenoid and return fields.


## BIB samples location

The latest background samples are produced for the LCC V106.2 lattice, and can be found here:: 
- Web: https://cernbox.cern.ch/files/spaces/eos/project/f/fcc-ee-mdi/BIB/LCC/V106.2
- EOS: /eos/project/f/fcc-ee-mdi/BIB/LCC/V106.2
In order to access the files on the EOS directories, you need to join the `fcc-ee-MDI` egroup.

The following background sources are to be considered:
- Incoherend Pair Creation (IPC): generated with GuineaPig
- Synchrotron radiation (SR): halo and core, generated with bdsim
- Collimation backgrounds (beam gas and Coulomb scattering): generated with Xsuite and FLUKA

Each of the background sources has a readme file with more information about the generation and the contact persons.


## Detector simulation

Uses `ddsim` (see [docs]([url](https://fcc-ee-detector-full-sim.docs.cern.ch/DD4hep/)). Simulation recipe currently in bib-studies repo [here]([url](https://github.com/HEP-FCC/bib-studies/blob/main/simulation/README.md)).

This will produce root files you can process; see below how.

If you produce good files (i.e. not too experimental), they may be placed in the common EOS space; make sure you follow the existing folder naming and structure!

```sh
#location
/eos/experiment/fcc/ee/simulation
#inner structure:
/eos/experiment/fcc/ee/[simulation,generation]/Key4HEP_release(only for simulations)/ENERGY/EXPERIMENT/STAGE/SAMPLE"
```

Detector concept groups should attempt to produce their own files, with our support.

## bib-studies

See [bib-studies]([url](https://github.com/HEP-FCC/bib-studies)) for how to simulate BIB samples through a detector and produce occupancy plots.


[test](bib-studies.md)

<details>
<summary>Collapsed section example </summary>

You can add text within a collapsed section.
You can add an image or a code block, too.


</details>
