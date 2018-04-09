FCCSW
=====

Common software for all FCC experiments.
This software is based on Gaudi.


Installation
------------

Log in to lxplus SLC6. Please note that these instructions might not work on another SLC6 machine.

Clone your fork of this repository:

~~~{.sh}
git clone git@github.com:[your-github-username]/FCCSW.git
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
./run fccrun.py Examples/options/example_options.py
~~~

you can also sepcify some options like number of events and output file
~~~{.sh}
./run fccrun.py Examples/options/example_options.py --outputfile=my_own_output.root --nevents=1000
~~~

You should see an HepMC printout. More tests can be executed with

~~~{.sh}
make test
~~~


Documentation
----

Please consult [FCCSW documentation](http://fccsw.web.cern.ch/fccsw/)
