#!/bin/bash
FILE="compare.out"
PHYSICS="papasPhysics.txt"
MATCHPHYSICS=$FCCPAPASCPP/data/required_cms_physics.txt
ROOTFILE=$FCCPAPASCPP/data/ee_ZH_Zmumu_Hbb.root
if [ -f $FILE ]
then
rm $FILE
fi
if [ -f $PHYSICS ]
then
rm $PHYSICS
fi
./run fccrun.py   Sim/SimPapas/options/simple_CMS.py -r $ROOTFILE
if ([ -f $PHYSICS ] && [ -f $MATCHPHYSICS ])
then
#echo "difference files"
diff $MATCHPHYSICS $PHYSICS  >  $FILE

if [ -f $FILE ]
then
if [[ -s $FILE ]];
then
echo "$FILE is different"
else
echo "$FILE matches"
rm $FILE
rm $PHYSICS
fi
else
echo "$FILE not found."
fi
else
echo "$PHYSICS not found."
fi


