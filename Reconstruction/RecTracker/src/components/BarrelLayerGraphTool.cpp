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

tricktrack::CMGraph BarrelLayerGraphTool::getGraph() {
  
  auto g = tricktrack::CMGraph();

  auto l1 = tricktrack::CMLayer("innerLayer", 10);
  auto l2 = tricktrack::CMLayer("middleLayer", 10);
  auto l3 = tricktrack::CMLayer("outerLayer", 10);
  auto l4 = tricktrack::CMLayer("outermostLayer", 10);

  auto lp1 = tricktrack::CMLayerPair(0, 1);
  auto lp2 = tricktrack::CMLayerPair(1, 2);
  auto lp3 = tricktrack::CMLayerPair(2, 3);
  l1.theOuterLayers.push_back(1);
  l2.theInnerLayers.push_back(0);
  l2.theOuterLayers.push_back(2);
  l3.theInnerLayers.push_back(1);
  l3.theOuterLayers.push_back(3);
  l4.theInnerLayers.push_back(2);
  l1.theOuterLayerPairs.push_back(0);
  l2.theInnerLayerPairs.push_back(0);
  l2.theOuterLayerPairs.push_back(1);
  l3.theInnerLayerPairs.push_back(1);
  l3.theOuterLayerPairs.push_back(2);
  l4.theInnerLayerPairs.push_back(2);

  auto theLayerGraph = tricktrack::CMGraph();
  g.theLayers.push_back(l1);
  g.theLayers.push_back(l2);
  g.theLayers.push_back(l3);
  g.theLayers.push_back(l4);
  g.theLayerPairs.push_back(lp1);
  g.theLayerPairs.push_back(lp2);
  g.theLayerPairs.push_back(lp3);
  g.theRootLayers.push_back(0);

  return g;
  }

