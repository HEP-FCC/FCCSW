#!/bin/bash

export NEVT=500

export BFIELD=0

#option -n 8 -R "span[hosts=1]" for running in parallel
for ENE in 500 1000 5000 10000
do
if [ ${ENE} -le 500 ]
then
    bsub -q 1nw prepare_batch.sh geant_fullsim_ecal_SPG_batch.py ${NEVT} ${ENE} ${BFIELD}
else 
    bsub -q 1nw -n 8 -R "span[hosts=1]" prepare_batch.sh geant_fullsim_ecal_SPG_batch.py ${NEVT} ${ENE} ${BFIELD}
fi
done
