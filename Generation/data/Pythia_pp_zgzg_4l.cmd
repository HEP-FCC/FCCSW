!
! File: Pythia_LHEinput.cmd 
!
! This file contains commands to be read in for a Pythia8 run.
! Lines not beginning with a letter or digit are comments.
! Names are case-insensitive  -  but spellings-sensitive!
! Adjusted from Pythia example: main42.cmnd

! 1) Settings that will be used in a main program.
Main:numberOfEvents = 1            ! number of events to generate
Main:timesAllowErrors = 3          ! abort run after this many flawed events
#Random:seed = 1234                ! initialize random generator with a seed


! 2) Settings related to output in init(), next() and stat() functions.
Init:showChangedSettings = on      ! list changed settings
Init:showAllSettings = off         ! list all settings
Init:showChangedParticleData = on  ! list changed particle data
Init:showAllParticleData = off     ! list all particle data
Next:numberCount = 10              ! print message every n events
Next:numberShowLHA = 1             ! print LHA information n times
Next:numberShowInfo = 1            ! print event information n times
Next:numberShowProcess = 1         ! print process record n times
Next:numberShowEvent = 1           ! print event record n times
Stat:showPartonLevel = off         ! additional statistics on MPI

! 3b) PDF settings. Default is to use internal PDFs
! some pdf sets examples: cteq61.LHpdf cteq61.LHgrid MRST2004nlo.LHgrid
#PDF:pSet = LHAPDF5:MRST2001lo.LHgrid
! Allow extrapolation of PDF's beyond x and Q2 boundaries, at own risk.
! Default behaviour is to freeze PDF's at boundaries.
#PDF:extrapolate = on

! 4) Read-in Les Houches Event file - alternative beam and process selection.
Beams:frameType = 4                      ! read info from a LHEF
Beams:LHEF = /afs/cern.ch/user/s/selvaggi/public/tutorialFCC/pp_zgzg_4l.lhe

! 5) Other settings. Can be expanded as desired.
! Note: may overwrite some of the values above, so watch out.
