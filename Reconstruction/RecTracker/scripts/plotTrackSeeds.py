
import sys
import matplotlib
import ROOT
import argparse
from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt

# matplotlib customization
#from matplotlib import rc
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
## for Palatino and other serif fonts use:
#rc('font',**{'family':'serif','serif':['Palatino']})
#rc('text', usetex=True)

# only backend that will work with LCG
matplotlib.use("TkAgg")

# command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("filename", help="edm file to read")
parser.add_argument("--track_plots", help="create real space plots of tracks")
parser.add_argument("--nevents", help="max number of events to process", type=int, default=1000)
args = parser.parse_args()

plot_tracks = args.track_plots

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
  # generate real space points from track parameters
  x = xc + rho * np.cos(charge*path_points + phi0 - np.pi*0.5)
  y = yc + rho * np.sin(charge*path_points + phi0 - np.pi*0.5)
  return np.array([x, y, z]).T


# plot detector cartoon 
if plot_tracks:
  
  plt.figure("xy")
  ax = plt.gca()
  for r in [25, 60, 100, 150]:
    ax.add_artist(plt.Circle((0,0), r,fill=False, color="red", alpha=0.4, lw=6 ))
  plt.figure("rz")
  ax = plt.gca()
  for z in [25, 60, 100, 150]:
    ax.add_artist(plt.Rectangle((-150,z - 3), 300, 5 ,fill=True, color="red", alpha=0.4, lw=0 ))

events = EventStore([args.filename])
print len(events),  " events in rootfile ", args.filename

# lists to hold event data
l_etas = []
l_pts = []
l_dpts = []
l_true_pts = []

# main event loop
for i, store in enumerate(events):
    print "event ", i
    if i > args.nevents:
      break
    genparticles = store.get("allGenParticles")
    print "processing allGenParticles ..."
    for t in genparticles:
        momentum = [t.core().p4.px, t.core().p4.py, t.core().p4.pz, t.core().p4.mass]
        tm = ROOT.TLorentzVector(*momentum)
        print "\t sim Eta:", tm.Eta() 
        l_etas.append(tm.Eta())
        l_true_pts.append(np.rint(tm.Pt()))
        vertex = [0,0,0]
        print "\tsim trackID: ", t.core().bits, "sim pdgId: ", t.core().pdgId, "momentum: ", [t.core().p4.px, t.core().p4.py, t.core().p4.pz]
        print "\tsim phi: ", np.arctan(t.core().p4.py / t.core().p4.px)
        print "\tsim cottheta: ", t.core().p4.pz / np.sqrt(t.core().p4.px**2 +  t.core().p4.py**2)
        print "\tsim q_pT: ", 1. / np.sqrt(t.core().p4.px**2 +  t.core().p4.py**2)
        charge = 1
        if t.core().charge == -1:
          charge = -1
        cpar = Particle2Track(momentum, vertex, charge=charge)
        print "\tsim calculated params:", cpar
        cmom = Track2Particle(cpar)
        print "\tsim calculated particle: ", cmom

    print "processing tracks ..."
    tracks = store.get('tracks')
    for t in tracks:
        if t.bits() == 1:
          print "\t track ID: ", t.bits()
          pos = []
          ids = []
          ts = t.states(0)
          trackparams = [
            ts.d0(),
            ts.z0(),
            ts.phi(),
            ts.theta(),
            ts.qOverP() * -10,
            ]
          l_pts.append(1./trackparams[-1])
          print "\ttrack parameters: ", trackparams
          trackcov = [ts.cov()[0], ts.cov()[5], ts.cov()[9], ts.cov()[12], ts.cov()[14]] 
          print "\ttrack covariances: ", trackcov
          l_dpts.append(np.sqrt(trackcov[2]))
          
          if plot_tracks:
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

    if plot_tracks:
      plt.figure("xy")
      pos3 = helix(cpar)
      print pos3
      plt.plot(pos3[:,0],pos3[:,1], '-', lw=1,  color="red", alpha=.5)
      plt.figure("rz")
      plt.plot(pos3[:,2], np.sqrt(pos3[:,0]**2 + pos3[:,1]**2), '--', lw=2, color="red", alpha=0.7)
      pos = []
      ids = []
      print "processing trajectories ..."
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

      print "processing hits ..."
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
  plt.figure("xy")
  plt.xlim(-350, 350)
  plt.ylim(-350,350)
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


etas = np.array(l_etas)
dpts = np.array(l_dpts)
pts = np.array(l_pts)
colors = {1.: "black", 2: "darkblue", 5.: "blue", 100.: "green", 1000: "magenta", 10000.: "darkgreen", 10.: "red"}
for e in np.unique(l_true_pts):
  print "pT: ", e
  i = np.array(l_true_pts) == e
  e = int(e)
  plt.figure("pt_res")
  plt.semilogy(etas[i], np.abs(pts[i]), 'o', label=e, color=colors[e])
  plt.semilogy(etas[i], np.abs(dpts[i]), 'd', color=colors[e])
  plt.figure("pt_res_div")
  plt.semilogy(etas[i], np.abs(dpts[i]) / np.abs(pts[i]) * 100, 'o', label=e, color=colors[e])

plt.figure("pt_res")
plt.xlabel(r"$\eta$")

plt.legend(loc="best", title="Pt [GeV]")
plt.figure("pt_res_div")
plt.xlabel(r"$\eta$")
plt.ylabel(r"$\frac {\delta p_T} {p_T} [\%] $")
plt.title(r"Track resolution for const Pt across $\eta$")
plt.legend(loc="best", title="Pt [GeV]")
plt.savefig("tricktrack_singleparticle_res.png")
plt.show()
