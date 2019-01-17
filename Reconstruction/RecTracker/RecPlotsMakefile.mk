###########################
# FCC Track Reco Plots Makefile
# run from FCCSW directory
# with 
#
# make -f Reconstruction/RecTracker/RecPlotsMakeFile.mk
#
#
#
###########################


# run the track seeding 
tracks: muons_for_seeding_tracks.root
muons_for_seeding_tracks.root: muons
	./run fccrun.py Reconstruction/RecTracker/options/TrickTrackReco.py --nevents 5000 --cleanHits | tee muons_for_seeding_tracks.log 

# simulate single muons to have a sample for track seeding
muons: muons_for_seeding.log
muons_for_seeding.log:
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 15000 -s 0123456 --outName muons_for_seeding.root --singlePart --particle 13 --etaMin -6 --etaMax 6 --ptMin 1100 --noVertexSmearTool --ptMax 1000000 --trajectories | tee muons_for_seeding.log


muons_for_seeding_noVertexSmearTool_tracks.log: muons_for_seeding_noVertexSmearTool.log
	./run fccrun.py Reconstruction/RecTracker/options/TrickTrackReco.py --inputfile muons_for_seeding_noVertexSmearTool.root --outputfile muons_for_seeding_noVertexSmearTool_tracks.root --nevents 15000 | tee muons_for_seeding_noVertexSmearTool_tracks.log 
muons_for_seeding_noVertexSmearTool.log:
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 15000 -s 0123456 --outName muons_for_seeding_noVertexSmear.root --singlePart --particle 13 --etaMin 0 --etaMax 6 --ptMin 500 --ptMax 1000000 --trajectories --noVertexSmearTool | tee muons_for_seeding_noVertexSmear.log


geantinos_for_seeding_single_direction_tracks.log: geantinos_for_seeding_single_direction.log
	./run fccrun.py Reconstruction/RecTracker/options/TrickTrackReco.py --inputfile geantinos_for_seeding_single_direction.root --outputfile geantinos_for_seeding_single_direction_tracks.root --nevents 5000 | tee geantinos_for_seeding_single_direction_tracks.log 
geantinos_for_seeding_single_direction.log:
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 5000 -s 0123456 --outName geantinos_for_seeding_single_direction.root --singlePart --particle 13 --phiMin 0 --phiMax 0 --etaMin 1 --etaMax 1 --ptMin 500 --ptMax 1000000 --trajectories --geantinos --noVertexSmearTool | tee geantinos_for_seeding_single_direction.log

geantino_tracks: geantinos_for_seeding_tracks.root
geantinos_for_seeding_tracks.root: geantinos
	./run fccrun.py Reconstruction/RecTracker/options/TrickTrackReco.py --inputfile geantinos_for_seeding.root --outputfile geantinos_for_seeding_tracks.root --nevents 5000 | tee geantinos_for_seeding_tracks.log 
# simulate single muons to have a sample for track seeding
geantinos: geantinos_for_seeding.log
geantinos_for_seeding.log:
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 5000 -s 0123456 --outName geantinos_for_seeding.root --singlePart --particle 13 --etaMin 0 --etaMax 6 --ptMin 500 --ptMax 1000000 --trajectories --geantinos  | tee geantinos_for_seeding.log

geantinos_for_seeding_noVertexSmearTool.log:
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 15000 -s 0123456 --outName geantinos_for_seeding_noVertexSmearTool.root --singlePart --particle 13 --etaMin 0 --etaMax 6 --ptMin 500 --ptMax 1000000 --trajectories --geantinos --noVertexSmearTool  | tee geantinos_for_seeding_noVertexSmearTool.log


# simulate single muons to have a sample for track seeding
muons_discrete_pt: muons_for_seeding_discrete_pt.log
muons_for_seeding_discrete_pt.log:
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 5000 -s 0123456 --outName muons_for_seeding_discrete_pt.root --singlePart --particle 13 --etaMin 0 --etaMax 6 --discretePt --pt 1000 2000 5000 10000 100000 1000000 10000000  --trajectories | tee muons_for_seeding_discrete_pt.log
# use truth seeding + RiemannFit to calculate single particle resolutions from the muon sample
single_particle_resolutions: single_particle_resolutions.log muons_discrete_pt
single_particle_resolutions.log:
	./run fccrun.py Reconstruction/RecTracker/options/single_particle_trackFits.py --inputfile muons_for_seeding_discrete_pt.root --outputfile single_particle_resolutions.root | tee single_particle_resolutions.log
	



# create acceptance plots for displaced vertices
muons_displaced_vertex_check: muons_displaced_vertex_check.log
muons_displaced_vertex_check.log:
	./run fccrun.py  Reconstruction/RecTracker/options/geantSim_TrackerPerformance.py -N 2000 -s 0123456 --outName muons_displaced_vertex_check.root --singlePart --particle 13 --etaMin 1 --etaMax 1 --phiMin 0 --phiMax 0 --discretePt --pt 1000000 --noVertexSmearTool   --displacedVertex | tee muons_displaced_vertex_check.log
# run the track seeding 
displaced_vertex_tracks: muons_displaced_vertex_check.log
	./run fccrun.py Reconstruction/RecTracker/options/TrickTrackReco.py --inputfile muons_displaced_vertex_check.root --outputfile muons_displaced_vertex_check_tracks.root --nevents 2000
