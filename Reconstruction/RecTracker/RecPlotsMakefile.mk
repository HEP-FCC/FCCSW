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
	./run python Reconstruction/RecTracker/scripts/plotTrackSeeds.py tricktrack_seeding_example.root
tracks: muons_for_seeding.root
	./run fccrun.py Reconstruction/RecTracker/options/TrickTrackSeedingTest.py 
muons: 
	./run gaudirun.py Reconstruction/RecTracker/options/muons_for_seeding.py
