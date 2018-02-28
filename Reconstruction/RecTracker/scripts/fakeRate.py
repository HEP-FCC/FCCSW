
import sys
import matplotlib
import ROOT
import argparse
from EventStore import EventStore
import numpy as np
import matplotlib.pyplot as plt



# command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("filename", help="edm file to read")
parser.add_argument("--track_plots", help="create real space plots of tracks")
parser.add_argument("--nevents", help="max number of events to process", type=int, default=1000)
args = parser.parse_args()

plot_tracks = args.track_plots


events = EventStore([args.filename])
print len(events),  " events in rootfile ", args.filename


efficiencies = []
fake_rates = []
# main event loop
print "events:", args.nevents
for i, store in enumerate(events):
    if i < args.nevents:
      true_track_ids = []
      numFakes = 0
      genparticles = store.get("allGenParticles")
      #print "processing allGenParticles ..."
      for t in genparticles:
          momentum = [t.core().p4.px, t.core().p4.py, t.core().p4.pz, t.core().p4.mass]
          tm = ROOT.TLorentzVector(*momentum)
          #print "\t sim Eta:", tm.Eta() 
          #if np.abs(tm.Eta()) < 2 and tm.Pt() > 0.5:
            #print tm.Pt()

          #true_track_ids.append(t.core().bits)
      genparticles = store.get("simParticles")
      #print "processing simparticles ..."
      for t in genparticles:
          momentum = [t.core().p4.px, t.core().p4.py, t.core().p4.pz, t.core().p4.mass]
          tm = ROOT.TLorentzVector(*momentum)
          #print "\t sim Eta:", tm.Eta() 
          if np.abs(tm.Eta()) < 2 and tm.Pt() > 0.5:
            #print tm.Pt()

            true_track_ids.append(t.core().bits)

      #print "processing tracks ..."
      correctly_identified_tracks_ids = []
      tracks = store.get('tracks')
      numTracks = len(tracks)
      #print numTracks
      for t in tracks:
              reco_track_ids = []
              pos = []
              ids = []
                
              for j in range(t.hits_size()):
                reco_track_ids.append(t.hits(j).core().bits)
                cor = t.hits(j).position()
                pos.append([cor.x, cor.y, cor.z])
              #print reco_track_ids
              if len(set(reco_track_ids)) == 1:
                correctly_identified_tracks_ids.append(reco_track_ids[0])
              else:
                numFakes += 1
                
      if numTracks > 0:
        #print true_track_ids,  correctly_identified_tracks_ids
        efficiency = len(set(correctly_identified_tracks_ids)) / float(len(set(true_track_ids)))
        fake_rate = numFakes / float(numTracks)
        print len(true_track_ids), efficiency, fake_rate
        efficiencies.append(efficiency)
        fake_rates.append(fake_rate)

print "mean efficiency \t mean fakerate "
print np.mean(efficiencies), np.mean(fake_rates)

