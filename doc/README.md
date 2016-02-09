Running Doxygen in FCCSW
==

Doxygen
--
Set up doxygen on lxplus:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.sh}
source /afs/cern.ch/sw/lcg/releases/LCG_80/doxygen/1.8.9.1/x86_64-slc6-gcc49-opt/doxygen-env.sh
source /afs/cern.ch/sw/lcg/releases/graphviz/2.28.0-a8340/x86_64-slc6-gcc49-opt/graphviz-env.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/afs/cern.ch/sw/lcg/releases/graphviz/2.28.0-a8340/x86_64-slc6-gcc49-opt/lib
export PATH=$PATH:/afs/cern.ch/sw/lcg/releases/graphviz/2.28.0-a8340/x86_64-slc6-gcc49-opt/bin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Produce doxygen documents (after setting up your FCCSW environment with `init.(c)sh`)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.sh}
cd $FCCSW
make run-doxygen
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
