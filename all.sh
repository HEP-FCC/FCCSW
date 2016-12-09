#!/bin/bash

windowsEtaSeed=(5 11)
windowsEtaPos=(3 7)
windowsPhiSeed=(15 21 31)
windowsPhiPos=(11 15 21)

B=(0 1)
energies=(10 20)
GEO=("nocryo" "tracker_cryo")

# windowsEtaSeed=(5)
# windowsEtaPos=(3)
# windowsPhiSeed=(15)
# windowsPhiPos=(11)
# B=(0)
# energies=(10)
# GEO="nocryo"

DIR="dataEosDec16/txt/"

etaNum=${#windowsEtaSeed[@]}
phiNum=${#windowsPhiSeed[@]}

#source eosinit.sh
for geo in "${GEO[@]}"; do
    for bfield in "${B[@]}"; do
        for en in "${energies[@]}"; do
            for part in {1..1}; do
                for (( i=0; i<${etaNum}; i++ )); do
                    for (( j=0; j<${phiNum}; j++ )); do
                        ./run fccrun.py\
                              Reconstruction/RecCalorimeter/tests/options/recoEos.py \
                              -n \
                              --text ${geo} \
                              --Bfield ${bfield} \
                              --energy ${en} \
                              --winEta ${windowsEtaSeed[i]} \
                              --winEtaPos ${windowsEtaPos[i]} \
                              --winPhi ${windowsPhiSeed[j]} \
                              --winPhiPos ${windowsPhiPos[j]} \
                              --part ${part}  \
                              > $DIR/out.${geo}.${bfield}.${en}.${windowsEtaSeed[i]}.${windowsEtaPos[i]}.${windowsPhiSeed[j]}.${windowsPhiPos[j]}.${part}.txt \
                              2> $DIR/err.${geo}.${bfield}.${en}.${windowsEtaSeed[i]}.${windowsEtaPos[i]}.${windowsPhiSeed[j]}.${windowsPhiPos[j]}.${part}.txt &
                        echo Starting ${geo}.${bfield}.${en}.${windowsEtaSeed[i]}.${windowsEtaPos[i]}.${windowsPhiSeed[j]}.${windowsPhiPos[j]}.${part}
                    done
                done
            done
        done
    done
done
