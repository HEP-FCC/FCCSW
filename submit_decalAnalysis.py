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
CONFIG="50Layers_2.1mmW_50umPixels_18umThick"
FCCSW_VERSION="FCCSW0.8"
ENERGIES = ["0"]#"10","20","50","100","200","300","400","500","1000"]
#ENERGIES = ["200","300","400","500","1000" ] # number of jobs to be submitted
#ENERGIES = ["30","40","60","70","80","90"] # number of jobs to be submitted
#ENERGIES = ["1000" ] # number of jobs to be submitted
BFIELD = "4"
EVENTS = 10000
ETAMIN = -0.001
ETAMAX = 0.001
NRUNS = 400
FCCSW_DIR = "/afs/cern.ch/user/t/toprice/private/FCC/FCCSW/"
queue = "8nh"
########   customization end   #########

path = os.getcwd()

##### loop for creating and sending jobs #####
#e = 100
CONFIGS = os.listdir(FCCSW_DIR+"/batch_eos/")
CONFIGS = [ c for c in CONFIGS if CONFIG in c ]
#for e in ENERGIES:
for CONFIG in CONFIGS:
   print CONFIG
   for e in ENERGIES:
     ##### creates directory and file list for job #######
     RUNCONFIG=str(e)+"GeV_BFIELD"+str(BFIELD)+"T_ETAMIN"+str(ETAMIN)+"_ETAMAX"+str(ETAMAX)
   #  dir="batch_eos/"+CONFIG+"_"+FCCSW_VERSION+"/"+RUNCONFIG
     dir="batch_eos/"+CONFIG+"/"+RUNCONFIG
     print dir

     if os.path.isdir(dir):

       files = [ f for f in os.listdir(dir) if "output" in f and "500" in f] 
       print files

       os.chdir(dir)
      
       for f in files:
          #print "hist_"+f[7:]
          #os.chdir(dir)
          #os.system('pwd')
          #os.system('cp -v '+FCCSW_DIR+'/Detector/DetStudies/tests/options/longitudinalAnalysis_DEcal_batch.py run.py')
          #os.system("sed -i 's/<DETCONFIG>/"+str(CONFIG)+"/' run.py")
          #os.system("sed -i 's/<RUNCONFIG>/"+str(RUNCONFIG)+"/' run.py")
          #os.system("sed -i 's/<FILE>/"+f+"/' run.py")
          #os.system("sed -i 's/<OUTPUT>/hist_"+f[7:]+"/' run.py")
          #os.chdir(path)
          #os.system("./run gaudirun.py "+dir+"/run.py")
          N = f[f.find("4T")+3:f.find(".root")]
          if len(N) == 0:
            N = 0
          print f
          
          with open('analyse'+str(N)+'.sh', 'w') as fout:
            fout.write("#!/bin/sh\n")
            fout.write("echo\n")
            fout.write("echo 'START---------------'\n")
            fout.write("echo 'WORKDIR ' ${PWD}\n")

            fout.write('cp -v '+FCCSW_DIR+'/Detector/DetStudies/tests/options/decalAnalysis_DEcal_batch.py ana.py\n')
            fout.write("sed -i 's/<DETCONFIG>/"+str(CONFIG)+"/' ana.py\n")
            fout.write("sed -i 's/<RUNCONFIG>/"+str(RUNCONFIG)+"/' ana.py\n")
            fout.write("sed -i 's/<FILE>/"+f+"/' ana.py\n")
            fout.write("sed -i 's/<OUTPUT>/analysis_"+f[7:]+"/' ana.py\n")
            fout.write("echo\n")
            fout.write("cat ana.py\n")
            fout.write("echo\n")
            fout.write("source "+FCCSW_DIR+"init.sh\n")
            fout.write(FCCSW_DIR+"run gaudirun.py ana.py\n")
            fout.write("echo 'STOP---------------'\n")
            fout.write("echo\n")
            fout.write("cp -v *.root "+path+"/"+dir+"\n")
            fout.write("echo\n")


          os.system("chmod 755 analyse"+str(N)+".sh")
          os.system("bsub -q "+queue+" analyse"+str(N)+".sh")
          
     os.chdir(path)
     

