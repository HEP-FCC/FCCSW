#!/bin/sh -u
if ! [[ $LD_LIBRARY_PATH == *"llvm"* ]] ; then
    echo "setting up LLVM"
    source /afs/cern.ch/sw/lcg/external/llvm/3.6/x86_64-slc6/setup.sh
else
    echo "LLVM already setup."
fi

if ! find .clang-format ; then
    echo "copying FCCSW clang-format"
    cp /afs/cern.ch/exp/fcc/sw/0.6/SAS/0.1.4/x86_64-slc6-clang3.6-opt/config/FCCSW-clang-format.yaml .clang-format
fi


for fname in "$@"
do
    echo "Checking $fname"
    python /afs/cern.ch/exp/fcc/sw/0.6/SAS/0.1.4/x86_64-slc6-clang3.6-opt/scripts/sasFormattingChecker.py "$fname"
done


