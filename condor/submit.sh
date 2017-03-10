#!/bin/bash

#iteration number - begins with 1 (also used for the seed)
export NJOBS=20
#total number of jobs to submit
export EVTMAX=500
export BFIELD=0
#0 off, 1 on

for ENE in 10 20 100 500 2000
do
  for i in `seq 1 $NJOBS`
  do
      echo $i
      if [ ${ENE} -le 5000 ]
      then
	  sed -i "2s/.*/arguments             = $((${ENE}*1000)) ${EVTMAX} ${BFIELD} ${i} \$\(ClusterID\) \$\(ProcId\)/g"  submit.sub 
	  condor_submit submit.sub
      else
	  sed -i "2s/.*/arguments             = $((${ENE}*1000)) ${EVTMAX} ${BFIELD} ${i} \$\(ClusterID\) \$\(ProcId\)/g"  submit_long.sub 
	  condor_submit submit_long.sub
      fi
  done
done
