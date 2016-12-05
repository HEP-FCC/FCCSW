import argparse

def parse_standard_job_options(scriptname=""):
    """
    Returns the parsed arguments, adding a parser with commonly needed opts:
    - args.nevents      -- number of events (int), specify with --nevents
    - args.inputfile    -- the input file (string), specify with --inputfile
    - args.outputfile   -- the output file (string), specify with --outputfile
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('--inputfile', type=str, default='', help='specify an input file')
    parser.add_argument('--outputfile', type=str, default='', help='specify an output file')
    parser.add_argument('--nevents', type=int, default=None, help='specify number of events to process')
    args, _ = parser.parse_known_args()
    return args
