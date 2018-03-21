#!/usr/bin/env python
import os, re
import commands
import math, time
import sys

print 
print 'START'
print 
########   YOU ONLY NEED TO FILL THE AREA BELOW   #########
########   customization  area #########
CONFIG="50Layers_2.1mmW_50umPixels_18umThick_RRB"
NPILEUPEVENTS = ["1000"]#,"2","3","4","5","6"]
BFIELD = "1"
EVENTS = "1"
FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
queue = "8nm" # give bsub queue -- 8nm (8 minutes), 1nh (1 hour), 8nh, 1nd (1day), 2nd, 1nw (1 week), 2nw 
########   customization end   #########

path = os.getcwd()
print
#print 'do not worry about folder creation:'
#os.system("rm -r tmp")
os.system("mkdir batch")
#os.system("mkdir res")
print

##### loop for creating and sending jobs #####
for npu in NPILEUPEVENTS:
   ##### creates directory and file list for job #######
   dir="batch_eos/"+CONFIG+"_FCCSW0.9/"+str(npu)+"PILEUPEVENTS"

   os.system("rm -r "+dir)
   os.system("mkdir -p "+dir)
   os.chdir(dir)

   ##### creates jobs #######
   with open('job.sh', 'w') as fout:
      fout.write("#!/bin/sh\n")
      fout.write("echo\n")
      fout.write("echo 'START---------------'\n")
      fout.write("echo 'WORKDIR ' ${PWD}\n")
        
      fout.write("##### creates steering file #######\n")
      fout.write("cp "+FCCSW_DIR+"/geant_pile_up_batch_temp.py run.py\n")
      fout.write("sed -i 's/<NPILEUPEVENTS>/"+str(npu)+"/' run.py\n")
      fout.write("sed -i 's/<NEVENTS>/"+str(EVENTS)+"/' run.py\n\n")
      fout.write("sed -i 's/<CONFIG>/"+str(CONFIG)+"/' run.py\n\n")
      fout.write("source "+FCCSW_DIR+"init.sh\n")
      fout.write(FCCSW_DIR+"run gaudirun.py run.py\n")
      fout.write("echo 'STOP---------------'\n")
      fout.write("echo\n")
      fout.write("cp *.txt "+path+"/"+dir+"\n")
      fout.write("cp *.root "+path+"/"+dir+"\n")
      fout.write("echo\n")
   os.system("chmod 755 job.sh")
   
   ###### sends bjobs ######
   os.system("bsub -q "+queue+" -o logs job.sh")
   print "job nr " + str(npu) + " submitted"
   
   os.chdir(path)
   
print
print "your jobs:"
os.system("bjobs")
print
print 'END'
print
