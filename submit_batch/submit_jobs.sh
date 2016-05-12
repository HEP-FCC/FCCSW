#!/bin/bash

export NEVT=500
export ENE=100

#option -n 8 -R "span[hosts=1]" for running in parallel
for BFIELD in 0 1
do
if [ ${ENE} -le 200 ]
then
    bsub -q 1nw prepare_batch.sh geant_fullsim_ecal_SPG_vertex.py ${NEVT} ${ENE} ${BFIELD}
else 
    bsub -q 1nw -n 8 -R "span[hosts=1]" prepare_batch1.sh geant_fullsim_ecal_SPG_batch.py ${NEVT} ${ENE} ${BFIELD}
fi
done
