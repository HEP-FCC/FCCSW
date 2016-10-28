#!/usr/bin/env python
#
# Simple script that allows to specify key parameters such as input file, 
# output file and number of events via command line.
# Executes "./run gaudirun.py PythiaDelphes_config.py" with updated parameters.
#  
# Usage: 
#
# runPy8DelphesFCC.py [-h] [-cfg CONFIGURATION] [-i INPUT] [-o OUTPUT] [-n NEVENTS]
#
# Optional arguments:
#
#   -h,  --help            show this help message and exit
#   -i , --input           input file Pythia8 cmd file
#   -o , --output          output ROOT file
#   -n , --nevents         number of events to process

import os, argparse

def updateLine(line, mapping):
  newline = line
  for key, value in mapping.iteritems():
    if line.startswith('#'):
      continue
    splits = line.split('=')
    if(key in splits[0]):
      if type(value) is str:
        newline = '{} = "{}"\n'.format(key,value)
      elif type(value) is int:
        newline = '{} = {}\n'.format(key,value)
  return newline



def create_parser(): 
  parser = argparse.ArgumentParser()
  parser.add_argument("-i", "--input", type=str, help="input file Pythia8 cmd file")
  parser.add_argument("-o", "--output", type=str, help="output ROOT file")
  parser.add_argument("-n", "--nevents", type=int, help="number of events to process")
  return parser



def main(parser):
  options  = parser.parse_args()
  
  fccConfigFile = 'Sim/SimDelphesInterface/options/PythiaDelphes_config.py'
  tmpConfigFile = 'tmp.py'
  
  os.system('cp '+fccConfigFile+' '+tmpConfigFile)
  
  replacements = {
 		  'pythiaConfFile':options.input, 
 		  'out.filename':options.output, 
		  'nEvents':options.nevents
		 }

  with open(tmpConfigFile, 'w') as new_file:
    with open(fccConfigFile) as old_file:
      for line in old_file:
        new_line = updateLine(line, replacements)
        new_file.write(new_line)
      
  os.system('./run gaudirun.py '+tmpConfigFile)
  os.system('rm tmp.py')
  

if __name__ == '__main__':
    parser = create_parser()
    main(parser)
