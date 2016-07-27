from ROOT import gSystem
from EventStore import EventStore

gSystem.Load("libdatamodelDict")
store = EventStore(["./output.root"])
store_after = EventStore(["./out2.root"])

for iev, event in enumerate(store):
    event_after = store_after[iev]
    hits_before = event.get("hits")
    hits_after = event_after.get('hits')
    assert(len(hits_before) == len(hits_after))
