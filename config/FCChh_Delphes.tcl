#
# FCChh_Delphes.tcl - Delphes configuration card
#
#  By Z.Drasal(CERN)
#

#######################################
# Order of execution of various modules
#######################################

set ExecutionPath {
  ParticlePropagator

  ChargedHadronMomentumSmearing
  ElectronEnergySmearing
  MuonMomentumSmearing

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
  set Radius 2.40
  # half-length of the magnetic field coverage, in m
  set HalfLength 4.00

  # magnetic field
  set Bz 6.0
}

########################################
# Momentum resolution for charged tracks
########################################

module MomentumSmearing ChargedHadronMomentumSmearing {
  set InputArray ParticlePropagator/chargedHadrons
  set OutputArray chargedHadrons

  # set ResolutionFormula {resolution formula as a function of eta and pt}

  # resolution formula for muons
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

#################################
# Energy resolution for electrons
#################################

module EnergySmearing ElectronEnergySmearing {
  set InputArray ParticlePropagator/electrons
  set OutputArray electrons

  # set ResolutionFormula {resolution formula as a function of eta and energy}

  # resolution formula for electrons
  set ResolutionFormula {                  (abs(eta) <= 4.0) * (energy > 0.1   && energy <= 2.0e1) * (energy*0.007) +
                                           (abs(eta) <= 4.0) * (energy > 2.0e1)                    * sqrt(energy^2*0.005^2 + energy*0.02^2) +
                                           (abs(eta) > 4.0 && abs(eta) <= 6.0)                     * sqrt(energy^2*0.05^2 + energy*1.00^2)}

}

###############################
# Momentum resolution for muons
###############################

module MomentumSmearing MuonMomentumSmearing {
  set InputArray ParticlePropagator/muons
  set OutputArray muons

  # set ResolutionFormula {resolution formula as a function of eta and pt}

  # resolution formula for muons
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
  add InputArray ElectronEnergySmearing/electrons
  add InputArray MuonMomentumSmearing/muons
  set OutputArray tracks
}

##################
# ROOT tree writer
##################

module TreeWriter TreeWriter {
# add Branch InputArray BranchName BranchClass
  add Branch Delphes/allParticles AllParticles GenParticle
  add Branch Delphes/stableParticles StableParticles GenParticle  

  add Branch ChargedHadronMomentumSmearing/chargedHadrons ChargedHadrons Track
  #add Branch ElectronEnergySmearing/electrons Electrons Electron 
  #add Branch MuonMomentumSmearing/muons Muons Muon 
}
