Testing in FCCSW / Gaudi
==

The Gaudi testing infrastructure is based on CTest and QMTest. This document is meant as a short overview on how to use the CTest facilities in FCCSW.

Some additional / orthogonal information may be found on the [Gaudi Test TWiki](https://twiki.cern.ch/twiki/bin/view/Gaudi/GaudiTestingInfrastructure).

Folder structure
--

We assume that in your package you have a folder structure like this:

- options
- tests
  - options : Gaudi python configurations
  - scripts : python scripts

**options**
  Your normal python configurations for normal Gaudi jobs within your package.

**tests/options**
  Your Gaudi python configurations that are only used in tests.

**tests/scripts**
  Any python script that checks e.g. job output


Testing configurations
--

Testing whether a configuration does not fail (or does fail) can be done by adding a test with `gaudi_add_test` in your package `CMakeLists.txt`:

~~~{.cmake}
gaudi_add_test(<name>
               [FRAMEWORK options1 options2]
               [WORKING_DIRECTORY dir]
               [ENVIRONMENT variable[+]=value ...]
               [DEPENDS other_test ...]
               [FAILS] [PASSREGEX regex] [FAILREGEX regex]
               [TIMEOUT seconds]
               [LABELS label1 label2 ...])
~~~

Declare a run-time test in the subdirectory. `FRAMEWORK` - run a job with the specified options, e.g. `FRAMEWORK test/options/myexample.py`.

If special environment settings are needed, they can be specified in the section `ENVIRONMENT` as `<var>=<value>` or `<var>+=<value>`, where the second format
prepends the value to the PATH-like variable.

The `WORKING_DIRECTORY` option can be passed if the command needs to be run from a fixed directory (ignored by QMTEST tests). This should be used if the job needs paths that are relative. By default jobs are run from the build directory.

Great flexibility is given by the following options:

- `FAILS` - the tests succeds if the command fails (return code !=0)
- `DEPENDS` - ensures an order of execution of tests (e.g. do not run a read test if the write one failed)
- `PASSREGEX` - Specify a regexp; if matched in the output the test is successful
- `FAILREGEX` - Specify a regexp; if matched in the output the test is failed
- `LABELS` - list of labels to add to the test (for categorization)

Defining a test with generic applications
--

CTest also allows to run any other command as a test, e.g. python or compiled standalone macros. This allows to use assertions to ensure whether some output is according to specifications. Obviously, if for example the output of a Gaudi / FCCSW job should be checked, the test should depend on that test (declared with `DEPENDS`).

~~~{.cmake}
gaudi_add_test(<name>
               [COMMAND cmd args]
               [WORKING_DIRECTORY dir]
               [ENVIRONMENT variable[+]=value ...]
               [DEPENDS other_test ...]
               [FAILS] [PASSREGEX regex] [FAILREGEX regex]
               [TIMEOUT seconds]
               [LABELS label1 label2 ...])
~~~

The `COMMAND cmd args` allows to add any executable command, e.g. `COMMAND python myscript.py`.

Example
--

This example and more can be found within the `Examples` folder.

First run a configuration that generates some output `output.root` which we add as a first test:

~~~{.cmake}
gaudi_add_test(DummyGen
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/tests #so we know where the output is
               FRAMEWORK tests/options/examplejob.py)
~~~

Then we have a python script that checks the size of a collection:

~~~{.py}
import ROOT

rfile = ROOT.TFile.Open("./output.root", "READ")
events = rfile.Get("events")

assert(events.GetEntries() == 10)
~~~

Which we add to the CMakeLists as the second test:

~~~{.cmake}
gaudi_add_test(DummyGenCheck
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Examples/tests  # so we know where to get output
               COMMAND python ./scripts/dummy_gen_test.py
               DEPENDS DummyGen)                                       # as we need the output
~~~

Running tests
--

All tests defined in the project can be run with

~~~{.sh}
  make test
~~~

Any commandline option for cmake can be passed through the Makefile. This allows, for example, to exclude tests or only run a sub-set of tests:

~~~{.sh}
  make test ARGS="-E Example" # Skip any test that contains Example in it's name
  make test ARGS="-R Example" # Only run tests that contain Example in their names
~~~

If tests fail, they do so silently by default. You can activate the output by setting the environment variable `CTEST_OUTPUT_ON_FAILURE` or do:

~~~{.sh}
  make test ARGS="--output-on-failure"
~~~
