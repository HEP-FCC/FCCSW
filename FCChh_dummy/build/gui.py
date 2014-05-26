from Tkinter import *
import os
import glob

toremove=['elements.xml','geant4.xml','constants.xml','FCCDectOpt02.xml','materials.xml','N04.xml']
states = []
xmllist=glob.glob('../compact/*.xml')
print xmllist
xmllist=['../compact/HCal_Barrel.xml', '../compact/ECal_Barrel.xml', '../compact/BeamPipe.xml', '../compact/Pixel_Barrel.xml', '../compact/Solenoid_Outer.xml', '../compact/Dipoles.xml', '../compact/Muon_Barrel.xml', '../compact/ECal_EndCaps.xml', '../compact/Solenoid_Inner.xml', '../compact/HCal_EndCaps.xml', '../compact/Muons_EndCaps.xml', '../compact/ForwardCavities.xml', '../compact/ECal_Forwards.xml', '../compact/HCal_Forwards.xml', '../compact/Muons_Forward.xml']

root = Tk()

def RunAll():
    cmd='geoDisplay -compact file:../compact/FCCDectOpt02.xml'
    for v in xrange(len(xmllist)):
        cmd+=' -compact file:'
        cmd+=xmllist[v]
    print cmd
    os.system(cmd)

def showcommand():
    cmd='geoDisplay -compact file:../compact/FCCDectOpt02.xml'
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
    chk = Checkbutton(root, text = xml.replace('../compact/',''), variable = var, \
                         onvalue = 1, offvalue = 0, height=2, \
                         width = 20)

    #chk = Checkbutton(root, text=xml.replace('../compact/',''), variable=var)
    chk.pack()
    states.append(var)
    print var

b=Button(toolbar, text='Show', command=showcommand)
b.pack(side=LEFT, padx=2, pady=2)

mainloop()
