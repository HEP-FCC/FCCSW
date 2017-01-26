#!/bin/bash

export ENE=$1
export EVTMAX=$2
export BFIELD=$3
export i=$4

#path of the software
echo "Export paths"
export SOFTWARE_PATH_AFS=/afs/cern.ch/user/c/cneubuse/FCCSW
export CMTCONFIG=x86_64-slc6-gcc49-opt

echo "Copy a working directory"
cp -r $SOFTWARE_PATH_AFS .

cd FCCSW/

echo "Init"
#source the script
source init.sh

#add job options to the python script
sed -i "8s/.*/energy=$ENE/g"  condor/geant_fullsim_hcal_singleparticles.py 
sed -i "9s/.*/num_events=$EVTMAX/g" condor/geant_fullsim_hcal_singleparticles.py 
sed -i "10s/.*/magnetic_field=$BFIELD/g" condor/geant_fullsim_hcal_singleparticles.py
sed -i "11s/.*/i=$i/g" condor/geant_fullsim_hcal_singleparticles.py

echo "Cat geant_fullsim_hcal_singleparticles.py"
cat condor/geant_fullsim_hcal_singleparticles.py

echo "run"
LD_PRELOAD=build.$BINARY_TAG/lib/libDetSegmentation.so ./run gaudirun.py condor/geant_fullsim_hcal_singleparticles.py

echo "LS:"
ls -l

echo "Copy output file to eos"
export EOS_MGM_URL="root://eospublic.cern.ch"
source /afs/cern.ch/project/eos/installation/client/etc/setup.sh
xrdcp output.root root://eospublic//eos/fcc/users/c/cneubuse/FccHcal/fullModelSim/output_pi$((${ENE}/1000))GeV_bfield${BFIELD}_part${i}.root

echo "Cleaning"
cd ../
rm -rf FCCSW
