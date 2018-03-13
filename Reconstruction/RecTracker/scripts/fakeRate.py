"""
Example reconstruction analysis script for FCCSW track reconstruction.

Performs calculation of efficiencies and fake rates for FCCSW simulated data
based on the MC Truth and the reconstucted tracks.
"""

import ROOT
import argparse
# PODIO python interface: requires the fcc-edm library (libdatamodel.so in LD_LIBRARY_PATH)
# and PODIO in the python path 
from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats



# command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("filename", help="edm file to read")
parser.add_argument("--nevents", help="max number of events to process", type=int, default=1000)
parser.add_argument("--ptCut", help="min pt for particles to be included in efficiency definition [GeV]", type=float, default=1.)
parser.add_argument("--etaCut", help="max eta for particles to be included in efficiency definiton ", type=float, default=5.)
parser.add_argument("--textBoxPositionX", help="textBoxPositionX", type=float, default=0.5)
parser.add_argument("--textBoxPositionY", help="textBoxPositionY", type=float, default=0.7)
parser.add_argument("--histos", action="store_true",  help="plot efficiency histograms", )
parser.add_argument("--ignore_secondaries", action="store_true",  help="don't include secondary particles in efficiency definitions", )
parser.add_argument("--process_genparticles", action="store_true",  help="use genparticles in the event loop", )
parser.add_argument("--process_trackerhits", action="store_true",  help="use trackerhits in the event loop", )
args = parser.parse_args()


events = EventStore([args.filename])
print len(events),  " events in rootfile ", args.filename
args.nevents = min(len(events), args.nevents)

# arrays to hold ntuples
etas = {}
pts = {}
efficiencies = []
fake_rates = []
successfully_found_track = {}
# main event loop
for i, store in enumerate(events):
    if i < args.nevents:
      
      ### print "Processing generated Particles from branch 'allGenParticles'" # 
      if args.process_genparticles: # currently unused
        for p in store.get("GenParticles"):
            pm = [ p.core().p4.px, 
                   p.core().p4.py, 
                   p.core().p4.pz, 
                   p.core().p4.mass, ]
            pm = ROOT.TLorentzVector(*pm)
            pass

      ### print "processing trackerhits from branch 'positionedhits' " #########
      if args.process_trackerhits: # currently unused
        hits = store.get("TrackerPositionedHits")
        ids = []
        for h in hits:
              ids.append(h.core().bits)
        id_unique, id_counts = np.unique(ids, return_counts=True)
        print dict(zip(id_unique, id_counts))
        unreconstructable = id_unique[id_counts < 4]

      true_track_ids = []
      numFakes = 0
      num_ignored_secondaries = 0
      for p in store.get("SimParticles"):
            pm = [ p.core().p4.px, 
                   p.core().p4.py, 
                   p.core().p4.pz, 
                   p.core().p4.mass, ]
            pm = ROOT.TLorentzVector(*pm)

            if np.abs(pm.Pt()) > args.ptCut and np.abs(pm.Eta()) < args.etaCut:
              if args.ignore_secondaries:
                if p.core().status != 201: # fcc convention for secondaries
                  true_track_ids.append(p.core().bits)
                  etas[(i, p.core().bits)] = pm.Eta()
                  pts[(i, p.core().bits)] = pm.Pt()
                  successfully_found_track[(i, p.core().bits)] = 0 # initialization, per default not found
                else:
                  num_ignored_secondaries +=1
              else:
                true_track_ids.append(p.core().bits)
                etas[(i, p.core().bits)] = pm.Eta()
                pts[(i, p.core().bits)] = pm.Pt()
      print num_ignored_secondaries, " secondary particles ignored"
          
      ### print "processing reco tracks from branch 'tracks' " #########
      correctly_identified_tracks_ids = []
      tracks = store.get('tracks')
      numTracks = len(tracks)
      for t in tracks:
              reco_track_ids = []
              # go through the hits comprising the track
              for j in range(t.hits_size()):
                # access mc truth for the track Id
                reco_track_ids.append(t.hits(j).core().bits) 
              # if all the hits have the same track id the track was identified correctly
              if len(set(reco_track_ids)) == 1:
                # we might choose to ignore this track for the efficiency nonetheless,
                # (based on pt, eta ... )
                if reco_track_ids[0] in true_track_ids:
                  correctly_identified_tracks_ids.append(reco_track_ids[0])
                  successfully_found_track[(i, reco_track_ids[0])] = 1
              else:
                numFakes += 1
                
      # safeguard against empty lists
      if numTracks > 0:
        try:
          # efficiency: proportion of true tracks that were successfully reconstructe
          efficiency = len(set(correctly_identified_tracks_ids)) / float(len(set(true_track_ids)))
        except ZeroDivisionError:
          efficiency = 0 
        # fake rate: proportion of incorrectly reconstructed tracks
        fake_rate = numFakes / float(numTracks)
        print len(true_track_ids), " tracks in event, eff.: ", efficiency, ", fake rate: ", fake_rate
        efficiencies.append(efficiency)
        fake_rates.append(fake_rate)
      else: 
        efficiencies.append(0)
        fake_rates.append(0)

sk = sorted(etas.iterkeys())
etas = np.array([etas[k] for k in sk])
pts = np.array([pts[k] for k in sk])
efficiencies = np.array([successfully_found_track.get(k, 0) for k in sk])

def efficiency_plot(x, y, conf, ax=plt.gca()):
  means_result = scipy.stats.binned_statistic(x, [y, y**2], 
      bins=conf["numBins"], 
      range=conf["histo_range"], 
      statistic='mean')
  means, means2 = means_result.statistic
  standard_deviations = np.sqrt(means2 - means**2)
  bin_edges = means_result.bin_edges
  bin_centers = (bin_edges[:-1] + bin_edges[1:])/2.
  bin_width = 0.45 * (bin_edges[1:] - bin_edges[0:-1])
  means = np.array(means)
  # clip errorbars
  standard_deviations_high = 0.5 *   np.minimum(standard_deviations, 1. - means )
  standard_deviations_low =  0.5 * np.minimum(standard_deviations ,  means)

  # do the plot
  (_, caps, _) = plt.errorbar(x=bin_centers, y=means, 
        xerr=bin_width, yerr=[standard_deviations_low, standard_deviations_high], 
        linestyle = '', marker='o',capsize=2, capthick=1)

  for cap in caps:
    cap.set_markeredgewidth(1)
  plt.xlabel(conf["xlabel"], horizontalalignment='right', x=1.0)
  plt.ylabel(conf["ylabel"], horizontalalignment='right', y=.95, #rotation="horizontal", 
    labelpad=3)
  plt.text(args.textBoxPositionX, args.textBoxPositionY, r'\textsf{\textit{\textbf{FCC}-hh Sim}, Det \textbf{v3.03}}' + conf["boxtext"], {'size': 24}, transform=plt.gca().transAxes, bbox={"boxstyle": "round", "alpha": 0.9, "fc": "white"})
  plt.gca().fill_between(bin_centers, 0, means, color="blue", alpha=0.2)
  if conf["setlogx"]:
    plt.xscale("log")

  plt.xlim(conf["xlimlow"], conf["xlimhigh"])
  plt.ylim(-0.05,1.05)


  for f in conf["filenames"]: # allow different formats to be saved
    print "Saving ", f
    plt.savefig(f)

# plot efficiencies and fake rates split in pt / eta regimes
if args.histos:
  plt.figure("eff_eta")
  # do some preselection
  eta_selection = pts > args.ptCut
  x = etas[eta_selection]
  y = efficiencies[eta_selection]
  eta_histo_conf = {}
  eta_histo_conf["numBins"] = 70
  eta_histo_conf["histo_range"] = (-6, 6)
  eta_histo_conf["xlabel"] = r"$\eta$"
  eta_histo_conf['ylabel'] = "seeding efficiency"
  eta_histo_conf["filenames"] = [args.filename.replace(".root", "") + "_eff_eta.png"]
  eta_histo_conf["setlogx"] = False
  eta_histo_conf["xlimlow"] = -6
  eta_histo_conf["xlimhigh"] = 6
  eta_histo_conf["boxtext"] = r"\\ \textsf{ $\qquad$ MinBias $\sqrt s = 100$ TeV, n = " + str(args.nevents) + r" \newline Cuts: $|\eta| < $" + str(args.etaCut) + r",  $\quad p_T >  $}" + str(args.ptCut) + " GeV" 
  efficiency_plot(x, y, eta_histo_conf)

  plt.figure("eff_pt")
  # do some preselection
  pt_selection = np.abs(etas) < args.etaCut
  x = pts[pt_selection]
  y = efficiencies[pt_selection]
  pt_histo_conf = {}
  pt_histo_conf["numBins"] = np.logspace(-1,6,20)
  pt_histo_conf["histo_range"] = None
  pt_histo_conf["xlabel"] = r"$p_T$ [GeV]"
  pt_histo_conf['ylabel'] = "seeding efficiency"
  pt_histo_conf["filenames"] = [args.filename.replace(".root", "") + "_eff_pt.png"]
  pt_histo_conf["setlogx"] = True
  pt_histo_conf["xlimlow"] = 0.05
  pt_histo_conf["xlimhigh"] = 10**5
  pt_histo_conf["boxtext"] = r"\\ \textsf{ $\qquad$ MinBias $\sqrt s = 100$ TeV, n = " + str(args.nevents) + r" \newline Cuts: $\qquad |\eta| >  $" + str(args.etaCut)+ r",  $\quad p_T >  $}" + str(args.ptCut) + " GeV" 
  efficiency_plot(x, y, pt_histo_conf)

print " ####################################### "
print "mean efficiency \t mean fakerate "
print np.mean(efficiencies), np.mean(fake_rates)

