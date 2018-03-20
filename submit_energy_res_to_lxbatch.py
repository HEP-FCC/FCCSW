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
FCCSW_VERSION="FCCSW0.9"
#ENERGIES = ["10","20","50","70","100","200", "300", "500","700","1000"]
#ENERGIES = ["10", "50","100","200", "300", "500","700","1000"]
#ENERGIES = ["101","501","1001" ] # number of jobs to be submitted
ENERGIES = ["10"]#,"20","30","40","60","70","80","90","100"] # number of jobs to be submitted
BFIELD = "4"
EVENTS = 10
ETAMIN = -0.001
ETAMAX = 0.001
NRUNS = 1
FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
queue = "8nm" # give bsub queue -- 8nm (8 minutes), 1nh (1 hour), 8nh, 1nd (1day), 2nd, 1nw (1 week), 2nw 
########   customization end   #########

path = os.getcwd()
print
#print 'do not worry about folder creation:'
#os.system("rm -r tmp")
os.system("mkdir batch_eos")
#os.system("mkdir res")
print

##### loop for creating and sending jobs #####
#e = 100
for e in ENERGIES:
#for BFIELD in range(0,7):
   ##### creates directory and file list for job #######
   dir="batch_eos/"+CONFIG+"_"+FCCSW_VERSION+"/"+str(e)+"GeV_BFIELD"+str(BFIELD)+"T_ETAMIN"+str(ETAMIN)+"_ETAMAX"+str(ETAMAX)
   #os.system("rm -r "+dir)
   os.system("mkdir -p "+dir)
   os.chdir(dir)
    
   ##### creates steering file #######
   #os.system("cp ../../geant_batch_temp.py run.py")
   #os.system("sed -i 's/<ENERGY>/"+str(e)+"/' run.py")
   #os.system("sed -i 's/<FIELD>/"+str(BFIELD)+"/' run.py")
   #os.system("sed -i 's/<EVENTS>/"+str(EVENTS)+"/' run.py")
  
   for N in range(0,0+NRUNS):

     ##### creates jobs #######
     with open('job'+str(N)+'.sh', 'w') as fout:
        fout.write("#!/bin/sh\n")
        fout.write("echo\n")
        fout.write("echo 'START---------------'\n")
        fout.write("echo 'WORKDIR ' ${PWD}\n")
          
        fout.write("##### creates steering file #######\n")
        fout.write("cp "+FCCSW_DIR+"/geant_batch_temp.py run.py\n")
        fout.write("sed -i 's/<ENERGY>/"+str(e)+"/' run.py\n")
        fout.write("sed -i 's/<FIELD>/"+str(BFIELD)+"/' run.py\n")
        fout.write("sed -i 's/<EVENTS>/"+str(EVENTS)+"/' run.py\n\n")
        fout.write("sed -i 's/<ETAMIN>/"+str(ETAMIN)+"/' run.py\n\n")
        fout.write("sed -i 's/<ETAMAX>/"+str(ETAMAX)+"/' run.py\n\n")
        fout.write("sed -i 's/<CONFIG>/"+str(CONFIG)+"/' run.py\n\n")
        fout.write("sed -i 's/<NFILE>/"+str(N)+"/' run.py\n\n")

        fout.write("source "+FCCSW_DIR+"init.sh\n")
        fout.write(FCCSW_DIR+"run gaudirun.py run.py\n")
        fout.write("echo 'STOP---------------'\n")
        fout.write("echo\n")
        fout.write("cp -v *.root "+path+"/"+dir+"\n")
        fout.write("echo\n")
     os.system("chmod 755 job"+str(N)+".sh")
     
     ###### sends bjobs ######
     os.system("bsub -q "+queue+" job"+str(N)+".sh")
     print "job nr " + str(e) + " " + str(N) +" submitted"
   
   os.chdir(path)
   
print
print "your jobs:"
os.system("bjobs")
print
print 'END'
print
