FCCSW Documentation
==

In this directory
--
- [C++ Coding Style](./CppCodingStyleGuidelines.md)
- [C++ Best Practices](./GeneralCppGuidelines.md)
- [Basic introduction for GAUDI users](./GAUDIbasics.md)

Doxygen
--
Set up doxygen on lxplus:
```bash
source /afs/cern.ch/sw/lcg/releases/LCG_80/doxygen/1.8.9.1/x86_64-slc6-gcc49-opt/doxygen-env.sh
```

Produce doxygen documents (after setting up your FCCSW environment with `init.(c)sh`)
```bash
cd $FCCSW
make run-doxygen
```

CERN TWiki Documentation
--
[FCCSW Main TWiki](https://twiki.cern.ch/twiki/bin/view/FCC/FccSoftware)
