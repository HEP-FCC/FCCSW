#Wed Aug  5 12:21:01 2015"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.DataObjectDescriptor import *
from GaudiKernel.Proxy.Configurable import *

class CompareMaterial( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'Timeline' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'IsClonable' : False, # bool
    'Cardinality' : 1, # int
    'NeededResources' : [  ], # list
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(CompareMaterial, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TestAlgorithms'
  def getType( self ):
      return 'CompareMaterial'
  pass # class CompareMaterial

class Geant4Test( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'Timeline' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'IsClonable' : False, # bool
    'Cardinality' : 1, # int
    'NeededResources' : [  ], # list
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(Geant4Test, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TestAlgorithms'
  def getType( self ):
      return 'Geant4Test'
  pass # class Geant4Test

class RecoGeoTest( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'Timeline' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'IsClonable' : False, # bool
    'Cardinality' : 1, # int
    'NeededResources' : [  ], # list
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(RecoGeoTest, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TestAlgorithms'
  def getType( self ):
      return 'RecoGeoTest'
  pass # class RecoGeoTest

class RecoGeoWrite( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection("particles|out|0|0"), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'Timeline' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'IsClonable' : False, # bool
    'Cardinality' : 1, # int
    'NeededResources' : [  ], # list
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(RecoGeoWrite, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TestAlgorithms'
  def getType( self ):
      return 'RecoGeoWrite'
  pass # class RecoGeoWrite

class SurfaceTest( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'Timeline' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'IsClonable' : False, # bool
    'Cardinality' : 1, # int
    'NeededResources' : [  ], # list
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(SurfaceTest, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TestAlgorithms'
  def getType( self ):
      return 'SurfaceTest'
  pass # class SurfaceTest

class TestAlgorithm( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection(""), # DataDescriptorCollection
    'AuditAlgorithms' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditReinitialize' : False, # bool
    'AuditRestart' : False, # bool
    'AuditExecute' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditBeginRun' : False, # bool
    'AuditEndRun' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'Timeline' : False, # bool
    'MonitorService' : 'MonitorSvc', # str
    'RegisterForContextService' : True, # bool
    'IsClonable' : False, # bool
    'Cardinality' : 1, # int
    'NeededResources' : [  ], # list
    'ErrorsPrint' : True, # bool
    'PropertiesPrint' : False, # bool
    'StatPrint' : True, # bool
    'TypePrint' : True, # bool
    'Context' : '', # str
    'RootInTES' : '', # str
    'RootOnTES' : '', # str
    'GlobalTimeOffset' : 0.0000000, # float
    'StatTableHeader' : ' |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |', # str
    'RegularRowFormat' : ' | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |', # str
    'EfficiencyRowFormat' : ' |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |', # str
    'UseEfficiencyRowFormat' : True, # bool
    'CounterList' : [ '.*' ], # list
    'StatEntityList' : [  ], # list
    'VetoObjects' : [  ], # list
    'RequireObjects' : [  ], # list
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(TestAlgorithm, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'TestAlgorithms'
  def getType( self ):
      return 'TestAlgorithm'
  pass # class TestAlgorithm
