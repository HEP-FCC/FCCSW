from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
storeWithNoise = EventStore(['output_ecalReco_flatNoise_test.root'])
storeNoNoise = EventStore(['output_ecalReco_noNoise_test.root'])

for event in storeWithNoise:
    clusters = event.get('caloClusters')
    assert(len(clusters) == 1)

for event in storeNoNoise:
    clusters = event.get('caloClusters')
    assert(len(clusters) == 1)
