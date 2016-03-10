from Tkinter import *
import os
import glob

states = []
#xmllist=glob.glob('DetectorDescription/Detectors/compact/*.xml')
#print xmllist
compactpath='DetectorDescription/Detectors/compact/'
xmllist=['FCChh_BeamTube.xml', 'FCChh_Tracker.xml', 'FCChh_HCalBarrel_TileCal.xml', 'FCChh_Solenoids.xml', 'FCChh_Dipoles.xml' ]
xmllist=[compactpath+p for p in xmllist]
root = Tk()

def RunAll():
    cmd='geoDisplay -compact file:%sFCChh_DectMaster.xml'%compactpath
    for v in xrange(len(xmllist)):
        cmd+=' -compact file:'
        cmd+=xmllist[v]
    print cmd
    os.system(cmd)

def showcommand():
    cmd='geoDisplay -compact file:%sFCChh_DectMaster.xml'%compactpath
    for v in xrange(len(states)):
         if states[v].get(): 
             cmd+=' -compact file:'
             cmd+=xmllist[v]
            
    print cmd
    os.system(cmd)
toolbar = Frame(root)  # create a toolbar

b = Button(toolbar, text="Run All", width=6, command=RunAll)
b.pack(side=LEFT, padx=2, pady=2)

b = Button(toolbar, text="Quit", width=6, command=quit)
b.pack(side=LEFT, padx=2, pady=2)

toolbar.pack(side=TOP, fill=X)
states = []                 
for xml in xmllist:
    var = IntVar()
    chk = Checkbutton(root, text = xml.replace('DetectorDescription/Detectors/compact/',''), variable = var, \
                         onvalue = 1, offvalue = 0, height=2, \
                         width = 20)

    #chk = Checkbutton(root, text=xml.replace('../compact/',''), variable=var)
    chk.pack()
    states.append(var)

b=Button(toolbar, text='Show', command=showcommand)
b.pack(side=LEFT, padx=2, pady=2)

mainloop()
