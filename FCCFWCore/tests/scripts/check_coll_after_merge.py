from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store_signal = EventStore(["./output_geant_pgun_fullsim.root"])
store_pileup = EventStore(["./tracker_with_field.root"])
store_merged = EventStore(["./output_overlaid_with_pileup.root"])


assert( len(store_merged) <= len(store_signal) )

# assuming that the pileup read wasn't randomized for this test
# and that just one pileup event was overlaid, as in the current overlay example
for iev in range(len(store_merged)):
    event_signal = store_signal[iev]
    event_pileup = store_pileup[iev]
    event_merged = store_merged[iev]

    particles_signal = event_signal.get('allGenParticles')
    particles_pileup = event_pileup.get('allGenParticles')
    particles_merged = event_merged.get("overlaidGenParticles")

    # check that the right number of particles are present
    assert(len(particles_merged) == len(particles_signal) + len(particles_pileup))

    total_px_signal = 0
    total_px_pileup = 0
    total_px_merged = 0

    for i in range(len(particles_signal)):
        s = particles_signal[i]
        total_px_signal += s.core().p4.px
    for i in range(len(particles_pileup)):
        p = particles_pileup[i]
        total_px_pileup += p.core().p4.px
    for i in range(len(particles_merged)):
        m = particles_merged[i]
        total_px_merged += m.core().p4.px
    
    # check that we merged the right particles
    assert( total_px_merged - (total_px_signal + total_px_pileup) < 1e-10 )
      
