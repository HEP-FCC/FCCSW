#include "BarrelLayerGraphTool.h"

#include "RecInterface/ILayerGraphTool.h"

// TrickTrack headers
#include "tricktrack/CMGraph.h"

DECLARE_TOOL_FACTORY(BarrelLayerGraphTool)

BarrelLayerGraphTool::BarrelLayerGraphTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ILayerGraphTool>(this);
}

StatusCode BarrelLayerGraphTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}

StatusCode BarrelLayerGraphTool::finalize() { return GaudiTool::finalize(); }

/// Construct and return the layergraph for the Cellular automaton,
// for the inner four barrel layers only
// the interface to the layer graph is likely to be changed
tricktrack::CMGraph BarrelLayerGraphTool::getGraph() {
  
  auto g = tricktrack::CMGraph();

  constexpr unsigned int numberOfHits = 1000; // used to resize vector "isOuterHitOfCell""
  auto lb0 = tricktrack::CMLayer("innerLayer", numberOfHits);
  auto lb1 = tricktrack::CMLayer("middleLayer", numberOfHits);
  auto lb2 = tricktrack::CMLayer("outerLayer", numberOfHits);
  auto lb3 = tricktrack::CMLayer("outermostLayer", numberOfHits);
  auto le4 = tricktrack::CMLayer("firstEndcap", numberOfHits);
  auto le5 = tricktrack::CMLayer("secondEndcap", numberOfHits);
  auto le6 = tricktrack::CMLayer("thirdEndcap", numberOfHits);
  auto le7 = tricktrack::CMLayer("fourthEndcap", numberOfHits);

  // barrel - barrel
  auto lbb0 = tricktrack::CMLayerPair(0, 1);
  auto lbb1 = tricktrack::CMLayerPair(1, 2);
  auto lbb2 = tricktrack::CMLayerPair(2, 3);
  // barrel - endcap
  auto lbe3 = tricktrack::CMLayerPair(0, 4);
  auto lbe4 = tricktrack::CMLayerPair(1, 4);
  auto lbe5 = tricktrack::CMLayerPair(2, 4);
  auto lbe6 = tricktrack::CMLayerPair(3, 4);
  // endcap - endcap
  auto lee7 = tricktrack::CMLayerPair(4, 5);
  auto lee8 = tricktrack::CMLayerPair(5, 6);
  auto lee9 = tricktrack::CMLayerPair(6, 7);

  // barrel - barrel
  lb0.theOuterLayers.push_back(1);
  lb1.theInnerLayers.push_back(0);
  lb1.theOuterLayers.push_back(2);
  lb2.theInnerLayers.push_back(1);
  lb2.theOuterLayers.push_back(3);
  lb3.theInnerLayers.push_back(2);
  // barrel - endcap
  le4.theInnerLayers.push_back(0);
  lb0.theOuterLayers.push_back(4);
  le4.theInnerLayers.push_back(1);
  lb1.theOuterLayers.push_back(4);
  le4.theInnerLayers.push_back(2);
  lb2.theOuterLayers.push_back(4);
  le4.theInnerLayers.push_back(3);
  lb3.theOuterLayers.push_back(4);
  // endcap - endcap
  le4.theOuterLayers.push_back(5);
  le5.theInnerLayers.push_back(4);
  le5.theOuterLayers.push_back(6);
  le6.theInnerLayers.push_back(5);
  le6.theOuterLayers.push_back(7);
  le7.theInnerLayers.push_back(6);

  // barrel - barrel
  lb0.theOuterLayerPairs.push_back(0);
  lb1.theInnerLayerPairs.push_back(0);
  lb1.theOuterLayerPairs.push_back(1);
  lb2.theInnerLayerPairs.push_back(1);
  lb2.theOuterLayerPairs.push_back(2);
  lb3.theInnerLayerPairs.push_back(2);
  // barrel - endcap
  lb0.theOuterLayerPairs.push_back(3);
  le4.theInnerLayerPairs.push_back(3);
  lb1.theOuterLayerPairs.push_back(4);
  le4.theInnerLayerPairs.push_back(4);
  lb2.theOuterLayerPairs.push_back(5);
  le4.theInnerLayerPairs.push_back(5);
  lb3.theOuterLayerPairs.push_back(6);
  le4.theInnerLayerPairs.push_back(6);


  //endcap - endcap

  auto theLayerGraph = tricktrack::CMGraph();
  g.theLayers.push_back(lb0);
  g.theLayers.push_back(lb1);
  g.theLayers.push_back(lb2);
  g.theLayers.push_back(lb3);
  g.theLayers.push_back(le4);
  g.theLayers.push_back(le5);
  g.theLayers.push_back(le6);
  g.theLayers.push_back(le7);
  g.theLayerPairs.push_back(lbb0);
  g.theLayerPairs.push_back(lbb1);
  g.theLayerPairs.push_back(lbb2);
  g.theLayerPairs.push_back(lbe3);
  g.theLayerPairs.push_back(lbe4);
  g.theLayerPairs.push_back(lbe5);
  g.theLayerPairs.push_back(lbe6);
  g.theLayerPairs.push_back(lee7);
  g.theLayerPairs.push_back(lee8);
  g.theLayerPairs.push_back(lee9);
  g.theRootLayers.push_back(0);

  return g;
  }

