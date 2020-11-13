FCCSW
=====

Common software for all FCC experiments.
This software is based on Gaudi.



Installation
------------


Log in to lxplus (CentOS7). Please note that these instructions might not work on another CentOS7 machine.

Clone your fork of this repository:

~~~{.sh}
git clone https://github.com/HEP-FCC/FCCSW.git
cd FCCSW
~~~

Set up your environment, as everytime you want to build or use this software:

~~~{.sh}
source ./init.sh
~~~

Compile the FCC software:

~~~{.sh}
make -j 12
~~~

Test
----

~~~{.sh}
./run fccrun Examples/options/pythia.py
~~~

you can also specify some options like number of events and output file
~~~{.sh}
./run fccrun Examples/options/pythia.py --Filename=my_own_output.root -n 1000
~~~

You should see an HepMC printout. More tests can be executed with

~~~{.sh}
make test
~~~


Documentation
----

Please consult [FCCSW documentation](http://hep-fcc.github.io/FCCSW/)
