---
layout: site
id: mdi
---

# MDI (Machine-Detector Interface)

[contact egroup](mailto:FCC-PED-SoftwareAndComputing-MDI_NOSPAM@cern.ch) | [Mandate](Mandate) | [Intro slides @PSC](https://indico.cern.ch/event/1668596/contributions/7014766/attachments/3259323/5818928/BIB-MDI%20coordination.pdf)

Initial page to gather recipes for FCCee MDI-related software.

<details>
<summary>Collapsed section example </summary>

You can add text within a collapsed section.
You can add an image or a code block, too.

</details>

## mdi_v2

aciarma/k4geo/tree/ mdi_v2

Second iteration of the cryostat model. Details on [MDI#79]([url](https://indico.cern.ch/event/1679641/)).
Starts at 1400mm from IP Max radius 220mm Profile follows 100mrad
5mm Tungsten layer shielding.
Quadrupoles and 2 SR masks for L*=2.4m
Not an engineered model.

<img width="802" height="555" alt="Screenshot 2026-05-28 alle 16 58 25" src="https://github.com/user-attachments/assets/bba52f4e-f7b8-4c2f-9a55-1d7154dd9a0c" />

Field map for IR antisolenoid produced by B. Parker for the non-local compensation scheme 
- only screening anti-solenoid
- 
The field map was produced starting from a realistic field map for IDEA. The k4geo description instead uses a flat 2T field. Thus when overlapping the two fields we get some non-zero field inside the screening solenoids.
• This is not 100% correct but still better then having 2T.

Similar behaviour for the other detector concepts. Once every concept provide (and use in k4geo) a realistic field map, a dedicated antisolenoid field map can be produced.

<img width="1085" height="226" alt="Screenshot 2026-05-28 alle 16 59 15" src="https://github.com/user-attachments/assets/61a5ca9f-adf2-41ac-a765-115adb1960d1" />

The use of a field map for the IR antisolenoids allows us to have the fringe fields (Br component) due to the transition between detector field and screening antisolenoid field.

Fringes are important for the transport of low pT low angle particles that can spiralize along the field lines and enter the detectors.

In the non-local compensation scheme this is much reduced w.r.t. the local scheme (which foresees a -5T compensation antisolenoid in front of the screening antisolenoid).

<img width="578" height="356" alt="Screenshot 2026-05-28 alle 17 00 04" src="https://github.com/user-attachments/assets/5b17697e-4559-41ec-a064-e91e2ccc13a8" />


The xml files to be used to produce samples are the following:

aciarma/k4geo/tree/mdi_v2:k4geo/FCCee/ALLEGRO/compact/ALLEGRO_o1_v03/ALLEGRO_o1_v03_CAD.xml
aciarma/k4geo/tree/mdi_v2:k4geo/FCCee/IDEA/compact/IDEA_o1_v03/IDEA_o1_v03_CAD.xml
aciarma/k4geo/tree/mdi_v2:k4geo/FCCee/ILD_FCCee/compact/ILD_FCCee_v01/ILD_FCCee_v01_CAD.xml

Please remember that the IR antisolenoid field map does not match perfectly the flat 2T field which is currently in the detectors k4geo description.

More accurate maps will be produced once the detectors provide and implement a realistic field map for their own solenoid and return fields.


## BIB sample production

## bib-studies

See [bib-studies]([url](https://github.com/HEP-FCC/bib-studies)) for how to simulate BIB samples through a detector and produce occupancy plots.
