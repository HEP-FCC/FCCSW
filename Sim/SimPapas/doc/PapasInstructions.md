#TODO check official documentation to see where it fits
# joschka re library
# joschka make papas tests for it? to run in gaudi (not papas itself)

(1) Download and install papas   https://github.com/HEP-FCC/papas
(2) If already installed remember to call
source init.sh
from papas directory.
(3) Move to FCCSW directory
Down load and install according to instructions

Generate some particles

wget https://raw.githubusercontent.com/HEP-FCC/fcc-physics/master/pythia8/ee_ZH_Zmumu_Hbb.txt


Or On MacOS, do:

curl -O https://raw.githubusercontent.com/HEP-FCC/fcc-physics/master/pythia8/ee_ZH_Zmumu_Hbb.txt

Generate ee to ZH events with Z to mumu and H to b bbar:
fcc-pythia8-generate ee_ZH_Zmumu_Hbb.txt

Run
./run gaudirun.py $FCCSWPATH/Sim/SimPapas/options/simple_papastool.py
 
 
Veryify that you have produced some particles in ROOT:
 root output.root
 events->Print()
 events->Draw("papasreconstructed.core.p4.px", "papasreconstructed.core.status==1")



