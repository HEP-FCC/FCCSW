#!/bin/bash

# $1: joboption name (geant_fullsim_ecal_SPG_batch.py)
# $2: number of events to be processed
# $3: energy of generated particles (in GeV)
# $4: bfield on/off (1/0)
export JOB=$1
export NEVT=$2
export ENEGEV=$3
#Conversion from GeV to MeV (energy must be in MeV for DD4hep)                                                                                            
export ENEMEV=$(($ENEGEV * 1000))
export BFIELD=$4

mkdir MyWorkDir
cd MyWorkDir 

# Copy entire run directory in my working place
mkdir FCCSW
cd FCCSW
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/Algebra .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/DetectorDescription .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/DataObjects .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/FWCore .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/Generation .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/RecoGeometry .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/Reconstruction .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/Sim .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/Test .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/Tracking .
cp -r /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/cmake .
cp /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/run .
cp /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/submit_batch/*.py .
cp /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/*.sh .
cp /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/CMakeLists.txt .
cp /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/Makefile .
cp /afs/cern.ch/work/n/novaj/HEP-FCC/FCCSW/toolchain.cmake .

# List content of the working directory for debugging purposes
echo "ls -rtl "
ls -rtl

# Prepare the joboption file (add the parameters on top)
sed -i '1s/^/\n/' ${JOB}
sed -i '1s/^/EVTMAX='${NEVT}'\n/' ${JOB}
sed -i '1s/^/ENE='${ENEMEV}'\n/' ${JOB}
sed -i '1s/^/BFIELD='${BFIELD}'\n/' ${JOB}

# Joboption file:
echo "Joboption file content:"
cat ${JOB}

# Setup & compile
echo "Setting the enviroment"
source init.sh
echo "Compiling "
make clean
make -j 8

# Run the job
./run gaudirun.py ${JOB} | tee myjob.log

# Copy out the results if exist
if [ -e output.root ] ; then
xrdcp output.root root://eosatlas//eos/atlas/user/n/novaj/FCC/April1/hits_fccsw_ecal_bfield${BFIELD}_e${ENEGEV}GeV_v1.root
xrdcp myjob.log root://eosatlas//eos/atlas/user/n/novaj/FCC/April1/myjob_ecal_bfield${BFIELD}_e${ENEGEV}GeV_v1.log
fi
 
# Clean workspace before exit
cd ../..
rm -rf MyWorkDir
