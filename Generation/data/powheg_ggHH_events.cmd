! 1) Settings that will be used in a main program.
Main:numberOfEvents = 1            ! number of events to generate
Main:timesAllowErrors = 100        ! abort run after this many flawed events
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

! 3) Tell Pythia that LHEF input is used
Beams:frameType             = 4

! Number of outgoing particles of POWHEG Born level process
! (i.e. not counting additional POWHEG radiation)

25:onMode = off
25:onIfMatch = 5 -5
25:onIfMatch = 22 22
ResonanceDecayFilter:filter = on
ResonanceDecayFilter:exclusive = on
ResonanceDecayFilter:mothers = 25
ResonanceDecayFilter:daughters = 5,5,22,22


POWHEG:nFinal = 2
POWHEG:veto = 1
POWHEG:vetoCount = 3
POWHEG:pThard = 0
POWHEG:pTemt = 0
POWHEG:emitted = 0
POWHEG:pTdef = 1
POWHEG:MPIveto = 0



!Beams:LHEF = pwgevents-0001.lhe
Beams:LHEF = /afs/cern.ch/work/s/selvaggi/public/PowhegWrapper/POWHEG-BOX-V2/ggHH/test-pythia8/pwgevents.lhe
Random:seed = 1
