############################
# FCC Track Reco Plots Makefile
# run from FCCSW directory
# with 
#
# make -f Reconstruction/RecTracker/RecPlotsMakeFile.mk plots
#
#
#
###########################
.PHONY: all
all: plots

plots: tricktrack_seeding_example.root
	./run python Reconstruction/RecTracker/scripts/plotTrackSeeds.py tricktrack_tracks.root
tracks: muons_for_seeding.root
	./run fccrun.py Reconstruction/RecTracker/options/TrickTrackReco.py 
muons: 
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 1000 -s 0123456 --outName muons_for_seeding.root --singlePart --particle 13

