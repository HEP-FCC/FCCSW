#include "FullInnerLayerGraphTool.h"

#include "RecInterface/ILayerGraphTool.h"

// TrickTrack headers
#include "tricktrack/CMGraph.h"

DECLARE_TOOL_FACTORY(FullInnerLayerGraphTool)

FullInnerLayerGraphTool::FullInnerLayerGraphTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ILayerGraphTool>(this);
}

StatusCode FullInnerLayerGraphTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}

StatusCode FullInnerLayerGraphTool::finalize() { return GaudiTool::finalize(); }

/// Construct and return the layergraph for the Cellular automaton,
// for the inner four barrel layers only
// the interface to the layer graph is likely to be changed
tricktrack::CMGraph FullInnerLayerGraphTool::getGraph() {
  
  auto g = tricktrack::CMGraph();

  constexpr unsigned int numberOfHits = 10000; // used to resize vector "isOuterHitOfCell""
  auto lb0 = tricktrack::CMLayer("innerLayer", numberOfHits);
  auto lb1 = tricktrack::CMLayer("middleLayer", numberOfHits);
  auto lb2 = tricktrack::CMLayer("outerLayer", numberOfHits);
  auto lb3 = tricktrack::CMLayer("outermostLayer", numberOfHits);
  auto le4 = tricktrack::CMLayer("firstEndcap", numberOfHits);
  auto le5 = tricktrack::CMLayer("secondEndcap", numberOfHits);
  auto le6 = tricktrack::CMLayer("thirdEndcap", numberOfHits);
  auto le7 = tricktrack::CMLayer("fourthEndcap", numberOfHits);
  auto ln8 = tricktrack::CMLayer("firstNegEndcap", numberOfHits);
  auto ln9 = tricktrack::CMLayer("secondNegEndcap", numberOfHits);
  auto ln10 = tricktrack::CMLayer("thirdNegEndcap", numberOfHits);
  auto ln11 = tricktrack::CMLayer("fourthNegEndcap", numberOfHits);
  auto ln12 = tricktrack::CMLayer("fifthNegEndcap", numberOfHits);
  auto le13 = tricktrack::CMLayer("fifthPosEndcap", numberOfHits);

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
  auto lee18 = tricktrack::CMLayerPair(7, 13);
  // barrel - negendcap
  auto lbn10 = tricktrack::CMLayerPair(0, 8);
  auto lbn11 = tricktrack::CMLayerPair(1, 8);
  auto lbn12 = tricktrack::CMLayerPair(2, 8);
  auto lbn13 = tricktrack::CMLayerPair(3, 8);
  // negendcap - negendcap
  auto lnn14 = tricktrack::CMLayerPair(8, 9);
  auto lnn15 = tricktrack::CMLayerPair(9, 10);
  auto lnn16 = tricktrack::CMLayerPair(10, 11);
  auto lnn17 = tricktrack::CMLayerPair(11, 12);

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
  le7.theOuterLayers.push_back(13);
  le13.theInnerLayers.push_back(7);
  // barrel - negendcap
  ln8.theInnerLayers.push_back(0);
  lb0.theOuterLayers.push_back(8);
  ln8.theInnerLayers.push_back(1);
  lb1.theOuterLayers.push_back(8);
  ln8.theInnerLayers.push_back(2);
  lb2.theOuterLayers.push_back(8);
  ln8.theInnerLayers.push_back(3);
  lb3.theOuterLayers.push_back(8);
  // negendcap - negendcap
  ln8.theOuterLayers.push_back(9);
  ln9.theInnerLayers.push_back(8);
  ln9.theOuterLayers.push_back(10);
  ln10.theInnerLayers.push_back(9);
  ln10.theOuterLayers.push_back(11);
  ln11.theInnerLayers.push_back(10);
  ln11.theOuterLayers.push_back(12);
  ln12.theInnerLayers.push_back(11);

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
  le4.theOuterLayerPairs.push_back(7);
  le5.theInnerLayerPairs.push_back(7);
  le5.theOuterLayerPairs.push_back(8);
  le6.theInnerLayerPairs.push_back(8);
  le6.theOuterLayerPairs.push_back(9);
  le7.theInnerLayerPairs.push_back(9);
  le7.theOuterLayerPairs.push_back(18);
  le13.theInnerLayerPairs.push_back(18);

  // barrel - endcap
  lb0.theOuterLayerPairs.push_back(10);
  ln8.theInnerLayerPairs.push_back(10);
  lb1.theOuterLayerPairs.push_back(11);
  ln8.theInnerLayerPairs.push_back(11);
  lb2.theOuterLayerPairs.push_back(12);
  ln8.theInnerLayerPairs.push_back(12);
  lb3.theOuterLayerPairs.push_back(13);
  ln8.theInnerLayerPairs.push_back(13);

  //endcap - endcap
  ln8.theOuterLayerPairs.push_back(14);
  ln9.theInnerLayerPairs.push_back(14);
  ln9.theOuterLayerPairs.push_back(15);
  ln10.theInnerLayerPairs.push_back(15);
  ln10.theOuterLayerPairs.push_back(16);
  ln11.theInnerLayerPairs.push_back(16);
  ln11.theOuterLayerPairs.push_back(17);
  ln12.theInnerLayerPairs.push_back(17);

  auto theLayerGraph = tricktrack::CMGraph();
  g.theLayers.push_back(lb0);
  g.theLayers.push_back(lb1);
  g.theLayers.push_back(lb2);
  g.theLayers.push_back(lb3);
  g.theLayers.push_back(le4);
  g.theLayers.push_back(le5);
  g.theLayers.push_back(le6);
  g.theLayers.push_back(le7);
  g.theLayers.push_back(ln8);
  g.theLayers.push_back(ln9);
  g.theLayers.push_back(ln10);
  g.theLayers.push_back(ln11);
  g.theLayers.push_back(ln12);
  g.theLayers.push_back(le13);
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
  g.theLayerPairs.push_back(lbn10);
  g.theLayerPairs.push_back(lbn11);
  g.theLayerPairs.push_back(lbn12);
  g.theLayerPairs.push_back(lbn13);
  g.theLayerPairs.push_back(lnn14);
  g.theLayerPairs.push_back(lnn15);
  g.theLayerPairs.push_back(lnn16);
  g.theLayerPairs.push_back(lnn17);
  g.theLayerPairs.push_back(lee18);
  g.theRootLayers.push_back(0);
  g.theRootLayers.push_back(4);
  g.theRootLayers.push_back(5);
  g.theRootLayers.push_back(8);
  g.theRootLayers.push_back(9);

  return g;
  }

