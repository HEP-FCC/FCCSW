RecTracker package
===

Track reconstruction in FCCSW is organised in the following steps ( the completeness of the implementation varies):
A makefile to run the individual components can be found in Reconstruction/RecTracker/RecPlotsMakefile.

# 1. Detector Response -  Digitisation & Clusterization

As the technology of the track detector modules is not yet known in detail, a geometric digitisation based on ACTS algorithms is foreseen.

# 2. Track Seeding

Based on track hits, short "tracklet" candidates for tracks are constructed using a cellular automaton

# 3. Track following and fitting

A Kalman Filter is foreseen to find the full track and estimate the track parameters.


