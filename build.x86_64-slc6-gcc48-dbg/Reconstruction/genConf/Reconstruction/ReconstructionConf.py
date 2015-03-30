#Mon Mar 30 14:02:54 2015"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.DataObjectDescriptor import *
from GaudiKernel.Proxy.Configurable import *

class HepMCJetClustering( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection("hepmc|NULL|0|0"), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection("jets|NULL|0|1"), # DataDescriptorCollection
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
    'JetAlgorithm' : 'kt', # str
    'ConeRadius' : 1.00000, # float
    'RecominbationScheme' : 'E', # str
    'InclusiveJets' : True, # bool
    'PtMin' : 0.00000, # float
    'Dcut' : -1.00000, # float
    'NJets' : -1, # int
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'JetAlgorithm' : """ the Jet Algorithm to use [kt, antikt, cambridge] """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'ConeRadius' : """ cone radius """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'NJets' : """ Number of jets for exclusive jets """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'RecominbationScheme' : """ the Recombination Scheme to use [E, pt, et] """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'InclusiveJets' : """ use inclusive or exclusive jets """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'Dcut' : """ dcut for exclusive jets """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'PtMin' : """ Minimum pT of jets for inclusiveJets """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(HepMCJetClustering, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'Reconstruction'
  def getType( self ):
      return 'HepMCJetClustering'
  pass # class HepMCJetClustering

class JetClustering_MCParticleCollection_GenJetCollection_GenJetParticleAssociationCollection_( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection("particles|NULL|0|0"), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection("constituents|NULL|0|1#jets|NULL|0|1"), # DataDescriptorCollection
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
    'jetAlgorithm' : 'antikt', # str
    'coneRadius' : 0.500000, # float
    'recominbationScheme' : 'E', # str
    'inclusiveJets' : True, # bool
    'ptMin' : 10.0000, # float
    'dcut' : -1.00000, # float
    'nJets' : -1, # int
    'verbose' : False, # bool
    'areaType' : 'none', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'nJets' : """ Number of jets for exclusive jets """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'jetAlgorithm' : """ the Jet Algorithm to use [kt, antikt, cambridge] """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'dcut' : """ dcut for exclusive jets """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'areaType' : """ Area type [none, active, passive] """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'recominbationScheme' : """ the Recombination Scheme to use [E, pt, et] """,
    'coneRadius' : """ cone radius """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'inclusiveJets' : """ use inclusive or exclusive jets """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'ptMin' : """ Minimum pT of jets for inclusiveJets """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'verbose' : """ Boolean flag for verbosity """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(JetClustering_MCParticleCollection_GenJetCollection_GenJetParticleAssociationCollection_, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'Reconstruction'
  def getType( self ):
      return 'JetClustering<MCParticleCollection, GenJetCollection, GenJetParticleAssociationCollection>'
  pass # class JetClustering_MCParticleCollection_GenJetCollection_GenJetParticleAssociationCollection_

class JetClustering_ParticleCollection_JetCollection_JetParticleAssociationCollection_( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection("particles|NULL|0|0"), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection("constituents|NULL|0|1#jets|NULL|0|1"), # DataDescriptorCollection
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
    'jetAlgorithm' : 'antikt', # str
    'coneRadius' : 0.500000, # float
    'recominbationScheme' : 'E', # str
    'inclusiveJets' : True, # bool
    'ptMin' : 10.0000, # float
    'dcut' : -1.00000, # float
    'nJets' : -1, # int
    'verbose' : False, # bool
    'areaType' : 'none', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'nJets' : """ Number of jets for exclusive jets """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'jetAlgorithm' : """ the Jet Algorithm to use [kt, antikt, cambridge] """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'dcut' : """ dcut for exclusive jets """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'areaType' : """ Area type [none, active, passive] """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'recominbationScheme' : """ the Recombination Scheme to use [E, pt, et] """,
    'coneRadius' : """ cone radius """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'inclusiveJets' : """ use inclusive or exclusive jets """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'ptMin' : """ Minimum pT of jets for inclusiveJets """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'verbose' : """ Boolean flag for verbosity """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(JetClustering_ParticleCollection_JetCollection_JetParticleAssociationCollection_, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'Reconstruction'
  def getType( self ):
      return 'JetClustering<ParticleCollection, JetCollection, JetParticleAssociationCollection>'
  pass # class JetClustering_ParticleCollection_JetCollection_JetParticleAssociationCollection_

class JetHistograms( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection("jets|NULL|0|0"), # DataDescriptorCollection
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
      super(JetHistograms, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'Reconstruction'
  def getType( self ):
      return 'JetHistograms'
  pass # class JetHistograms

class MCParticleGenJetClustering( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection("particles|NULL|0|0"), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection("constituents|NULL|0|1#jets|NULL|0|1"), # DataDescriptorCollection
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
    'jetAlgorithm' : 'antikt', # str
    'coneRadius' : 0.500000, # float
    'recominbationScheme' : 'E', # str
    'inclusiveJets' : True, # bool
    'ptMin' : 10.0000, # float
    'dcut' : -1.00000, # float
    'nJets' : -1, # int
    'verbose' : False, # bool
    'areaType' : 'none', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'nJets' : """ Number of jets for exclusive jets """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'jetAlgorithm' : """ the Jet Algorithm to use [kt, antikt, cambridge] """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'dcut' : """ dcut for exclusive jets """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'areaType' : """ Area type [none, active, passive] """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'recominbationScheme' : """ the Recombination Scheme to use [E, pt, et] """,
    'coneRadius' : """ cone radius """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'inclusiveJets' : """ use inclusive or exclusive jets """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'ptMin' : """ Minimum pT of jets for inclusiveJets """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'verbose' : """ Boolean flag for verbosity """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(MCParticleGenJetClustering, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'Reconstruction'
  def getType( self ):
      return 'MCParticleGenJetClustering'
  pass # class MCParticleGenJetClustering

class ParticleJetClustering( ConfigurableAlgorithm ) :
  __slots__ = { 
    'OutputLevel' : 0, # int
    'Enable' : True, # bool
    'ErrorMax' : 1, # int
    'ErrorCount' : 0, # int
    'Inputs' : DataObjectDescriptorCollection("particles|NULL|0|0"), # DataDescriptorCollection
    'Outputs' : DataObjectDescriptorCollection("constituents|NULL|0|1#jets|NULL|0|1"), # DataDescriptorCollection
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
    'jetAlgorithm' : 'antikt', # str
    'coneRadius' : 0.500000, # float
    'recominbationScheme' : 'E', # str
    'inclusiveJets' : True, # bool
    'ptMin' : 10.0000, # float
    'dcut' : -1.00000, # float
    'nJets' : -1, # int
    'verbose' : False, # bool
    'areaType' : 'none', # str
  }
  _propertyDocDct = { 
    'StatTableHeader' : """ The header row for the output Stat-table """,
    'nJets' : """ Number of jets for exclusive jets """,
    'ErrorsPrint' : """ Print the statistics of errors/warnings/exceptions """,
    'RegisterForContextService' : """ The flag to enforce the registration for Algorithm Context Service """,
    'StatPrint' : """ Print the table of counters """,
    'jetAlgorithm' : """ the Jet Algorithm to use [kt, antikt, cambridge] """,
    'VetoObjects' : """ Skip execute if one or more of these TES objects exists """,
    'TypePrint' : """ Add the actal C++ component type into the messages """,
    'CounterList' : """ RegEx list, of simple integer counters for CounterSummary. """,
    'dcut' : """ dcut for exclusive jets """,
    'EfficiencyRowFormat' : """ The format for the regular row in the output Stat-table """,
    'areaType' : """ Area type [none, active, passive] """,
    'RequireObjects' : """ Execute only if one or more of these TES objects exists """,
    'recominbationScheme' : """ the Recombination Scheme to use [E, pt, et] """,
    'coneRadius' : """ cone radius """,
    'PropertiesPrint' : """ Print the properties of the component  """,
    'UseEfficiencyRowFormat' : """ Use the special format for printout of efficiency counters """,
    'inclusiveJets' : """ use inclusive or exclusive jets """,
    'RegularRowFormat' : """ The format for the regular row in the output Stat-table """,
    'ptMin' : """ Minimum pT of jets for inclusiveJets """,
    'StatEntityList' : """ RegEx list, of StatEntity counters for CounterSummary. """,
    'verbose' : """ Boolean flag for verbosity """,
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(ParticleJetClustering, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'Reconstruction'
  def getType( self ):
      return 'ParticleJetClustering'
  pass # class ParticleJetClustering
