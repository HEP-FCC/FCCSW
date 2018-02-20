#include "BarrelLayerGraphTool6.h"

#include "RecInterface/ILayerGraphTool.h"

// TrickTrack headers
#include "tricktrack/CMGraph.h"

DECLARE_TOOL_FACTORY(BarrelLayerGraphTool6)

BarrelLayerGraphTool6::BarrelLayerGraphTool6(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ILayerGraphTool>(this);
}

StatusCode BarrelLayerGraphTool6::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}

StatusCode BarrelLayerGraphTool6::finalize() { return GaudiTool::finalize(); }

/// Construct and return the layergraph for the Cellular automaton,
// for the inner four barrel layers only
// the interface to the layer graph is likely to be changed
tricktrack::CMGraph BarrelLayerGraphTool6::getGraph() {
  
  auto g = tricktrack::CMGraph();

  constexpr unsigned int numberOfHits = 1000; // used to resize vector "isOuterHitOfCell""
  auto l1 = tricktrack::CMLayer("innerLayer", numberOfHits);
  auto l2 = tricktrack::CMLayer("middleLayer", numberOfHits);
  auto l3 = tricktrack::CMLayer("outerLayer", numberOfHits);
  auto l4 = tricktrack::CMLayer("outermostLayer", numberOfHits);
  auto l5 = tricktrack::CMLayer("outeroutermostLayer", numberOfHits);
  auto l6 = tricktrack::CMLayer("outerouteroutermostLayer", numberOfHits);

  auto lp1 = tricktrack::CMLayerPair(0, 1);
  auto lp2 = tricktrack::CMLayerPair(1, 2);
  auto lp3 = tricktrack::CMLayerPair(2, 3);
  auto lp4 = tricktrack::CMLayerPair(3, 4);
  auto lp5 = tricktrack::CMLayerPair(4, 5);
  l1.theOuterLayers.push_back(1);
  l2.theInnerLayers.push_back(0);
  l2.theOuterLayers.push_back(2);
  l3.theInnerLayers.push_back(1);
  l3.theOuterLayers.push_back(3);
  l4.theInnerLayers.push_back(2);
  l4.theOuterLayers.push_back(4);
  l5.theInnerLayers.push_back(3);
  l5.theOuterLayers.push_back(5);
  l6.theInnerLayers.push_back(2);
  l1.theOuterLayerPairs.push_back(0);
  l2.theInnerLayerPairs.push_back(0);
  l2.theOuterLayerPairs.push_back(1);
  l3.theInnerLayerPairs.push_back(1);
  l3.theOuterLayerPairs.push_back(2);
  l4.theInnerLayerPairs.push_back(2);
  l4.theOuterLayerPairs.push_back(3);
  l5.theInnerLayerPairs.push_back(3);
  l5.theOuterLayerPairs.push_back(4);
  l6.theInnerLayerPairs.push_back(4);

  auto theLayerGraph = tricktrack::CMGraph();
  g.theLayers.push_back(l1);
  g.theLayers.push_back(l2);
  g.theLayers.push_back(l3);
  g.theLayers.push_back(l4);
  g.theLayers.push_back(l5);
  g.theLayers.push_back(l6);
  g.theLayerPairs.push_back(lp1);
  g.theLayerPairs.push_back(lp2);
  g.theLayerPairs.push_back(lp3);
  g.theLayerPairs.push_back(lp4);
  g.theLayerPairs.push_back(lp5);
  g.theRootLayers.push_back(0);

  return g;
  }

