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
#ENERGIES = ["1","10","20","30","40","50","60","70","80","90","100","200"]#,"300","400","500"]#,"600","700","800","900","1000" ] # number of jobs to be submitted
ENERGIES = ["300","400","500","600","700","800","900","1000" ] # number of jobs to be submitted
BFIELD = "1"
EVENTS = "100"
FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
queue = "8nh" # give bsub queue -- 8nm (8 minutes), 1nh (1 hour), 8nh, 1nd (1day), 2nd, 1nw (1 week), 2nw 
########   customization end   #########

path = os.getcwd()
print
#print 'do not worry about folder creation:'
#os.system("rm -r tmp")
os.system("mkdir batch")
#os.system("mkdir res")
print

##### loop for creating and sending jobs #####
for e in ENERGIES:
   ##### creates directory and file list for job #######
   dir="batch/30Layers_5.6mmPb_50umPixels_18umThick/"+str(e)+"GeV_BFIELD"+str(BFIELD)
   os.system("mkdir -p "+dir)
   os.chdir(dir)
    
   ##### creates steering file #######
   #os.system("cp ../../geant_batch_temp.py run.py")
   #os.system("sed -i 's/<ENERGY>/"+str(e)+"/' run.py")
   #os.system("sed -i 's/<FIELD>/"+str(BFIELD)+"/' run.py")
   #os.system("sed -i 's/<EVENTS>/"+str(EVENTS)+"/' run.py")
  
   ##### creates jobs #######
   with open('job.sh', 'w') as fout:
      fout.write("#!/bin/sh\n")
      fout.write("echo\n")
      fout.write("echo 'START---------------'\n")
      fout.write("echo 'WORKDIR ' ${PWD}\n")
        
      fout.write("##### creates steering file #######\n")
      fout.write("cp "+FCCSW_DIR+"/geant_batch_temp.py run.py\n")
      fout.write("sed -i 's/<ENERGY>/"+str(e)+"/' run.py\n")
      fout.write("sed -i 's/<FIELD>/"+str(BFIELD)+"/' run.py\n")
      fout.write("sed -i 's/<EVENTS>/"+str(EVENTS)+"/' run.py\n\n")

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
   print "job nr " + str(e) + " submitted"
   
   os.chdir(path)
   
print
print "your jobs:"
os.system("bjobs")
print
print 'END'
print
