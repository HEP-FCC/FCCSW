#Mon Apr 27 09:55:27 2015"""Automatically generated. DO NOT EDIT please"""
from GaudiKernel.Proxy.Configurable import *

class FCCDataSvc( ConfigurableService ) :
  __slots__ = { 
    'OutputLevel' : 7, # int
    'AuditServices' : False, # bool
    'AuditInitialize' : False, # bool
    'AuditStart' : False, # bool
    'AuditStop' : False, # bool
    'AuditFinalize' : False, # bool
    'AuditReInitialize' : False, # bool
    'AuditReStart' : False, # bool
    'RootCLID' : 110, # int
    'RootName' : '/Event', # str
    'ForceLeaves' : False, # bool
    'InhibitPathes' : [  ], # list
    'DataFaultName' : 'DataFault', # str
    'DataAccessName' : 'DataAccess', # str
    'EnableFaultHandler' : False, # bool
    'EnableAccessHandler' : False, # bool
  }
  _propertyDocDct = { 
  }
  def __init__(self, name = Configurable.DefaultName, **kwargs):
      super(FCCDataSvc, self).__init__(name)
      for n,v in kwargs.items():
         setattr(self, n, v)
  def getDlls( self ):
      return 'FWCorePlugins'
  def getType( self ):
      return 'FCCDataSvc'
  pass # class FCCDataSvc
