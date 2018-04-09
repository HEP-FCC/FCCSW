from __future__ import print_function
import argparse
import ROOT

parser = argparse.ArgumentParser(description='Job Options Dumper')
parser.add_argument( dest='fname', type=str, help="name of file to read")


def dump_joboptions(filename):
  """ Simple and straightforward dump of the output of Gaudi's JobOptionsSvc
  to stdout.
  
  Parameters
  ----------

  filename: str
      The name of the FCCSW output file containing joboptions to print
  """
  f = ROOT.TFile(filename)
  t = f.metadata
  for event in t:
      s =  event.gaudiConfigOptions
      for e in s:
          print(e)

if __name__ == "__main__":
    args = parser.parse_args()
    dump_joboptions(args.fname)

  
