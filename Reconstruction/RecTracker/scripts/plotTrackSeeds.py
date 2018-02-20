
import matplotlib

from matplotlib import rc
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
## for Palatino and other serif fonts use:
#rc('font',**{'family':'serif','serif':['Palatino']})
#rc('text', usetex=True)

matplotlib.use("TkAgg")

import sys


import ROOT
ROOT.gSystem.Load("libdatamodel.so")

plot_tracks = True

def Particle2Track(p, vertex, charge=1., B=4.):
    pT = np.sqrt(p[0]**2 + p[1]**2)
    pX = p[0]
    pY = p[1]
    pZ = p[2]
    q_pT = charge / pT
    R = pT / (0.003 * charge * B) 
    d0 = 0.#-charge*(np.sqrt(vertex[0]**2 + vertex[1]**2) - R)
    phi0 = np.arctan(pY / pX)
    cotTheta = pZ / pT
    z0 = vertex[2] 
    return [d0, z0, phi0, cotTheta, q_pT]

def Track2Particle(params):
    d0, z0, phi0, cotTheta, q_pT = params
    charge = np.sign(q_pT)
    pT = charge / q_pT
    pZ  =  cotTheta * pT
    vertex = [d0*np.cos(phi0), d0*np.sin(phi0), z0]
    p = [pT*np.cos(phi0), pT*np.sin(phi0), pZ]
    return [p, vertex]
    

def helix(params, time_points=(0,2,2000), B=4.):
  path_points = np.linspace(*time_points)
  d0, z0, phi0, cotTheta, q_pT = params
  charge = np.sign(q_pT)
  # motion in z linear, unchanged by magnetic field in z
  # cartesian coordinates of point of closest approach
  x0 =  d0 * np.cos(phi0)
  y0 = d0 * np.sin(phi0)
  rho = charge / np.abs(q_pT) / 0.0003 / B
  z = z0 +  cotTheta * np.abs(rho) * path_points 
  # cartesian coordinates of helix center
  xc = x0 + rho * np.sin(phi0)
  yc = y0 + rho * np.cos(phi0)
  print "helix center and radius:", xc, yc, rho
  # generate real space points from track parameters
  x = xc + rho * np.cos(charge*path_points + phi0 - np.pi*0.5)
  y = yc + rho * np.sin(charge*path_points + phi0 - np.pi*0.5)
  return np.array([x, y, z]).T



from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt
plt.figure("xy")
ax = plt.gca()
for r in [25, 60, 100, 150]:
  ax.add_artist(plt.Circle((0,0), r,fill=False, color="red", alpha=0.4, lw=6 ))
plt.figure("rz")
ax = plt.gca()
for z in [25, 60, 100, 150]:
  ax.add_artist(plt.Rectangle((-150,z - 3), 300, 5 ,fill=True, color="red", alpha=0.4, lw=0 ))

events = EventStore([sys.argv[1]])
print 'number of events: ', len(events)
for i, store in enumerate(events):
    print "event ", i
    if i > 3:
      break
    pos = []
    ids = []
    clusters = store.get('trajectoryPoints')
    print clusters    
    for c in clusters:
        if c.bits() < 100:
          cor = c.position()
          pos.append([cor.x, cor.y, cor.z])
          ids.append([c.bits()])
    pos = np.array(pos)
    ids = np.array(ids)
    plt.figure("xy")
    plt.plot(pos[:,0],pos[:,1], '-', color="green")
    plt.figure("rz")
    plt.plot(pos[:,2], np.sqrt(pos[:,0]**2 + pos[:,1]**2), '-', color="green")

    hits = store.get('positionedHits')
    pos = []
    ids = []
    for c in hits:
        cor = c.position()
        pos.append([cor.x, cor.y, cor.z])
        ids.append([c.bits()])
    pos = np.array(pos)
    ids = np.array(ids)
    plt.figure("xy")
    plt.plot(pos[:,0],pos[:,1], 'o', color="midnightblue")
    plt.figure("rz")
    plt.plot(pos[:,2], np.sqrt(pos[:,0]**2 + pos[:,1]**2), 'o', color="midnightblue")

    if plot_tracks:

      tracks = store.get('tracks')
      for t in tracks:
          if t.bits() == 1:
            print "track ID: ", t.bits()
            pos = []
            ids = []
            ts = t.states(0)
            trackparams = [
              ts.d0(),
              ts.z0(),
              ts.phi(),
              ts.theta(),
              ts.qOverP() *-50,
              ]
            print "track parameters: ", trackparams
            pos2 = helix(trackparams)
                
            for j in range(t.hits_size()):
              cor = t.hits(j).position()
              pos.append([cor.x, cor.y, cor.z])
            pos = np.array(pos)
            plt.figure("xy")
            plt.plot(pos[:,0],pos[:,1], '--', color="black")
            plt.plot(pos2[:,0],pos2[:,1], '-', lw=3, color="blue", alpha=0.5)
            plt.figure("rz")
            plt.plot(pos[:,2], np.sqrt(pos[:,0]**2 + pos[:,1]**2), '--', color="black")
            plt.plot(pos2[:,2], np.sqrt(pos2[:,0]**2 + pos2[:,1]**2), '--', color="blue", alpha=0.5)
      part = store.get("allGenParticles")
      for t in part:
          momentum = [t.core().p4.px, t.core().p4.py, t.core().p4.pz]
          vertex = [0,0,0]
          print "sim trackID: ", t.core().bits, "sim pdgId: ", t.core().pdgId, "momentum: ", [t.core().p4.px, t.core().p4.py, t.core().p4.pz]
          print "sim phi: ", np.arctan(t.core().p4.py / t.core().p4.px)
          print "sim cottheta: ", t.core().p4.pz / np.sqrt(t.core().p4.px**2 +  t.core().p4.py**2)
          print "sim q_pT: ", 1. / np.sqrt(t.core().p4.px**2 +  t.core().p4.py**2)
          charge = 1
          if t.core().charge == -1:
            charge = -1
          cpar = Particle2Track(momentum, vertex, charge=charge)
          print "calculated params:", cpar
          cmom = Track2Particle(cpar)
          print "calculated particle: ", cmom
          plt.figure("xy")
          #cpar[-1] = 0.00001
          pos3 = helix(cpar)
          print pos3
          plt.plot(pos3[:,0],pos3[:,1], '-', lw=1,  color="red", alpha=.5)
          plt.figure("rz")
          plt.plot(pos3[:,2], np.sqrt(pos3[:,0]**2 + pos3[:,1]**2), '--', lw=2, color="red", alpha=0.7)

plt.figure("xy")
plt.xlim(-1250, 1250)
plt.ylim(-1250,1250)
plt.xlabel("x [mm]")
plt.ylabel("y [mm]")
plt.savefig("tt_xy.png")
plt.savefig("tt_xy.pdf")
plt.figure("rz")
plt.xlim(-150, 150)
plt.ylim(0,150)
plt.xlabel("z [mm]")
plt.ylabel("r [mm]")
plt.savefig("tt_rz.png")
plt.savefig("tt_rz.pdf")

plt.show()
