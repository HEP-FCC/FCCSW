import argparse
import re

parser = argparse.ArgumentParser()

def add_defaults():
    ## Obligatory arguments
    parser.add_argument("inputFile", help="Input file name for analysis", type = str)
    parser.add_argument("energy", help="Energy of the particle [GeV]", type=int, nargs='+')
    ## Optional arguments
    parser.add_argument('-r', "--regex", help='String to insert in place of \'?\' character in file names ("energy" inserts the values of energies)', type=str, nargs='+')
    parser.add_argument("-o","--output", help="Output file name", type = str)
    parser.add_argument('-v', "--verbose", action='store_true', help='Verbose')

def parse_args():
    global args
    args = parser.parse_args()
    global verbose
    verbose = args.verbose
    global energies
    energies = args.energy
    global filenamesOut
    filenamesOut = []
    filenameOutPattern = args.output
    global filenamesIn
    filenamesIn = []
    filenameInPattern = args.inputFile
    global regex
    regex = args.regex
    if regex:
        filenamesIn, checkIn = substitute(filenameInPattern)
        if filenameOutPattern:
            filenamesOut, checkOut = substitute(filenameOutPattern)
        if not(checkIn):# and not(checkOut):
            print("Character '?' not found in the input file name.")
            print("Either include '?' or do not use '-r/--regex' option")
            exit()
    else:
        ## it is just a filename
        filenamesIn = [filenameInPattern]
        if filenameOutPattern:
            filenamesOut = [filenameOutPattern]

def substitute(name):
    result = []
    pattern = re.compile('\?')
    check = pattern.search(name)
    if check:
        if len(regex) == 1 and regex[0] == 'energy':
            for e in energies:
                result.append( re.sub(pattern,str(e),name) )
        else:
            if len(energies) > 1 and len(energies) != len(regex):
                print("If '-r/--regex' option is used, energy must be either an integer")
                print("or a list of an equal size as the regex list.")
                exit()
            for r in regex:
                result.append( re.sub(pattern,r,name) )
    else:
        result = [name]
    return result, check

def energy(file_number):
    if len(energies) == 1:
        return energies[0]
    else:
        return energies[file_number]

def output(file_number):
    if not(filenamesOut):
        return
    if len(filenamesOut) == 1:
        return filenamesOut[0]
    else:
        return filenamesOut[file_number]

def print_config():
    for name in filenamesIn:
        print("Input file: "+ name)
    if filenamesOut:
        for name in filenamesOut:
            print("Output file: " + name)
    if energies:
        print("Energy of initial particles: " + str(energies) + " GeV")

if __name__ == "__main__":
    add_defaults()
    parse_args()
    print_config()
