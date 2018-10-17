Interactive mode driven by command lines of Geant4 in FCCSW
==

The interactive mode allows for FCCSW to run interactively, waiting for command lines enetered from keyboard.

By running the script: 
./run gaudirun.py Examples/options/interactive_mode.py

the program will load the geometry and enters into the interactive mode waiting for commands to be entered, by setting InteractiveMode="true" in SimG4Svc.

If the PATH has been correctly configured with the DAWN installation, the user by entering the commands here below, can visualise the geometry in DAWN:

/vis/scene/create
/vis/scene/add/volume
/vis/scene/add/hits
/vis/scene/add/trajectories
/tracking/storeTrajectory 1
/vis/open DAWNFILE
/vis/geometry/set/forceSolid all -1 true
/vis/drawVolume
/vis/viewer/flush

