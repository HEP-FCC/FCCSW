Very basic introduction to GAUDI
====

GAUDI application framework provides 'blocks' of software that can interact with each other. Those 'blocks' (GAUDI components) are: tools, algorithms and services. Each of them has its own interface that defines the basic functionality.
User may specify which 'blocks' (and how) should be created in a job configuration file written in python.
Basic usage of GAUDI application can be limited to only editing that job configuration file.

### Properties

All components may contain data members which can be initialised from the job configuration file. Those variables are called *properties* and need to be declared in the constructor. For instance that may be `int`, `double`, `std::string`, but also a `ToolHandle` or `std::vector<std::string>`.
A property may be set in a python configuration either when a component is created

~~~{.py}
# algorithm HepMCReader with a property "Filename"
# in HepMCReader class declaration:
#                  std::string m_filename;
# in HepMCReader constructor:
#                  declareProperty("Filename", m_filename="");
# in python configuration file:
reader = HepMCReader("Reader", Filename="example_MyPythia.dat")
~~~

or afterwards (except for ToolHandle - TODO CHECK)

~~~{.py}
# algorithm PodioOutput with a property "outputCommands"
# in PodioOutput class declaration:
#                  std::vector<std::string> m_outputCommands;
# in PodioOutput constructor:
#                  declareProperty("outputCommands", m_outputCommands=defaultCommands);
# in python configuration file:
out.outputCommands = ["keep *"]
~~~

### Data inputs and outputs


In particular, both tools and algorithms may have data input and output (`DataHandle<T>`) specified. They are declared in the constructor as well.

~~~{.py}
# algorithm HepMCReader with output "hepmc"
# in HepMCReader class declaration:
#                  DataHandle<HepMC::GenEvent> m_hepmchandle;
# in HepMCReader constructor:
#                  declareOutput("hepmc", m_hepmchandle);
reader.DataOutputs.hepmc.Path = "hepmcevent"
~~~

Path "hepmc" is further an input to the next algorithm, which is an algorithm `HepMCConverter`
(Note that common name `hepmc` in those algorithms is just a coincidence. What matters is the path specified in python file).

~~~{.py}
# algorithm HepMCReader with a property "outputCommands"
# in HepMCConverter class declaration:
#                  DataHandle<HepMC::GenEvent> m_hepmchandle;
# in HepMCConverter constructor:
#                  declareInput("hepmc", m_hepmchandle);
hepmc_converter.DataInputs.hepmc.Path="hepmcevent"
~~~
### Running Gaudi

FCCSW provides an executable `./run` that is responsible for setting the correct environment.
Any command may be run in the FCCSW environment by passing it as argument to `./run`, e.g. `./run echo $LD_LIBRARY_PATH`,
which should give a printout including the library paths of your FCCSW install.
Gaudi provides a script `gaudirun.py` to parse the users python job configuration file, create the Gaudi `AppMgr` and run the configured algorithms, which can be done by entering `./run gaudirun.py config/simple_workflow.py`.

____
### For Developers: Running Gaudi from the interpreter

During development and when performance is not critical, the flexibility and interactivity of interpreters such as `ipython` can be very helpful. For this purpose, Gaudi provides `GaudiPython`: Python-bindings to its C++ classes. Because of the C++ nature of Gaudi, these bindings are ['pythonic'](https://www.python.org/dev/peps/pep-0020/) only to a degree, but can nevertheless be useful.
The job configuration file may be used with GaudiPython by simply executing it before `AppMgr` is constructed:

```{.py}
./run python -i config/simple_workflow.py
>>> from GaudiPython import AppMgr #
>>> gaudi = AppMgr()
>>> gaudi.run(1)
```
This way offers some limited possibilities to inspect the transient event store between events by calling

```{.py}
>>> evt = gaudi.evtSvc()
>>> evt.dump()
>>> unreadableDataObject = evt['/Event/hepmc']
```

It might be possible to further process the DataObject returned from the event service by loading its dictionary (TODO).

`GaudiPython` also includes a baseclass `PyAlgorithm` for Algorithms written in python, that can be useful for prototyping. As a simple example, the following code can be
inserted before the call to gaudi.run (which implies gaudi.initialize).

```{.py}
class MyPyAlg(PyAlgorithm):
  def initialize(self):
    print 'Initializing User Analysis...'
  def execute(self):
    print 'Executing User Analysis...'
  def finalize(self):
    print 'Finalizing User Analysis...'

mypythonalg = MyPyAlg('MyPyAlg')
gaudi.addAlgorithm('MyPyAlg')
```


