#
# Official Delphes card prepared by FCC-hh collaboration
#
#  Main authors: Heather Gray (CERN)
#                Filip Moortgat (CERN)
#                Michele Selvaggi (UCL)
#
#  Released on: Mar 04th, 2016
#
#  Version: v01
#
#  Configuration: FCC-hh generic detector with dipole in the forward region, no pile-up
#
#
#######################################
# Order of execution of various modules
#######################################

set ExecutionPath {
  ParticlePropagator

  ChargedHadronTrackingEfficiency
  ElectronTrackingEfficiency
  MuonTrackingEfficiency

  ChargedHadronMomentumSmearing
  ElectronMomentumSmearing
  MuonMomentumSmearing

  TrackMerger
  
  ECal
  HCal

  TowerMerger
  EFlowMerger

  PhotonEfficiency
  PhotonIsolation

  ElectronFilter
  ElectronIsolation

  MuonIsolation

  NeutrinoFilter

  MissingET
  GenMissingET

  GenJetFinder
  FastJetFinder

  JetEnergyScale

  JetFlavorAssociation

  BTagging
  CTagging
  TauTagging

  ScalarHT

  UniqueObjectFinder

  TreeWriter
}

#################################
# Propagate particles in cylinder
#################################

module ParticlePropagator ParticlePropagator {
  set InputArray Delphes/stableParticles

  set OutputArray stableParticles
  set ChargedHadronOutputArray chargedHadrons
  set ElectronOutputArray electrons
  set MuonOutputArray muons

  # radius of the magnetic field coverage, in m
  set Radius 6.00
  # half-length of the magnetic field coverage, in m
  set HalfLength 11.5

  # magnetic field
  set Bz 6.0
}

####################################
# Charged hadron tracking efficiency
####################################

module Efficiency ChargedHadronTrackingEfficiency {
  set InputArray ParticlePropagator/chargedHadrons
  set OutputArray chargedHadrons

 # tracking efficiency formula for charged hadrons
  set EfficiencyFormula { (pt <= 0.5) * (0.00) + \
(abs(eta) <= 1.5) * (pt > 0.5 && pt <= 1) * (0.90) + \
(abs(eta) <= 1.5) * (pt > 1) * (0.95) + \
(abs(eta) > 1.5 && abs(eta) <= 4) * (pt > 0.5 && pt <= 1) * (0.85) + \
(abs(eta) > 1.5 && abs(eta) <= 4) * (pt > 1) * (0.90) + \
(abs(eta) > 4 && abs(eta) <= 6) * (pt > 0.5 && pt <= 1) * (0.80) + \
(abs(eta) > 4 && abs(eta) <= 6) * (pt > 1.0) * (0.85) + \
(abs(eta) > 6.0) * (0.00)}

}  

##############################
# Electron tracking efficiency
##############################

module Efficiency ElectronTrackingEfficiency {
  set InputArray ParticlePropagator/electrons
  set OutputArray electrons

# tracking efficiency formula for electrons
set EfficiencyFormula { (pt <= 10) * (0.00) + \
(abs(eta) <= 1.5) * (pt > 10 && pt <= 50) * (0.80) + \
(abs(eta) <= 1.5) * (pt > 50) * (0.90) + \
(abs(eta) > 1.5 && abs(eta) <= 4) * (pt > 10 && pt <= 50) * (0.80) + \
(abs(eta) > 1.5 && abs(eta) <= 4) * (pt > 50) * (0.90) + \
(abs(eta) > 4 && abs(eta) <= 6) * (pt > 10 && pt <= 50) * (0.70) + \
(abs(eta) > 4 && abs(eta) <= 6) * (pt > 50) * (0.80) + \
(abs(eta) > 6) * (0.00)}

}
##########################
# Muon tracking efficiency
##########################

module Efficiency MuonTrackingEfficiency {
  set InputArray ParticlePropagator/muons
  set OutputArray muons

 # tracking efficiency formula for muons
set EfficiencyFormula { (pt <= 10) * (0.00) + \
(abs(eta) <= 1.5) * (pt > 10 ) * (0.99) + \
(abs(eta) <= 4 & abs(eta) > 1.5) * (pt > 10 ) * (0.99) + \
(abs(eta) > 4) * (0.00)}
}

########################################
# Momentum resolution for charged tracks
########################################

module MomentumSmearing ChargedHadronMomentumSmearing {
  set InputArray ChargedHadronTrackingEfficiency/chargedHadrons
  set OutputArray chargedHadrons

# set ResolutionFormula {resolution formula as a function of eta and pt}

# resolution formula for charged hadrons
#
# Automatically generated tracker resolution formula for layout: FCChh_Option2 (by Z. Drasal, CERN)
#
# Central and Dipole
#
set ResolutionFormula {    (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00194513) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.001945 + (pt-5.000000)* 0.000003) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.001962 + (pt-10.000000)* 0.000006) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.002547 + (pt-100.000000)* 0.000014) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.014708 + (pt-1000.000000)* 0.000015) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 10000.0000) * (0.145553*pt/10000.000000) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00247067) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002471 + (pt-5.000000)* -0.000002) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002462 + (pt-10.000000)* 0.000008) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.003170 + (pt-100.000000)* 0.000015) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.016485 + (pt-1000.000000)* 0.000016) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 10000.0000) * (0.162501*pt/10000.000000) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00242524) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002425 + (pt-5.000000)* 0.000013) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002490 + (pt-10.000000)* 0.000042) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.006273 + (pt-100.000000)* 0.000049) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.050079 + (pt-1000.000000)* 0.000050) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 10000.0000) * (0.498773*pt/10000.000000) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00168511) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.001685 + (pt-5.000000)* 0.000115) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002262 + (pt-10.000000)* 0.000167) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.017274 + (pt-100.000000)* 0.000172) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.172095 + (pt-1000.000000)* 0.000172) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 10000.0000) * (1.720887*pt/10000.000000) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00277124) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002771 + (pt-5.000000)* 0.000430) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.004921 + (pt-10.000000)* 0.000467) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.046933 + (pt-100.000000)* 0.000469) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.469096 + (pt-1000.000000)* 0.000469) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 10000.0000) * (4.690935*pt/10000.000000) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00653887) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.006539 + (pt-5.000000)* 0.001256) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.012821 + (pt-10.000000)* 0.001272) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.127345 + (pt-100.000000)* 0.001273) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (1.273365 + (pt-1000.000000)* 0.001273) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 10000.0000) * (12.733637*pt/10000.000000)
}
}


###################################
# Momentum resolution for electrons
###################################

module MomentumSmearing ElectronMomentumSmearing {
  set InputArray ElectronTrackingEfficiency/electrons
  set OutputArray electrons

 # set ResolutionFormula {resolution formula as a function of eta and pt}

# resolution formula for muons
# Automatically generated tracker resolution formula for layout: FCChh_Option2 (by Z. Drasal, CERN)
# Central and Dipole
#
set ResolutionFormula {    (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00194513) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.001945 + (pt-5.000000)* 0.000003) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.001962 + (pt-10.000000)* 0.000006) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.002547 + (pt-100.000000)* 0.000014) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.014708 + (pt-1000.000000)* 0.000015) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 10000.0000) * (0.145553*pt/10000.000000) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00247067) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002471 + (pt-5.000000)* -0.000002) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002462 + (pt-10.000000)* 0.000008) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.003170 + (pt-100.000000)* 0.000015) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.016485 + (pt-1000.000000)* 0.000016) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 10000.0000) * (0.162501*pt/10000.000000) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00242524) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002425 + (pt-5.000000)* 0.000013) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002490 + (pt-10.000000)* 0.000042) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.006273 + (pt-100.000000)* 0.000049) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.050079 + (pt-1000.000000)* 0.000050) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 10000.0000) * (0.498773*pt/10000.000000) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00168511) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.001685 + (pt-5.000000)* 0.000115) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002262 + (pt-10.000000)* 0.000167) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.017274 + (pt-100.000000)* 0.000172) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.172095 + (pt-1000.000000)* 0.000172) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 10000.0000) * (1.720887*pt/10000.000000) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00277124) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002771 + (pt-5.000000)* 0.000430) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.004921 + (pt-10.000000)* 0.000467) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.046933 + (pt-100.000000)* 0.000469) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.469096 + (pt-1000.000000)* 0.000469) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 10000.0000) * (4.690935*pt/10000.000000) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00653887) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.006539 + (pt-5.000000)* 0.001256) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.012821 + (pt-10.000000)* 0.001272) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.127345 + (pt-100.000000)* 0.001273) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (1.273365 + (pt-1000.000000)* 0.001273) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 10000.0000) * (12.733637*pt/10000.000000)
}
}

###############################
# Momentum resolution for muons
###############################

module MomentumSmearing MuonMomentumSmearing {
  set InputArray MuonTrackingEfficiency/muons
  set OutputArray muons

# set ResolutionFormula {resolution formula as a function of eta and pt}

# resolution formula for muons
# Automatically generated tracker resolution formula for layout: FCChh_Option2 (by Z. Drasal, CERN)
# Central and Dipole
#
set ResolutionFormula {    (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00194513) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.001945 + (pt-5.000000)* 0.000003) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.001962 + (pt-10.000000)* 0.000006) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.002547 + (pt-100.000000)* 0.000014) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.014708 + (pt-1000.000000)* 0.000015) + \
   (abs(eta) >= 0.0000 && abs(eta) < 1.0000) * (pt >= 10000.0000) * (0.145553*pt/10000.000000) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00247067) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002471 + (pt-5.000000)* -0.000002) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002462 + (pt-10.000000)* 0.000008) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.003170 + (pt-100.000000)* 0.000015) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.016485 + (pt-1000.000000)* 0.000016) + \
   (abs(eta) >= 1.0000 && abs(eta) < 2.0000) * (pt >= 10000.0000) * (0.162501*pt/10000.000000) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00242524) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002425 + (pt-5.000000)* 0.000013) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002490 + (pt-10.000000)* 0.000042) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.006273 + (pt-100.000000)* 0.000049) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.050079 + (pt-1000.000000)* 0.000050) + \
   (abs(eta) >= 2.0000 && abs(eta) < 3.0000) * (pt >= 10000.0000) * (0.498773*pt/10000.000000) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00168511) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.001685 + (pt-5.000000)* 0.000115) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.002262 + (pt-10.000000)* 0.000167) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.017274 + (pt-100.000000)* 0.000172) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.172095 + (pt-1000.000000)* 0.000172) + \
   (abs(eta) >= 3.0000 && abs(eta) < 4.0000) * (pt >= 10000.0000) * (1.720887*pt/10000.000000) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00277124) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.002771 + (pt-5.000000)* 0.000430) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.004921 + (pt-10.000000)* 0.000467) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.046933 + (pt-100.000000)* 0.000469) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (0.469096 + (pt-1000.000000)* 0.000469) + \
   (abs(eta) >= 4.0000 && abs(eta) < 5.0000) * (pt >= 10000.0000) * (4.690935*pt/10000.000000) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 0.0000 && pt < 5.0000) * (0.00653887) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 5.0000 && pt < 10.0000) * (0.006539 + (pt-5.000000)* 0.001256) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 10.0000 && pt < 100.0000) * (0.012821 + (pt-10.000000)* 0.001272) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 100.0000 && pt < 1000.0000) * (0.127345 + (pt-100.000000)* 0.001273) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 1000.0000 && pt < 10000.0000) * (1.273365 + (pt-1000.000000)* 0.001273) + \
   (abs(eta) >= 5.0000 && abs(eta) < 6.0000) * (pt >= 10000.0000) * (12.733637*pt/10000.000000)
}
}

##############
# Track merger
##############

module Merger TrackMerger {
# add InputArray InputArray
  add InputArray ChargedHadronMomentumSmearing/chargedHadrons
  add InputArray ElectronMomentumSmearing/electrons
  add InputArray MuonMomentumSmearing/muons
  set OutputArray tracks
}

#############
#   ECAL
#############

module SimpleCalorimeter ECal {
  set ParticleInputArray ParticlePropagator/stableParticles
  set TrackInputArray TrackMerger/tracks

  set TowerOutputArray ecalTowers
  set EFlowTrackOutputArray eflowTracks
  set EFlowTowerOutputArray eflowPhotons

  set IsEcal true 
 
  set EnergyMin 0.5
  set EnergySignificanceMin 1.0

  set SmearTowerCenter true

  set pi [expr {acos(-1)}]

  # lists of the edges of each tower in eta and phi
  # each list starts with the lower edge of the first tower
  # the list ends with the higher edged of the last tower

  # 0.5 degree towers (5x5 mm^2)
   set PhiBins {}
  for {set i -256} {$i <= 256} {incr i} {
    add PhiBins [expr {$i * $pi/256.0}]
  }
  foreach eta {-2.5 -2.4875 -2.475 -2.4625 -2.45 -2.4375 -2.425 -2.4125 -2.4 -2.3875 -2.375 -2.3625 -2.35 -2.3375 -2.325 -2.3125 -2.3 -2.2875 -2.275 -2.2625 -2.25 -2.2375 -2.225 -2.2125 -2.2 -2.1875 -2.175 -2.1625 -2.15 -2.1375 -2.125 -2.1125 -2.1 -2.0875 -2.075 -2.0625 -2.05 -2.0375 -2.025 -2.0125 -2 -1.9875 -1.975 -1.9625 -1.95 -1.9375 -1.925 -1.9125 -1.9 -1.8875 -1.875 -1.8625 -1.85 -1.8375 -1.825 -1.8125 -1.8 -1.7875 -1.775 -1.7625 -1.75 -1.7375 -1.725 -1.7125 -1.7 -1.6875 -1.675 -1.6625 -1.65 -1.6375 -1.625 -1.6125 -1.6 -1.5875 -1.575 -1.5625 -1.55 -1.5375 -1.525 -1.5125 -1.5 -1.4875 -1.475 -1.4625 -1.45 -1.4375 -1.425 -1.4125 -1.4 -1.3875 -1.375 -1.3625 -1.35 -1.3375 -1.325 -1.3125 -1.3 -1.2875 -1.275 -1.2625 -1.25 -1.2375 -1.225 -1.2125 -1.2 -1.1875 -1.175 -1.1625 -1.15 -1.1375 -1.125 -1.1125 -1.1 -1.0875 -1.075 -1.0625 -1.05 -1.0375 -1.025 -1.0125 -1.0 -0.9875 -0.975 -0.9625 -0.95 -0.9375 -0.925 -0.9125 -0.9 -0.8875 -0.875 -0.8625 -0.85 -0.8375 -0.825 -0.8125 -0.8 -0.7875 -0.775 -0.7625 -0.75 -0.7375 -0.725 -0.7125 -0.7 -0.6875 -0.675 -0.6625 -0.65 -0.6375 -0.625 -0.6125 -0.6 -0.5875 -0.575 -0.5625 -0.55 -0.5375 -0.525 -0.5125 -0.5 -0.4875 -0.475 -0.4625 -0.45 -0.4375 -0.425 -0.4125 -0.4 -0.3875 -0.375 -0.3625 -0.35 -0.3375 -0.325 -0.3125 -0.3 -0.2875 -0.275 -0.2625 -0.25 -0.2375 -0.225 -0.2125 -0.2 -0.1875 -0.175 -0.1625 -0.15 -0.1375 -0.125 -0.1125 -0.1 -0.0875 -0.075 -0.0625 -0.05 -0.0375 -0.025 -0.0125 0 0.0125 0.025 0.0375 0.05 0.0625 0.075 0.0875 0.1 0.1125 0.125 0.1375 0.15 0.1625 0.175 0.1875 0.2 0.2125 0.225 0.2375 0.25 0.2625 0.275 0.2875 0.3 0.3125 0.325 0.3375 0.35 0.3625 0.375 0.3875 0.4 0.4125 0.425 0.4375 0.45 0.4625 0.475 0.4875 0.5 0.5125 0.525 0.5375 0.55 0.5625 0.575 0.5875 0.6 0.6125 0.625 0.6375 0.65 0.6625 0.675 0.6875 0.7 0.7125 0.725 0.7375 0.75 0.7625 0.775 0.7875 0.8 0.8125 0.825 0.8375 0.85 0.8625 0.875 0.8875 0.9 0.9125 0.925 0.9375 0.95 0.9625 0.975 0.9875 1.0 1.0125 1.025 1.0375 1.05 1.0625 1.075 1.0875 1.1 1.1125 1.125 1.1375 1.15 1.1625 1.175 1.1875 1.2 1.2125 1.225 1.2375 1.25 1.2625 1.275 1.2875 1.3 1.3125 1.325 1.3375 1.35 1.3625 1.375 1.3875 1.4 1.4125 1.425 1.4375 1.45 1.4625 1.475 1.4875 1.5 1.5125 1.525 1.5375 1.55 1.5625 1.575 1.5875 1.6 1.6125 1.625 1.6375 1.65 1.6625 1.675 1.6875 1.7 1.7125 1.725 1.7375 1.75 1.7625 1.775 1.7875 1.8 1.8125 1.825 1.8375 1.85 1.8625 1.875 1.8875 1.9 1.9125 1.925 1.9375 1.95 1.9625 1.975 1.9875 2 2.0125 2.025 2.0375 2.05 2.0625 2.075 2.0875 2.1 2.1125 2.125 2.1375 2.15 2.1625 2.175 2.1875 2.2 2.2125 2.225 2.2375 2.25 2.2625 2.275 2.2875 2.3 2.3125 2.325 2.3375 2.35 2.3625 2.375 2.3875 2.4 2.4125 2.425 2.4375 2.45 2.4625 2.475 2.4875 2.5} {
    add EtaPhiBins $eta $PhiBins
  }
  # 0.025 eta  x 0.025 for eta between 2.5 and 4.0
  set PhiBins {}
  for {set i -128} {$i <= 128} {incr i} {
    add PhiBins [expr {$i * $pi/128.0}]
  }
  foreach eta {-4.0 -3.975 -3.95 -3.925 -3.9 -3.875 -3.85 -3.825 -3.8 -3.775 -3.75 -3.725 -3.7 -3.675 -3.65 -3.625 -3.6 -3.575 -3.55 -3.525 -3.5 -3.475 -3.45 -3.425 -3.4 -3.375 -3.35 -3.325 -3.3 -3.275 -3.25 -3.225 -3.2 -3.175 -3.15 -3.125 -3.1 -3.075 -3.05 -3.025 -3.0 -2.975 -2.95 -2.925 -2.9 -2.875 -2.85 -2.825 -2.8 -2.775 -2.75 -2.725 -2.7 -2.675 -2.65 -2.625 -2.6 -2.575 -2.55 -2.525 2.525 2.55 2.575 2.6 2.625 2.65 2.675 2.7 2.725 2.75 2.775 2.8 2.825 2.85 2.875 2.9 2.925 2.95 2.975 3.0 3.025 3.05 3.075 3.1 3.125 3.15 3.175 3.2 3.225 3.25 3.275 3.3 3.325 3.35 3.375 3.4 3.425 3.45 3.475 3.5 3.525 3.55 3.575 3.6 3.625 3.65 3.675 3.7 3.725 3.75 3.775 3.8 3.825 3.85 3.875 3.9 3.925 3.95 3.975 4.0} {
    add EtaPhiBins $eta $PhiBins
  }

  # 0.05 x 0.05 for eta between 4.0 and 6.0
  set PhiBins {}
  for {set i -64} {$i <= 64} {incr i} {
    add PhiBins [expr {$i * $pi/64.0}] 
  }
  foreach eta {-6.0 -5.95 -5.9 -5.85 -5.8 -5.75 -5.7 -5.65 -5.6 -5.55 -5.5 -5.45 -5.4 -5.35 -5.3 -5.25 -5.2 -5.15 -5.1 -5.05 -5.0 -4.95 -4.9 -4.85 -4.8 -4.75 -4.7 -4.65 -4.6 -4.55 -4.5 -4.45 -4.4 -4.35 -4.3 -4.25 -4.2 -4.15 -4.1 -4.05 4.05 4.1 4.15 4.2 4.25 4.3 4.35 4.4 4.45 4.5 4.55 4.6 4.65 4.7 4.75 4.8 4.85 4.9 4.95 5.0 5.05 5.1 5.15 5.2 5.25 5.3 5.35 5.4 5.45 5.5 5.55 5.6 5.65 5.7 5.75 5.8 5.85 5.9 5.95 6.0} {
    add EtaPhiBins $eta $PhiBins
  }
  # default energy fractions {abs(PDG code)} {fraction of energy deposited in ECAL}
  
  add EnergyFraction {0} {0.4}
  # energy fractions for e, gamma and pi0
  add EnergyFraction {11} {1.0}
  add EnergyFraction {22} {1.0}
  add EnergyFraction {111} {1.0}
  # energy fractions for muon, neutrinos and neutralinos
  add EnergyFraction {12} {0.0}
  add EnergyFraction {13} {0.0}
  add EnergyFraction {14} {0.0}
  add EnergyFraction {16} {0.0}
  add EnergyFraction {1000022} {0.0}
  add EnergyFraction {1000023} {0.0}
  add EnergyFraction {1000025} {0.0}
  add EnergyFraction {1000035} {0.0}
  add EnergyFraction {1000045} {0.0}
  # energy fractions for K0short and Lambda
  # add EnergyFraction {310} {0.3}
  # add EnergyFraction {3122} {0.3}

  # set ECalResolutionFormula {resolution formula as a function of eta and energy}
  set ResolutionFormula {                     (abs(eta) <= 4.0) * sqrt(energy^2*0.01^2 + energy*0.10^2) + \
                            (abs(eta) > 4.0 && abs(eta) <= 6.0) * sqrt(energy^2*0.01^2  + energy*0.10^2)}

 
}

#############
#   HCAL
#############

module SimpleCalorimeter HCal {
  set ParticleInputArray ParticlePropagator/stableParticles
  set TrackInputArray ECal/eflowTracks

  set TowerOutputArray hcalTowers
  set EFlowTrackOutputArray eflowTracks
  set EFlowTowerOutputArray eflowNeutralHadrons

  set IsEcal false 
 
  set EnergyMin 1.0
  set EnergySignificanceMin 1.0

  set SmearTowerCenter true

  set pi [expr {acos(-1)}]

  # lists of the edges of each tower in eta and phi
  # each list starts with the lower edge of the first tower
  # the list ends with the higher edged of the last tower


  # 6 degree towers
  set PhiBins {}
  for {set i -128} {$i <= 128} {incr i} {
    add PhiBins [expr {$i * $pi/128.0}]
  } 
  foreach eta {-2.5 -2.45 -2.4 -2.35 -2.3 -2.25 -2.2 -2.15 -2.1 -2.05 -2.0 -1.95 -1.9 -1.85 -1.8 -1.75 -1.7 -1.65 -1.6 -1.55 -1.5 -1.45 -1.4 -1.35 -1.3 -1.25 -1.2 -1.15 -1.1 -1.05 -1.0 -0.95 -0.9 -0.85 -0.8 -0.75 -0.7 -0.65 -0.6 -0.55 -0.5 -0.45 -0.4 -0.35 -0.3 -0.25 -0.2 -0.15 -0.1 -0.05 0.0 0.05 0.1 0.15 0.2 0.25 0.3 0.35 0.4 0.45 0.5 0.55 0.6 0.65 0.7 0.75 0.8 0.85 0.9 0.95 1.0 1.05 1.1 1.15 1.2 1.25 1.3 1.35 1.4 1.45 1.5 1.55 1.6 1.65 1.7 1.75 1.8 1.85 1.9 1.95 2.0 2.05 2.1 2.15 2.2 2.25 2.3 2.35 2.4 2.45 2.5} {
    add EtaPhiBins $eta $PhiBins
  }

  # 0.1 x 0.1 between eta 2.5 and 4.0
  set PhiBins {}
  for {set i -64} {$i <= 64} {incr i} {
    add PhiBins [expr {$i * $pi/64.0}]
  }
  foreach eta {-4.0 -3.9 -3.8 -3.7 -3.6 -3.5 -3.4 -3.3 -3.2 -3.1 -3.0 -2.9 -2.8 -2.7 -2.6 2.6 2.7 2.8 2.9 3.0 3.1 3.2 3.3 3.4 3.5 3.6 3.7 3.8 3.9 4.0} {
    add EtaPhiBins $eta $PhiBins
  }

  # 0.2 x 0.2 between eta 4.0 and 6.0
  set PhiBins {}
  for {set i -32} {$i <= 32} {incr i} {
    add PhiBins [expr {$i * $pi/32.0}]
  }
  foreach eta {-6.0 -5.8 -5.6 -5.4 -5.2 -5.0 -4.8 -4.6 -4.4 -4.2 4.2 4.4 4.6 4.8 5.0 5.2 5.4 5.6 5.8 6.0} {
    add EtaPhiBins $eta $PhiBins
  }
  
  # default energy fractions {abs(PDG code)} {Fecal Fhcal}
  add EnergyFraction {0} {0.6}
  # energy fractions for e, gamma and pi0
  add EnergyFraction {11} {0.0}
  add EnergyFraction {22} {0.0}
  add EnergyFraction {111} {0.0}
  # energy fractions for muon, neutrinos and neutralinos
  add EnergyFraction {12} {0.0}
  add EnergyFraction {13} {0.0}
  add EnergyFraction {14} {0.0}
  add EnergyFraction {16} {0.0}
  add EnergyFraction {1000022} {0.0}
  add EnergyFraction {1000023} {0.0}
  add EnergyFraction {1000025} {0.0}
  add EnergyFraction {1000035} {0.0}
  add EnergyFraction {1000045} {0.0}
  # energy fractions for K0short and Lambda
  # add EnergyFraction {310} {0.7}
  # add EnergyFraction {3122} {0.7}

   # set HCalResolutionFormula {resolution formula as a function of eta and energy}
  set ResolutionFormula {                     (abs(eta) <= 4.0) * sqrt(energy^2*0.03^2 + energy*0.50^2) + \
                            (abs(eta) > 4.0 && abs(eta) <= 6.0) * sqrt(energy^2*0.05^2 + energy*1.00^2)}
}

#################
# Electron filter
#################

module PdgCodeFilter ElectronFilter {
  set InputArray HCal/eflowTracks
  set OutputArray electrons
  set Invert true
  add PdgCode {11}
  add PdgCode {-11}
}

###################################################
# Tower Merger (in case not using e-flow algorithm)
###################################################

module Merger TowerMerger {
# add InputArray InputArray
  add InputArray ECal/ecalTowers
  add InputArray HCal/hcalTowers
  set OutputArray towers
}

####################
# Energy flow merger
####################

module Merger EFlowMerger {
# add InputArray InputArray
  add InputArray HCal/eflowTracks
  add InputArray ECal/eflowPhotons
  add InputArray HCal/eflowNeutralHadrons
  set OutputArray eflow
}


###################
# Missing ET merger
###################

module Merger MissingET {
# add InputArray InputArray
  add InputArray EFlowMerger/eflow
  set MomentumOutputArray momentum
}



##################
# Scalar HT merger
##################

module Merger ScalarHT {
# add InputArray InputArray
  add InputArray EFlowMerger/eflow
  set EnergyOutputArray energy
}

#################
# Neutrino Filter
#################

module PdgCodeFilter NeutrinoFilter {

  set InputArray Delphes/stableParticles
  set OutputArray filteredParticles

  set PTMin 0.0

  add PdgCode {12}
  add PdgCode {14}
  add PdgCode {16}
  add PdgCode {-12}
  add PdgCode {-14}
  add PdgCode {-16}

}


#####################
# MC truth jet finder
#####################

module FastJetFinder GenJetFinder {
#  set InputArray NeutrinoFilter/filteredParticles
  set InputArray Delphes/stableParticles

  set OutputArray jets

  # algorithm: 1 CDFJetClu, 2 MidPoint, 3 SIScone, 4 kt, 5 Cambridge/Aachen, 6 antikt
  set JetAlgorithm 7
  set ParameterR 0.4

  set JetPTMin 5.0
}

#########################
# Gen Missing ET merger
########################

module Merger GenMissingET {

# add InputArray InputArray
  add InputArray NeutrinoFilter/filteredParticles
  set MomentumOutputArray momentum
}



############
# Jet finder
############

module FastJetFinder FastJetFinder {
#  set InputArray TowerMerger/towers
  set InputArray EFlowMerger/eflow

  set OutputArray jets

  # algorithm: 1 CDFJetClu, 2 MidPoint, 3 SIScone, 4 kt, 5 Cambridge/Aachen, 6 antikt
  # 7: anti-kt with winner-take-all axis (for N-subjettiness), 8 N-jettiness
  set JetAlgorithm 7
  set ParameterR 0.4

  set JetPTMin 30.0
}

##################
# Jet Energy Scale
##################

module EnergyScale JetEnergyScale {
  set InputArray FastJetFinder/jets
  set OutputArray jets

 # scale formula for jets
  set ScaleFormula {1.00}
}


########################
# Jet Flavor Association
########################

module JetFlavorAssociation JetFlavorAssociation {

  set PartonInputArray Delphes/partons
  set ParticleInputArray Delphes/allParticles
  set ParticleLHEFInputArray Delphes/allParticlesLHEF
  set JetInputArray JetEnergyScale/jets

  set DeltaR 0.5
  set PartonPTMin 5.0
  set PartonEtaMax 6.0

}

###################
# Photon efficiency
###################

module Efficiency PhotonEfficiency {
  set InputArray ECal/eflowPhotons
  set OutputArray photons

  # set EfficiencyFormula {efficiency formula as a function of eta and pt}

  set EfficiencyFormula {                                      
  (pt <= 10.0) * (0.00) + \
  (abs(eta) <= 1.5) * (pt > 10.0)  * (0.95) + \
  (abs(eta) > 1.5 && abs(eta) <= 4.0) * (pt > 10.0)  * (0.90) + \
  (abs(eta) > 4.0 && abs(eta) <= 6.0) * (pt > 10.0)  * (0.80) + \
  (abs(eta) > 6.0) * (0.00)}

}

##################
# Photon isolation
##################

module Isolation PhotonIsolation {
  set CandidateInputArray PhotonEfficiency/photons
  set IsolationInputArray EFlowMerger/eflow

  set OutputArray photons

  set DeltaRMax 0.3

  set PTMin 0.5

  set PTRatioMax 0.1
}


####################
# Electron isolation
####################

module Isolation ElectronIsolation {
  set CandidateInputArray ElectronFilter/electrons
  set IsolationInputArray EFlowMerger/eflow

  set OutputArray electrons

  set DeltaRMax 0.3

  set PTMin 0.5

  set PTRatioMax 0.1
}


################
# Muon isolation
################

module Isolation MuonIsolation {
  set CandidateInputArray MuonMomentumSmearing/muons
  set IsolationInputArray EFlowMerger/eflow

  set OutputArray muons

  set DeltaRMax 0.3

  set PTMin 0.5

  set PTRatioMax 0.2
}


###########
# b-tagging
###########

module BTagging BTagging {
  set JetInputArray JetEnergyScale/jets

  set BitNumber 0

  add EfficiencyFormula {0} {0.001} 
  add EfficiencyFormula {4} {0.04} 
  add EfficiencyFormula {5} {0.75}

}

###########
# c-tagging
###########

module BTagging CTagging {
  set JetInputArray JetEnergyScale/jets

  set BitNumber 1

  add EfficiencyFormula {0} {0.01} 
  add EfficiencyFormula {4} {0.10} 
  add EfficiencyFormula {5} {0.25}

}


#############
# tau-tagging
#############


module TauTagging TauTagging {
  set ParticleInputArray Delphes/allParticles
  set PartonInputArray Delphes/partons
  set JetInputArray JetEnergyScale/jets

  set DeltaR 0.5

  set TauPTMin 1.0

  set TauEtaMax 4.0

  # add EfficiencyFormula {abs(PDG code)} {efficiency formula as a function of eta and pt}

  add EfficiencyFormula {0} {0.01} 
  add EfficiencyFormula {11} {0.005} 
  add EfficiencyFormula {15} {0.6} 

}

#####################################################
# Find uniquely identified photons/electrons/tau/jets
#####################################################

module UniqueObjectFinder UniqueObjectFinder {
# earlier arrays take precedence over later ones
# add InputArray InputArray OutputArray
  add InputArray PhotonIsolation/photons photons
  add InputArray ElectronIsolation/electrons electrons
  add InputArray MuonIsolation/muons muons
  add InputArray JetEnergyScale/jets jets
}


##################
# ROOT tree writer
##################

module TreeWriter TreeWriter {
# add Branch InputArray BranchName BranchClass
  add Branch Delphes/allParticles Particle GenParticle
  
  add Branch GenJetFinder/jets GenJet Jet
  add Branch GenMissingET/momentum GenMissingET MissingET

  add Branch HCal/eflowTracks EFlowTrack Track
  add Branch ECal/eflowPhotons EFlowPhoton Tower
  add Branch HCal/eflowNeutralHadrons EFlowNeutralHadron Tower
  
  add Branch UniqueObjectFinder/photons Photon Photon
  add Branch UniqueObjectFinder/electrons Electron Electron
  add Branch UniqueObjectFinder/muons Muon Muon
  add Branch UniqueObjectFinder/jets Jet Jet
  
  add Branch MissingET/momentum MissingET MissingET
  add Branch ScalarHT/energy ScalarHT ScalarHT
}

