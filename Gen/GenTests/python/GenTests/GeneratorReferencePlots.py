#################################################################################
# Usage:
#  python GeneratorReferencePlots.py 
#  Exemple: 
#  python GeneratorReferencePlots.py -v -r GaussHistos_REF_30000000.root -f GaussOutput.txt -l GaussOutput.txt -s GaussHistos_24142001.root -i
#################################################################################

from ROOT import TFile, TCanvas, TH1D
from ROOT import gDirectory, gPad, gStyle
from optparse import OptionParser
import re, sys, os, shutil

#################################################################################
def grepPattern(P,L):
  result = None
  resultobject = re.search( P , L )
  if ( resultobject != None ):
    result = resultobject.group(1)
  return result

#################################################################################
class GeneratorLogFile:
  def __init__(self,N):
    self.fileName = N
    self.GaussVersion = None
    self.PythiaVersion = None
    self.EventType = None
    self.TotalCrossSection = None
    self.TotalInteractions = None
    self.TotalIntWithB = None 
    self.TotalIntWithD = None
    self.TotalIntWithPromptCharm = None
    self.TotalAcceptedEvents = None
    self.TotalSignalProcessEvents = None
    self.TotalSignalProcessFromBEvents = None
    self.TotalZInvertedEvents = None
    self.TotalEventsAfterCut = None
    self.TotalTime = None
    self.CPUSpeed = None

  def pythiaSerie(self):
    pythiaSerie = 0
    if (self.PythiaVersion != None):
      pythiaSerie = int(self.PythiaVersion[0])
    return pythiaSerie

  def isPythia8(self):
    if (self.pythiaSerie()==8):
      return True
    else:
      return False

  def isPythia6(self):
    if (self.pythiaSerie()==6):
      return True
    else:
      return False
    
  def computeQuantities(self):
    f = open(self.fileName)
    for line in f:
      if ( self.EventType == None ):
        self.EventType = grepPattern('Requested to generate EventType (\d+)',line)
      if ( self.GaussVersion == None ):
        self.GaussVersion = grepPattern( 'Welcome to Gauss version (\S+)' , line )
      if ( self.PythiaVersion == None ):
        self.PythiaVersion = grepPattern( 'This is PYTHIA version (\S+)' , line )
        if ( self.PythiaVersion != None ):
          print self.PythiaVersion, ' ', self.PythiaVersion[0]
      if ( self.TotalCrossSection == None):
        if (self.isPythia8()):
           self.TotalCrossSection = grepPattern( '\| sum *\| *\d+ *\d+ *\d+ \| *(\S+)' , line )
        else :
          self.TotalCrossSection = grepPattern( 'All included subprocesses *I *\d+ *\d+ I *(\S+)' , line )
          if (self.TotalCrossSection != None):
            if ('D' in self.TotalCrossSection):
              self.TotalCrossSection = self.TotalCrossSection.replace('D', 'E')
      if ( self.TotalInteractions == None ):
        self.TotalInteractions = grepPattern( 'Number of generated interactions : (\d+)' , line )
      if ( self.TotalIntWithB == None ):
        self.TotalIntWithB = grepPattern( 'Number of generated interactions with >= 1b : (\d+)' , line ) 
      if ( self.TotalIntWithD == None ):
        self.TotalIntWithD = grepPattern( 'Number of generated interactions with >= 1c : (\d+)' , line ) 
      if ( self.TotalIntWithPromptCharm == None):
        self.TotalIntWithPromptCharm = grepPattern( 'Number of generated interactions with >= prompt C : (\d+)' , line ) 
      if ( self.TotalAcceptedEvents == None ):
        self.TotalAcceptedEvents = grepPattern( 'Number of accepted events : (\d+)' , line )
      if ( self.TotalSignalProcessEvents == None ):
        self.TotalSignalProcessEvents = grepPattern( 'Number of events for generator level cut, before : (\d+)' , line)
      if ( self.TotalSignalProcessFromBEvents == None ):
        self.TotalSignalProcessFromBEvents = grepPattern( 'Number of accepted interactions with >= 1b : (\d+)' , line )
      if ( self.TotalZInvertedEvents == None ):
        self.TotalZInvertedEvents = grepPattern( 'Number of z-inverted events : (\d+)' , line )
      if ( self.TotalEventsAfterCut == None ):
        self.TotalEventsAfterCut = grepPattern( 'Number of events for generator level cut, before : \d+, after : (\d+)' , line )
      if (self.CPUSpeed == None ):
        self.CPUSpeed = grepPattern( 'TimingAuditor.TIMER *INFO This machine has a speed about *(\S+)' , line )
      if ( self.TotalTime == None ):
        self.TotalTime = grepPattern( 'TimingAuditor.TIMER *INFO EVENT LOOP *\| *\d+\.\d+ *\| *(\S+)', line )

    f.close()
    
  def eventType(self):
    return self.EventType
  def gaussVersion(self):
    return self.GaussVersion
  def pythiaVersion(self):
    return self.PythiaVersion
  def totalCrossSection(self):
  #### This is the total cross-section printed by Pythia
    return float(self.TotalCrossSection)
  def bCrossSection(self):
  #### b quark or B hadron without b quark from production vertex
    return float( float(self.TotalCrossSection) * int(self.TotalIntWithB) / int(self.TotalInteractions))
  def cCrossSection(self):
  #### c quark or D hadron without c quark from production vertex
    return float( float(self.TotalCrossSection) * int(self.TotalIntWithD) / int(self.TotalInteractions))
  def promptCharmCrossSection(self):
  #### D hadron (like J/psi but also chi_c) without B hadron or c quark      
    return float( float(self.TotalCrossSection) * int(self.TotalIntWithPromptCharm) / int(self.TotalInteractions))
  def totalAcceptedEvents(self):
    return int(self.TotalAcceptedEvents)
  def signalProcessCrossSection(self):
  #### valid for J/psi (in general for all generation without CP mixture) 
    if (self.TotalSignalProcessEvents==None):
      return 0
    return float( float(self.TotalCrossSection) * int(self.TotalSignalProcessEvents) / int(self.TotalInteractions))
  def signalProcessFromBCrossSection(self):
  #### valid for J/psi (in general for all generation without CP mixture)
    return float( float(self.TotalCrossSection) * int(self.TotalSignalProcessFromBEvents) / int(self.TotalInteractions))
  def generatorLevelCutEfficiency(self):
    if ( self.TotalEventsAfterCut == None or self.TotalZInvertedEvents == None or self.TotalSignalProcessEvents == None ):
      return 0
    return float( ( int(self.TotalEventsAfterCut) + int(self.TotalZInvertedEvents) ) / float( self.TotalSignalProcessEvents) )
  def timePerEvent( self ):
    return float(float(self.TotalTime) * float(self.CPUSpeed) )
     
#################################################################################  
class GeneratorHisto:
  def __init__(self,c,RH,OH,XT,YT,HT,FN,LS):
    self.canvas = c
    self.referenceHisto = RH
    self.compHisto = OH
    self.XTitle = XT 
    self.YTitle = YT
    self.HistTitle = HT
    self.FileName = FN
    self.LogScale = LS
    
  def title(self):
    return self.HistTitle
  def refFileName(self):
    return "Reference"+self.FileName+".eps"
  def fileName(self):
    return self.FileName+".eps"
  def compFileName(self):
    return "Comp"+self.FileName+".jpg"    
    
  def plot(self):
    self.referenceHisto.GetXaxis().SetTitle( self.XTitle) 
    self.referenceHisto.GetXaxis().SetLabelSize( 0.025 ) 
    self.referenceHisto.GetYaxis().SetTitle( self.YTitle ) 
    self.referenceHisto.GetYaxis().SetLabelSize( 0.025 ) 
    self.referenceHisto.SetMarkerStyle(20)
    self.referenceHisto.SetMarkerColor(2)
    self.referenceHisto.SetMarkerSize(1.0)
    self.referenceHisto.SetTitle(self.HistTitle)
    if self.LogScale:
      gPad.SetLogy()
    else:
      gPad.SetLogy(0)
    self.referenceHisto.DrawNormalized('') 
    self.canvas.Update()
    self.canvas.Print(self.refFileName())  
#
    self.compHisto.GetXaxis().SetTitle( self.XTitle) 
    self.compHisto.GetXaxis().SetLabelSize( 0.025 ) 
    self.compHisto.GetYaxis().SetTitle( self.YTitle ) 
    self.compHisto.GetYaxis().SetLabelSize( 0.025 ) 
    self.compHisto.SetMarkerStyle(21)
    self.compHisto.SetMarkerColor(4)
    self.compHisto.SetMarkerSize(1.0)
    self.compHisto.SetTitle(self.HistTitle)
    if self.LogScale:
      gPad.SetLogy()
    else:
      gPad.SetLogy(0)
    self.compHisto.DrawNormalized('') 
    self.canvas.Update()
    self.canvas.Print(self.fileName())            
#
    self.referenceHisto.DrawNormalized('SAME')
    self.canvas.Update()
    self.canvas.Print(self.compFileName())     
    
#####################################################################
class GeneratorWebPage:
  def __init__(self,N):
    self.name = N
    self.listOfQuantities = []
    self.listOfPlots = []
    self.basicQuantities = dict()
    self.basicRefQuantities = dict()
    self.refPlots = dict()
    self.plots = dict()
    self.supPlots = dict()
  def setVersions(self,V,RV,P,RPV):
    self.version = V
    if V == None:
      self.version = 'v31r1'
    self.refVersion = RV
    if RV == None:
      self.refVersion = 'v26r1'
    self.pythiaVersion = P 
    if P == None:
      self.pythiaVersion = '6.424.2'
    self.refPythiaVersion = RPV
    if RPV == None:
      self.refPythiaVersion = '6.418.2'
  def setEventType(self,E):
    self.eventType = E
  def addQuantity(self,N,R,C):
    self.listOfQuantities.append(N)
    self.basicQuantities[N]=C
    self.basicRefQuantities[N]=R
  def addPlot(self,Plot):
    self.listOfPlots.append(Plot.title())
    self.refPlots[Plot.title()] = Plot.title()
    self.plots[Plot.title()] = Plot.fileName()
    self.refPlots[Plot.title()] = Plot.refFileName()
    self.supPlots[Plot.title()] = Plot.compFileName()
  def create(self):
    f = open(self.name,'w')
    f.write("<HTML>\n")
    f.write("<HEAD>\n")
#    f.write("<link rel=\"icon\" type=\"image/jpg\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/images/lhcblogo.gif\">\n")
    f.write("<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/css/lhcb.css\" type=\"text/css\" media=\"screen\">\n")
#    f.write("<link rel=\"stylesheet\" href=\"lhcb.css\" type=\"text/css\" media=\"screen\">\n")
    f.write("<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/css/css.css\" type=\"text/css\" media=\"screen\">\n")  
    f.write("<link rel=\"stylesheet\" href=\"css.css\" type=\"text/css\" media=\"screen\">\n")      
    f.write("<title>GAUSS Generator Plots</title>\n")
    f.write("</HEAD>\n")
    f.write("<BODY>\n")
    f.write("<div class=\"ctitle\">\n")
    f.write("<TABLE id=\"pagetitle\">\n")
    f.write("<TBODY>\n")
    f.write("<TR>\n")
    f.write("<TD class=iconspace>\n")
    f.write("<A href=\"http://cern.ch/lhcb-comp\">\n")
    f.write("<IMG id=lhcblogo  src=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/images/lhcbcomputinglogo.gif\" >\n")
    f.write("</A>\n")
    f.write("</TD>\n")
    f.write("<TD vAlign=middle align=center>\n")
    f.write("<H1><a href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss\" >Generator plots for Gauss version " + self.version + "</a></H1></TD>\n")
    f.write("</TR>\n")
    f.write("</TBODY>\n")
    f.write("</TABLE>\n")    
    f.write("</div>\n")
    f.write("<div class=pagebody>\n")
    f.write("<div id=manifest>\n")
    f.write("<p>Comparison of Gauss " + self.version + " (Pythia " + self.pythiaVersion + ") with " + self.refVersion + " (Pythia " + self.refPythiaVersion + ") " )
    f.write("for event type " + self.eventType + "</p>\n")
    f.write("</div>\n")
    f.write("<H2>Version numbers</H2>\n")
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")
#    
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Gauss\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.version+"\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.refVersion+"\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("Pythia\n") 
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.pythiaVersion+"\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write(self.refPythiaVersion+"\n")
    f.write("</TD>\n")
    f.write("</TR>\n")
    f.write("</TABLE>\n")
    
    f.write("<H2>Basic quantities</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   
    
    for nameOfQuantity in self.listOfQuantities:
#    for nameofQuantity, refQuantity in self.basicRefQuantities.iteritems():      
      f.write("<TR>\n")
      f.write("<TD>\n")
      f.write(nameOfQuantity + "\n")
      f.write("</TD>\n")
      f.write("<TD>\n")
      if self.basicQuantities[nameOfQuantity]!=None:
        f.write("%.3f"%self.basicQuantities[nameOfQuantity]+"\n")
      f.write("</TD>\n")
      f.write("<TD>\n")
      if self.basicQuantities[nameOfQuantity]!=None:
        f.write("%.3f"%self.basicRefQuantities[nameOfQuantity]+"\n")
      f.write("</TD>\n")
      f.write("</TR>\n")
    f.write("</TABLE>\n")
    
    f.write("<H2>Plots</H2>\n")
    
    f.write("<TABLE>\n") 
    f.write("<TR>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Current\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Reference\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("<TD>\n")
    f.write("<div class=firstcell>\n")    
    f.write("Superimposed\n")
    f.write("</div>\n")
    f.write("</TD>\n")
    f.write("</div>\n")
    f.write("</TR>\n")   

    for nameOfPlot in self.listOfPlots:
#    for nameofPlot, refPlot in self.refPlots.iteritems():      
      f.write("<TR>\n")
      f.write("<TD>\n")
      f.write(nameOfPlot + "\n")
      f.write("</TD>\n")
      f.write("<TD>\n")
      f.write("<A href=\""+self.plots[nameOfPlot]+"\">X</A>\n")
      f.write("</TD>\n")
      f.write("<TD>\n")
      f.write("<A href=\""+self.refPlots[nameOfPlot]+"\">X</A>\n")
      f.write("</TD>\n")
      f.write("<TD>\n")
      f.write("<A href=\""+self.supPlots[nameOfPlot]+"\">X</A>\n")
      f.write("</TD>\n")
      f.write("</TR>\n")
    f.write("</TABLE>\n")    
    
    f.write("</div>\n")
    f.write("</BODY>\n")
    f.write("</HTML>\n")
    f.close()
  def install(self):
    dstDir = "Gauss_"+self.eventType + "_" + self.version+"_"+self.refVersion+"_Pythia_"+self.pythiaVersion+"_"+self.refPythiaVersion
    if os.path.exists(dstDir):
      for f in os.listdir(dstDir):
        os.remove( dstDir+"/"+f)
      os.rmdir( dstDir )
    os.mkdir( dstDir )
    shutil.copy( self.name , dstDir )
    for nameofPlot , refPlot in self.refPlots.iteritems():
      shutil.copy( refPlot , dstDir )
      shutil.copy( self.plots[nameofPlot] , dstDir )
      shutil.copy( self.supPlots[nameofPlot] , dstDir ) 
  def addToIndex(self):
    dstDir = "Gauss_"+self.eventType + "_" + self.version+"_"+self.refVersion+"_Pythia_"+self.pythiaVersion+"_"+self.refPythiaVersion    
    f = open("index.html",'r')
    g = open("newindex.html",'w') 
    for line in f:
      g.write(line)
      if line=="</TR> <!-- end of table -->\n":
        g.write( "<TR>\n" )
        g.write( "<TD>\n" ) 
        g.write( "<A href=\""+dstDir+"/page.html\">"+self.eventType+"</A>\n")
        g.write( "</TD>\n<TD>\n")
        g.write( self.version + "\n </TD>\n<TD>\n" + self.pythiaVersion + "\n" )
        g.write( "</TD>\n<TD>" + self.refVersion + "\n</TD>\n<TD>\n" + self.refPythiaVersion + "\n" )
        g.write( "</TD>\n</TR>" )
    f.close()        
    g.close()
    os.remove( "index.html" ) 
    os.rename( "newindex.html","index.html")
      
#####################################################################
# Read command line options
def main():
  usage = "usage: %prog [options]"
  parser = OptionParser(usage)
  parser.add_option( "-r" , "--refhisto" , action="store", type="string" , 
    dest="referenceHistoName" , help="Histogram reference file" ) 
  parser.add_option( "-l" , "--reflog" , action="store" , type="string" ,
    dest="referenceLogfileName" , help="Reference log file" ) 
  parser.add_option( "-s" , "--histo" , action="store", type="string" , 
    dest="HistoName" , help="Histogram file to compare" ) 
  parser.add_option( "-f" , "--log" , action="store" , type="string" ,
    dest="LogfileName" , help="Log file to compare" ) 
  parser.add_option( "-v" , "--verbose" , action="store_true" , dest="verbose" )  
  parser.add_option( "-a" , "--add-to-index" , action="store_true" , dest="addToIndex" ) 
  parser.add_option( "-i" , "--install" , action="store_true" , dest="install" ) 
      
  (options, args) = parser.parse_args()
  
  webPage = GeneratorWebPage("page.html")
  
  #####################################################################
  if options.verbose:
    print "Read reference log file: " + options.referenceLogfileName
  ReferenceLog = GeneratorLogFile( options.referenceLogfileName ) 
  ReferenceLog.computeQuantities()

  if options.verbose:
    print "Read log file: " + options.LogfileName
  TheLog = GeneratorLogFile( options.LogfileName ) 
  TheLog.computeQuantities()
  
  if ReferenceLog.eventType() != TheLog.eventType():
    print "ERROR: the two files are of different event types: " + ReferenceLog.eventType() + " / " + TheLog.eventType()
  print "Comparison for event type " + ReferenceLog.eventType() 
  print "between versions " + ReferenceLog.gaussVersion() + " and " + TheLog.gaussVersion()
    
  if options.verbose:    
    print "Reference Event type = " , ReferenceLog.eventType() 
    print "Reference Gauss version = " , ReferenceLog.gaussVersion()
    print "Reference Total event = " , ReferenceLog.totalAcceptedEvents()
    print "Reference Pythia version = " , ReferenceLog.pythiaVersion()
    print "Reference Total cross-section = " , ReferenceLog.totalCrossSection()
    print "Reference b cross-section = " , ReferenceLog.bCrossSection()
    print "Reference c cross-section = " , ReferenceLog.cCrossSection()
    print "Reference prompt charm = " , ReferenceLog.promptCharmCrossSection()
    print "Reference signal process cross-section = " , ReferenceLog.signalProcessCrossSection()
    print "Reference signal process from B cross-section = " , ReferenceLog.signalProcessFromBCrossSection()
    print "Reference generator level cut efficiency = " , ReferenceLog.generatorLevelCutEfficiency()
    print "Reference processing time per event [ms] = " , ReferenceLog.timePerEvent()
    print "Event type = " , TheLog.eventType() 
    print "Gauss version = " , TheLog.gaussVersion()
    print "Total event = " , TheLog.totalAcceptedEvents()
    print "Pythia version = " , TheLog.pythiaVersion()
    print "Total cross-section = " , TheLog.totalCrossSection()
    print "b cross-section = " , TheLog.bCrossSection()
    print "c cross-section = " , TheLog.cCrossSection()
    print "prompt charm = " , TheLog.promptCharmCrossSection()
    print "signal process cross-section = " , TheLog.signalProcessCrossSection()
    print "signal process from B cross-section = " , TheLog.signalProcessFromBCrossSection()
    print "generator level cut efficiency = " , TheLog.generatorLevelCutEfficiency()
    print "processing time per event [ms] = " , TheLog.timePerEvent()
  
  webPage.setVersions( TheLog.gaussVersion() , ReferenceLog.gaussVersion() , TheLog.pythiaVersion() , ReferenceLog.pythiaVersion() )   
  webPage.setEventType( TheLog.eventType() )
  webPage.addQuantity("Total cross-section" , ReferenceLog.totalCrossSection() , TheLog.totalCrossSection() ) 
  webPage.addQuantity("b cross-section" , ReferenceLog.bCrossSection() , TheLog.bCrossSection() )
  webPage.addQuantity("c cross-section" , ReferenceLog.cCrossSection() , TheLog.cCrossSection() )
  webPage.addQuantity("Prompt charm cross-section" , ReferenceLog.promptCharmCrossSection() , TheLog.promptCharmCrossSection() )
  webPage.addQuantity("Signal process cross-section" , ReferenceLog.signalProcessCrossSection() , TheLog.signalProcessCrossSection() )                
  webPage.addQuantity("Generator level cut efficiency" , ReferenceLog.generatorLevelCutEfficiency() , TheLog.generatorLevelCutEfficiency() )                
  webPage.addQuantity("Processing time per event [ms]" , ReferenceLog.timePerEvent() , TheLog.timePerEvent() )                

  
  ##### Ref Histos    
  if options.verbose:
    print "Read reference histogram file: " + options.referenceHistoName 
  refFile = TFile( options.referenceHistoName ) 
 
  IntREF = gDirectory.Get( 'GenMonitorAlg/10' )
  PrimaryVtxXREF = gDirectory.Get( 'GenMonitorAlg/11' ) 
  PrimaryVtxYREF = gDirectory.Get( 'GenMonitorAlg/12' ) 
  PrimaryVtxZREF = gDirectory.Get( 'GenMonitorAlg/13' ) 
  MultiplicityREF = gDirectory.Get( 'GenMonitorAlg/3' ) 
  PseudorapREF = gDirectory.Get( 'GenMonitorAlg/44' )
  PtREF = gDirectory.Get( 'GenMonitorAlg/45' )
  ProcessREF = gDirectory.Get( 'GenMonitorAlg/5' )
  MultInLHCbREF = gDirectory.Get( 'GenMonitorAlg/4' ) 


  #####  Histos    
  if options.verbose:
    print "Compare with histogram file: " + options.HistoName
  aFile = TFile( options.HistoName )

  Int = gDirectory.Get( 'GenMonitorAlg/10' )
  PrimaryVtxX = gDirectory.Get( 'GenMonitorAlg/11' ) 
  PrimaryVtxY = gDirectory.Get( 'GenMonitorAlg/12' ) 
  PrimaryVtxZ = gDirectory.Get( 'GenMonitorAlg/13' ) 
  Multiplicity = gDirectory.Get( 'GenMonitorAlg/3' )
  Pseudorap = gDirectory.Get( 'GenMonitorAlg/44' )
  Pt = gDirectory.Get( 'GenMonitorAlg/45' )
  Process = gDirectory.Get( 'GenMonitorAlg/5' )
  MultInLHCb = gDirectory.Get( 'GenMonitorAlg/4' ) 

  c1 = TCanvas( 'c1' , 'Gauss' , 200 , 10 , 800 , 800 ) 
  
  gStyle.SetOptStat(2210)
  
  ####################################################################
  nIntRefHist = GeneratorHisto( c1 , IntREF , Int ,
    "Number of interactions" , "N" , "Number of primary interactions per bunch" ,
    "NInteractionsPerBunch" , True ) 
  nIntRefHist.plot()
  webPage.addPlot(nIntRefHist)
  ####################################################################
  primaryVtxXRefHist = GeneratorHisto( c1 , PrimaryVtxXREF , PrimaryVtxX ,
    "x (mm)" , "N/0.01 mm" , "x position of primary vertex" ,
    "XPrimaryVtx" , False )
  primaryVtxXRefHist.plot()
  webPage.addPlot(primaryVtxXRefHist)
  
  ####################################################################
  primaryVtxYRefHist = GeneratorHisto( c1 , PrimaryVtxYREF , PrimaryVtxY ,
    "y (mm)" , "N/0.01 mm" , "y position of primary vertex" ,
    "YPrimaryVtx" , False )
  primaryVtxYRefHist.plot()
  webPage.addPlot(primaryVtxYRefHist)
  
  ####################################################################
  primaryVtxZRefHist = GeneratorHisto( c1 , PrimaryVtxZREF , PrimaryVtxZ ,
    "z (mm)" , "N/4 mm" , "z position of primary vertex" ,
    "ZPrimaryVtx" , False )
  primaryVtxZRefHist.plot()
  webPage.addPlot(primaryVtxZRefHist)
  
  ####################################################################
  multiplicityRefHist = GeneratorHisto( c1 , MultiplicityREF , Multiplicity ,
    "N(charged particles)" , "N/5" , "Multiplicity of stable charged particles in 4#pi" ,
    "Multiplicity" , True )
  multiplicityRefHist.plot()
  webPage.addPlot(multiplicityRefHist)
  
  ####################################################################
  pseudorapRefHist = GeneratorHisto( c1 , PseudorapREF , Pseudorap ,
    "#eta" , "N/0.2" , "#eta of stable charged particles in 4#pi" ,
    "Pseudorapidity" , False )
  pseudorapRefHist.plot()
  webPage.addPlot(pseudorapRefHist)
  
  ####################################################################
  ptRefHist = GeneratorHisto( c1 , PtREF , Pt , 
    "p_{T} (GeV/c)" , "N/40 MeV/c" , "p_{T} of stable charged particles in 4#pi" ,
    "Pt" , True )
  ptRefHist.plot()
  webPage.addPlot(ptRefHist)
  
  ####################################################################
  processRefHist = GeneratorHisto( c1 , ProcessREF , Process ,  
    "Process Number" , "N" , "Process" ,
    "Process" , False )
  processRefHist.plot()
  webPage.addPlot(processRefHist)
  
  ####################################################################
  multiplicityInLHCbRefHist = GeneratorHisto( c1 , MultInLHCbREF , MultInLHCb ,
    "N(charged particles)" , "N/5" , "Multiplicity of charged particles in LHCb" ,
    "MultiplicityInLHCb" , True )
  multiplicityInLHCbRefHist.plot()
  webPage.addPlot(multiplicityInLHCbRefHist)
  
  ####################################################################
  
  refFile.Close()
  aFile.Close()
  
  ###########################################################################
  webPage.create()
  if options.install:
    webPage.install()
  if options.addToIndex:
    webPage.addToIndex()
    
if __name__ == "__main__":
  main()
  
