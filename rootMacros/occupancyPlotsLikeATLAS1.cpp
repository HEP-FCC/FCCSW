#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TTree.h"
#include "averageOccupanciesHelper.hpp"
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <numeric>

#include "TColor.h"
#include "TROOT.h"
#include "TStyle.h"

struct BarrelLayerInfo {
  std::map<float, std::vector<float>> occupancyPerRod;
  std::map<float, std::vector<float>> clusterOccupancyPerRod;
  std::map<float, std::vector<float>> clusterSizePerRod;
};

struct RegionRodInfo {
  std::map<long long int, std::vector<float>> occupancyPerRod;
  std::map<long long int, std::vector<float>> clusterOccupancyPerRod;
  std::map<long long int, std::vector<float>> clusterSizePerRod;
};

struct LayerAndRingInfo {
  int nSurfaces = 0;
  double occupancy = 0.;
  std::vector<float> occupancies;
  double dataRate = 0.;
  std::vector<float> dataRates;
  double dataRatePerSurface = 0.;

  void add(const LayerAndRingInfo& layerInfo) {
    occupancies.push_back(layerInfo.occupancy);
    dataRates.push_back(layerInfo.dataRate);
    occupancy += layerInfo.occupancy;
    dataRate += layerInfo.dataRate;
    dataRatePerSurface += layerInfo.dataRatePerSurface;
  };

  void standardDev(float& stdOccupancies, float& stdDataRates) {
    double sumOccupancy = 0;
    double sumDataRates = 0;

    for (int i = 0; i < occupancies.size(); i++) {
      // occupancy
      float diffOccupancy = (occupancies.at(i) - occupancy);
      sumOccupancy += (diffOccupancy * diffOccupancy);

      float diffDataRates = (dataRates.at(i) - dataRate);
      sumDataRates += (diffDataRates * diffDataRates);
    }
    if (occupancies.size() > 1) {
      sumOccupancy /= (occupancies.size() - 1);
      sumDataRates /= (occupancies.size() - 1);
    }
    stdOccupancies = sqrt(sumOccupancy);
    stdDataRates = sqrt(sumDataRates);
  };
};

void occupancyPlotsLikeATLAS1(int nEvents = 1,
                              std::string dirname =
                                  "/eos/experiment/fcc/hh/simulation/samples/v03/physics/MinBias/bFieldOn/"
                                  "etaFull/reco/tracker/PU1000/clusters/final/",
                              std::string outFile = "occupancyStudy_1Ev_test.root") {

  // --------------------------------------------------------
  // -------------Declare input file and branches------------
  // --------------------------------------------------------
  const char* ext = ".root";
  std::string treeName = "clusters";

  TSystemDirectory dir(dirname.data(), dirname.data());
  TList* files = dir.GetListOfFiles();

  // --------------------------------------------------------
  // ----------------Constants and definitions---------------
  // --------------------------------------------------------
  float pixelRegionR = 200;
  float macroPixelRegionR = 900;

  // needed to calculate the cluster occupancy (#cluster/(mm*mm))
  float moduleSurface_barrel[12] = {876.8,    1049.6,   1049.6,   1049.6,   5242.88,  5242.88,
                                    10485.76, 10485.76, 10485.76, 10485.76, 10485.76, 10485.76};

  std::vector<float> moduleSurface_innerEC = {2571.233012, 4840.264064, 5079.074424, 5060.047649};
  std::vector<float> boundsR_innerEC = {100., 200., 300., 400.};

  // all outer modules have the same surface
  float outerModuleSurface_EC = 10485.76;

  std::vector<float> moduleSurface_outerEC = {
      2631.309094,           4861.304322,           5129.335734,           5092.462398,
      5776.182408,           5457.687618,           outerModuleSurface_EC, outerModuleSurface_EC,
      outerModuleSurface_EC, outerModuleSurface_EC, outerModuleSurface_EC, outerModuleSurface_EC,
      outerModuleSurface_EC, outerModuleSurface_EC, outerModuleSurface_EC, outerModuleSurface_EC};
  std::vector<float> boundsR_outerEC = {100., 200.,  300.,  400.,  500.,  600.,  700.,  800.,
                                        900., 1000., 1100., 1200., 1300., 1400., 1500., 1600.};
  std::vector<float> moduleSurface_fwd0 = {
      3588.111018,           5378.281821,           5319.210975,           5306.641528,           5283.456345,
      5603.544392,           outerModuleSurface_EC, outerModuleSurface_EC, outerModuleSurface_EC, outerModuleSurface_EC,
      outerModuleSurface_EC, outerModuleSurface_EC, outerModuleSurface_EC};
  std::vector<float> boundsR_fwd0 = {110., 210., 310., 410., 510., 610., 710., 810., 920., 1020., 1120, 1220., 1320.};

  std::vector<float> moduleSurface_fwd1 = {4651.261744,
                                           5049.970554,
                                           5103.684333,
                                           5142.264175,
                                           5521.04557,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC,
                                           outerModuleSurface_EC};
  // std::vector<float> boundsR_fwd1 = {140., 240., 340., 440., 540.};
  std::vector<float> boundsR_fwd1 = {140., 240.,  340.,  440.,  540.,  640.,  740., 840.,
                                     940., 1040., 1140., 1240., 1340., 1440., 1540.};

  float PosZ_ec[nLayers_ec] = {950.,   1178.5, 1462.,  1813.7, 2250.,  2625.,   2986.1,  3387.5,  3854.7,  4386.1,
                               4990.7, 6250.,  7395.1, 8750.,  10000., 10985.6, 12068.4, 13248.5, 14555.2, 15990.7};

  float PosZ_ec_strip[nLayers_ec] = {2625., 2986.1, 3387.5,  3854.7,  4386.1,  4990.7,  7395.1,
                                     8750., 10000., 10985.6, 12068.4, 13248.5, 14555.2, 15990.7};

  // number of events = number of files
  int nFiles = 0;

  // data rate factor
  double frequency = 40e6;
  double factorToTB = 1 / (1024. * 1024. * 1024. * 1024.);  // binary
  double bitToByte = 0.125;
  double dataRateConversionFactor = (frequency * factorToTB * bitToByte);  // TB/s

  // Event information
  EventInformation eventInfo;

  // --------------------------------------------------------
  // ---------------------Plot defintions--------------------
  // --------------------------------------------------------

  // histograms
  TProfile2D* channelOccupancy_h =
      new TProfile2D("channelOccupancy_h", "#channels on/#channels per module", 700, -16150., 16150., 25, 0., 1600.);
  channelOccupancy_h->GetXaxis()->SetTitle("z[mm]");
  channelOccupancy_h->GetYaxis()->SetTitle("r[mm]");
  channelOccupancy_h->GetZaxis()->SetTitle("occupancy[%]");

  TH2F* channelOccupancy_h_barrel =
      new TH2F("channelOccupancy_h_barrel", "#channel on/#channels per module in %", 69, 0., 1600., 100, 0., 100.);
  channelOccupancy_h->GetXaxis()->SetTitle("z[mm]");
  channelOccupancy_h->GetYaxis()->SetTitle("r[mm]");

  std::vector<BarrelLayerInfo> layers_barrel(nLayers_barrel);

  std::vector<RegionRodInfo> eCapRegionInfo(3);  // pixel, macropixel, strip region

  // inner Endcap info per ring
  std::vector<LayerAndRingInfo> innerEndcapEventInfo(4);
  std::vector<LayerAndRingInfo> outerEndcapEventInfo(16);
  std::vector<LayerAndRingInfo> fwd0EndcapEventInfo(13);
  std::vector<LayerAndRingInfo> fwd1EndcapEventInfo(15);

  TTree* eventTree = new TTree("eventTree", "eventTree");
  eventTree->SetDirectory(0);
  double nPixelsOnPerEvent = 0.;
  double nClustersPerEvent = 0.;
  eventTree->Branch("nPixelsOnPerEvent", &nPixelsOnPerEvent);
  eventTree->Branch("nClustersPerEvent", &nClustersPerEvent);

  if (files) {
    TSystemFile* file;
    TString fname;
    TIter next(files);
    while ((file = (TSystemFile*)next())) {
      fname = dirname + file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(ext)) {
        // check number of events
        if (nFiles == nEvents) {
          std::cout << "Reached " << nEvents << " number of events." << std::endl;
          break;
        }
        nFiles++;

        // current file
        std::cout << "opening file: " << fname.Data() << std::endl;
        TFile inputFile(fname.Data());
        TTreeReader reader(treeName.c_str(), &inputFile);
        //   std::cout << "#surfaces: " << reader.GetEntries(true) << std::endl;
        // number of events per file
        int maxEvent = -1;

        TTreeReaderValue<int> _eventNr(reader, "event_nr");
        TTreeReaderValue<long long int> _moduleID(reader, "moduleID");
        TTreeReaderValue<int> _nChannels(reader, "nChannels");
        TTreeReaderValue<int> _nChannelsOn(reader, "nChannelsOn");
        TTreeReaderValue<int> _nChannels_l0(reader, "nChannels_l0");
        TTreeReaderValue<int> _nChannels_l1(reader, "nChannels_l1");
        TTreeReaderValue<float> _sX(reader, "s_x");
        TTreeReaderValue<float> _sY(reader, "s_y");
        TTreeReaderValue<float> _sZ(reader, "s_z");
        TTreeReaderValue<std::vector<short int>> _nCells(reader, "nCells");
        TTreeReaderValue<std::vector<short int>> _sizeX(reader, "size_x");
        TTreeReaderValue<std::vector<short int>> _sizeY(reader, "size_y");
        TTreeReaderValue<std::vector<float>> _x(reader, "g_x");
        TTreeReaderValue<std::vector<float>> _y(reader, "g_y");
        TTreeReaderValue<std::vector<float>> _z(reader, "g_z");

        /// Layer information for this event
        std::vector<LayerInformation> layerInfo_barrel(nLayers_barrel);
        std::vector<LayerInformation> layerInfo_ePos(nLayers_ec);
        std::vector<LayerInformation> layerInfo_eNeg(nLayers_ec);

        /// number of pixels, activated pixels and clusters per event
        double nPixelsOnEvent = 0.;
        double nPixels = 0.;
        double nClustersEvent = 0.;

        std::vector<LayerAndRingInfo> innerEndcapInfo(4);
        std::vector<LayerAndRingInfo> outerEndcapInfo(16);
        std::vector<LayerAndRingInfo> fwd0EndcapInfo(13);
        std::vector<LayerAndRingInfo> fwd1EndcapInfo(15);

        // --------------------------------------------------------
        // --------------------------READ--------------------------
        // --------------------------------------------------------

        // read in event
        while (reader.Next()) {

          nPixelsOnEvent += float(*_nChannelsOn);
          nPixels += float(*_nChannels);
          nClustersEvent += (*_nCells).size();
          // set the correct event number
          if ((*_eventNr) > maxEvent) maxEvent = *_eventNr;
          // calculate occupancy
          float channelRatio = float(*_nChannelsOn) / float(*_nChannels);
          // dataRate
          float dataRate = float(*_nChannelsOn) * (std::log2(float(*_nChannels_l0) * float(*_nChannels_l1)));
          // surface info
          TVector3 surfacePosition((*_sX), (*_sY), (*_sZ));
          float surfR = surfacePosition.Perp();
          // go thorugh information per surface
          // distinguish barrel/ec and their layers
          long long int systemID = ((*_moduleID) & mask);
          LayerInformation* layerInfo = nullptr;
          if ((systemID == 0) || (systemID == 1)) {
            //-------------------barrel-------------------
            // calculate global barrel layer ID
            long long int barrelLayerID = (((*_moduleID) & layerMaskBarrel) >> 4) + 6 * systemID;
            // average cluster size of this module
            double averageClusterSize = 0;
            for (int i = 0; i < (*_nCells).size(); i++) {
              averageClusterSize += (*_nCells).at(i);
              // total number of pixels on
            }
            // divide through number of clusters
            if ((*_nCells).size() > 1) averageClusterSize /= (*_nCells).size();
            // cluster occupancy
            float pixelClustersPerSurface = (float((*_nCells).size()) / moduleSurface_barrel[barrelLayerID]);
            // plots like Atlas
            layers_barrel.at(barrelLayerID)
                .occupancyPerRod[std::fabs(surfacePosition.Z())]
                .push_back(channelRatio * 100);
            layers_barrel.at(barrelLayerID)
                .clusterOccupancyPerRod[std::fabs(surfacePosition.Z())]
                .push_back(pixelClustersPerSurface);
            layers_barrel.at(barrelLayerID)
                .clusterSizePerRod[std::fabs(surfacePosition.Z())]
                .push_back(averageClusterSize);

            // create layer information
            layerInfo = &layerInfo_barrel.at(barrelLayerID);
            layerInfo->dataRatePerSurface += (dataRate / moduleSurface_barrel[barrelLayerID]);
            // barrel information
            channelOccupancy_h_barrel->Fill(surfR, channelRatio * 100);

          } else {
            // needed for cluster occupancy
            float surface = 0;
            //-------------------endcap-------------------
            // layer offset - needed for disc ID
            int layerOffset = 0;
            // pos endcap
            if (systemID == 3) {
              // outer
              layerOffset = 5;
              for (int i = 0; i < boundsR_outerEC.size(); i++) {
                if (surfR <= boundsR_outerEC.at(i)) {
                  surface = moduleSurface_outerEC.at(i);
                  outerEndcapInfo.at(i).occupancy += channelRatio;
                  outerEndcapInfo.at(i).nSurfaces += 1;
                  outerEndcapInfo.at(i).dataRate += dataRate;
                  outerEndcapInfo.at(i).dataRatePerSurface += (dataRate / surface);
                  break;
                }
              }

            } else if (systemID == 4) {
              // fwd
              layerOffset = 11;
              // check first in which fwd region we are
              if (std::fabs(surfacePosition.Z()) < 9000) {
                // first fwd region
                for (int i = 0; i < boundsR_fwd0.size(); i++) {
                  if (surfR <= boundsR_fwd0.at(i)) {
                    surface = moduleSurface_fwd0.at(i);
                    fwd0EndcapInfo.at(i).occupancy += channelRatio;
                    fwd0EndcapInfo.at(i).nSurfaces += 1;
                    fwd0EndcapInfo.at(i).dataRate += dataRate;
                    fwd0EndcapInfo.at(i).dataRatePerSurface += (dataRate / surface);
                    break;
                  }
                }

              } else {
                for (int i = 0; i < boundsR_fwd1.size(); i++) {
                  if (surfR <= boundsR_fwd1.at(i)) {
                    surface = moduleSurface_fwd1.at(i);
                    fwd1EndcapInfo.at(i).occupancy += channelRatio;
                    fwd1EndcapInfo.at(i).nSurfaces += 1;
                    fwd1EndcapInfo.at(i).dataRate += dataRate;
                    fwd1EndcapInfo.at(i).dataRatePerSurface += (dataRate / surface);

                    break;
                  }
                }
              }

            } else {
              // inner
              for (int i = 0; i < boundsR_innerEC.size(); i++) {
                if (surfR <= boundsR_innerEC.at(i)) {
                  surface = moduleSurface_innerEC.at(i);
                  innerEndcapInfo.at(i).occupancy += channelRatio;
                  innerEndcapInfo.at(i).nSurfaces += 1;
                  innerEndcapInfo.at(i).dataRate += dataRate;
                  innerEndcapInfo.at(i).dataRatePerSurface += (dataRate / surface);
                  break;
                }
              }
            }
            if (surface == 0)
              std::cout << "surface is 0: z" << surfacePosition.Z() << " surfR:  " << surfR << std::endl;
            // distinguish positive and negative side
            // calculate global ec pos layer ID
            long long unsigned discLayerID = (((*_moduleID) & layerMaskEC) >> 5) + layerOffset;
            if (!(((*_moduleID) & posNegMask) >> 4)) {
              //-------------------pos endcap-------------------
              // create layer information
              layerInfo = &layerInfo_ePos.at(discLayerID);

            } else {
              //-------------------neg endcap-------------------
              layerInfo = &layerInfo_eNeg.at(discLayerID);
            }

            float pixelClustersPerSurface = float((*_nCells).size()) / surface;

            // set occupancy for different regions in endcaps
            if (surfR < pixelRegionR) {
              layerInfo->occupancy_pixel += channelRatio;
              if (channelRatio > layerInfo->occupancy_pixel_Max) layerInfo->occupancy_pixel_Max = channelRatio;
              if (channelRatio < layerInfo->occupancy_pixel_Min) layerInfo->occupancy_pixel_Min = channelRatio;

              float averageClusterSize = 0.;

              for (int i = 0; i < (*_x).size(); i++) {
                TVector3 position((*_x).at(i), (*_y).at(i), (*_z).at(i));
                averageClusterSize += (*_nCells).at(i);
              }
              if ((*_x).size()) {
                averageClusterSize /= (*_x).size();
              }
              eCapRegionInfo.at(0).occupancyPerRod[discLayerID].push_back(channelRatio * 100);
              eCapRegionInfo.at(0).clusterOccupancyPerRod[discLayerID].push_back(pixelClustersPerSurface);
              eCapRegionInfo.at(0).clusterSizePerRod[discLayerID].push_back(averageClusterSize);

              layerInfo->nSurfaces_pixel += 1;
            } else if (surfR < macroPixelRegionR) {
              layerInfo->occupancy_macroPixel += channelRatio;
              if (channelRatio > layerInfo->occupancy_macroPixel_Max)
                layerInfo->occupancy_macroPixel_Max = channelRatio;
              if (channelRatio < layerInfo->occupancy_macroPixel_Min)
                layerInfo->occupancy_macroPixel_Min = channelRatio;
              layerInfo->nSurfaces_macroPixel += 1;

              float averageClusterSize = 0.;

              for (int i = 0; i < (*_x).size(); i++) {
                TVector3 position((*_x).at(i), (*_y).at(i), (*_z).at(i));
                averageClusterSize += (*_nCells).at(i);
              }
              if ((*_x).size()) {
                averageClusterSize /= (*_x).size();
              }
              eCapRegionInfo.at(1).occupancyPerRod[discLayerID].push_back(channelRatio * 100);
              eCapRegionInfo.at(1).clusterOccupancyPerRod[discLayerID].push_back(pixelClustersPerSurface);
              eCapRegionInfo.at(1).clusterSizePerRod[discLayerID].push_back(averageClusterSize);

            } else {
              layerInfo->occupancy_strip += channelRatio;
              if (channelRatio > layerInfo->occupancy_strip_Max) layerInfo->occupancy_strip_Max = channelRatio;
              if (channelRatio < layerInfo->occupancy_strip_Min) layerInfo->occupancy_strip_Min = channelRatio;
              layerInfo->nSurfaces_strip += 1;

              float averageClusterSize = 0.;

              for (int i = 0; i < (*_x).size(); i++) {
                TVector3 position((*_x).at(i), (*_y).at(i), (*_z).at(i));
                averageClusterSize += (*_nCells).at(i);
              }
              if ((*_x).size()) {
                averageClusterSize /= (*_x).size();
              }
              eCapRegionInfo.at(2).occupancyPerRod[discLayerID].push_back(channelRatio * 100);
              eCapRegionInfo.at(2).clusterOccupancyPerRod[discLayerID].push_back(pixelClustersPerSurface);
              eCapRegionInfo.at(2).clusterSizePerRod[discLayerID].push_back(averageClusterSize);
            }
          }
          // fill global occupancy histogram
          channelOccupancy_h->Fill((*_sZ), surfR, channelRatio * 100);
          // count surfaces
          layerInfo->nSurfaces += 1;
          // fill layerinfo for later
          layerInfo->occupancy += channelRatio;
          if (channelRatio > layerInfo->occupancyMax) layerInfo->occupancyMax = channelRatio;
          if (channelRatio < layerInfo->occupancyMin) layerInfo->occupancyMin = channelRatio;
          layerInfo->dataRate += dataRate;

        }  // after reading in
        maxEvent += 1;
        // std::cout << "Maxevent: " << maxEvent << std::endl;
        if (maxEvent <= 0) return;
        if (maxEvent > 1) {
          std::cout << "Error, currently more than one event within the same file can not be averaged with this "
                       "script, please change script!"
                    << std::endl;
          return;
        }

        // --------------------------------------------------------
        // ---------------------Average layers---------------------
        // --------------------------------------------------------

        // barrel
        int barrelLayerCounter = 0;
        for (auto& layer : layerInfo_barrel) {
          // average over surfaces
          layer.occupancy /= layer.nSurfaces;

          // set event info to module averaged layer info of this event
          eventInfo.layerInfo_barrel.at(barrelLayerCounter).add(layer);

          barrelLayerCounter++;
        }
        // endcap
        for (size_t i = 0; i < nLayers_ec; i++) {
          // pos EC
          // average over surfaces
          // discs
          layerInfo_ePos.at(i).occupancy /= layerInfo_ePos.at(i).nSurfaces;
          layerInfo_eNeg.at(i).occupancy /= layerInfo_eNeg.at(i).nSurfaces;

          layerInfo_ePos.at(i).occupancy_pixel /= layerInfo_ePos.at(i).nSurfaces_pixel;
          layerInfo_eNeg.at(i).occupancy_pixel /= layerInfo_eNeg.at(i).nSurfaces_pixel;

          layerInfo_ePos.at(i).occupancy_macroPixel /= layerInfo_ePos.at(i).nSurfaces_macroPixel;
          layerInfo_eNeg.at(i).occupancy_macroPixel /= layerInfo_eNeg.at(i).nSurfaces_macroPixel;

          layerInfo_ePos.at(i).occupancy_strip /= layerInfo_ePos.at(i).nSurfaces_strip;
          layerInfo_eNeg.at(i).occupancy_strip /= layerInfo_eNeg.at(i).nSurfaces_strip;

          // add to event info for event averaging
          eventInfo.layerInfo_ePos.at(i).add(layerInfo_ePos.at(i));
          eventInfo.layerInfo_eNeg.at(i).add(layerInfo_eNeg.at(i));

          // neg EC
        }

        // average rings
        size_t ringCounter = 0;
        for (auto& ring : innerEndcapInfo) {
          ring.occupancy /= ring.nSurfaces;
          // set event info
          innerEndcapEventInfo.at(ringCounter).add(ring);
          ringCounter++;
        }
        ringCounter = 0;
        for (auto& ring : outerEndcapInfo) {
          ring.occupancy /= ring.nSurfaces;
          // set event info
          outerEndcapEventInfo.at(ringCounter).add(ring);
          ringCounter++;
        }
        ringCounter = 0;
        for (auto& ring : fwd0EndcapInfo) {
          ring.occupancy /= ring.nSurfaces;
          // set event info
          fwd0EndcapEventInfo.at(ringCounter).add(ring);
          ringCounter++;
        }
        ringCounter = 0;
        for (auto& ring : fwd1EndcapInfo) {
          ring.occupancy /= ring.nSurfaces;
          // set event info
          fwd1EndcapEventInfo.at(ringCounter).add(ring);
          ringCounter++;
        }

        inputFile.Close();

        /*   std::cout << "#pixels: " << nPixels << " #PixelsOn: " << nPixelsOnEvent << " #Clusters: " << nClustersEvent
                     << std::endl;*/

        nPixelsOnPerEvent = nPixelsOnEvent;
        nClustersPerEvent = nClustersEvent;
        eventTree->Fill();
      }  // check if file exists
    }    // go through files
         //  std::cout << "after going through files" << std::endl;
    // --------------------------------------------------------
    // ---------------------Average events---------------------
    // --------------------------------------------------------
    std::cout << "Creating new output file: " << outFile << " and writing out histograms. " << std::endl;
    TFile outputFile(outFile.c_str(), "recreate");
    channelOccupancy_h->Write();
    channelOccupancy_h_barrel->Write();

    // --------------------------------------------------------
    // ---------------------Plot defintions--------------------
    // --------------------------------------------------------

    TTree* tree_barrel = new TTree("tree_barrel", "tree_barrel");
    TTree* tree_ePos = new TTree("tree_ePos", "tree_ePos");
    TTree* tree_eNeg = new TTree("tree_eNeg", "tree_eNeg");

    tree_barrel->SetDirectory(0);
    tree_ePos->SetDirectory(0);
    tree_eNeg->SetDirectory(0);

    float currentOccupancy = 0;
    float currentOccStdDev = 0;
    float currentOccupancyMax = 0;
    float currentOccupancyMin = 0;

    float currentDataRate = 0;
    float currentDataRateDev = 0;
    float currentDataRatePerSurface = 0;

    // for endcaps
    float currentOccupancy_pixel = 0;
    float currentOccupancyMax_pixel = 0;
    float currentOccupancyMin_pixel = 0;

    float currentOccupancy_macroPixel = 0;
    float currentOccupancyMax_macroPixel = 0;
    float currentOccupancyMin_macroPixel = 0;

    float currentOccupancy_strip = 0;
    float currentOccupancyMax_strip = 0;
    float currentOccupancyMin_strip = 0;

    tree_barrel->Branch("occupancy", &currentOccupancy);
    tree_barrel->Branch("occupancyDev", &currentOccStdDev);
    tree_barrel->Branch("occMax", &currentOccupancyMax);
    tree_barrel->Branch("occMin", &currentOccupancyMin);
    tree_barrel->Branch("dataRate", &currentDataRate);
    tree_barrel->Branch("dataRateDev", &currentDataRateDev);
    tree_barrel->Branch("dataRatePerSurface", &currentDataRatePerSurface);

    tree_ePos->Branch("occupancy", &currentOccupancy);
    tree_ePos->Branch("occupancyDev", &currentOccStdDev);
    tree_ePos->Branch("occMax", &currentOccupancyMax);
    tree_ePos->Branch("occMin", &currentOccupancyMin);
    tree_ePos->Branch("occupancy_pixel", &currentOccupancy_pixel);
    tree_ePos->Branch("occMax_pixel", &currentOccupancyMax_pixel);
    tree_ePos->Branch("occMin_pixel", &currentOccupancyMin_pixel);
    tree_ePos->Branch("occupancy_macroPixel", &currentOccupancy_macroPixel);
    tree_ePos->Branch("occMax_macroPixel", &currentOccupancyMax_macroPixel);
    tree_ePos->Branch("occMin_macroPixel", &currentOccupancyMin_macroPixel);
    tree_ePos->Branch("occupancy_strip", &currentOccupancy_strip);
    tree_ePos->Branch("occMax_strip", &currentOccupancyMax_strip);
    tree_ePos->Branch("occMin_strip", &currentOccupancyMin_strip);

    tree_eNeg->Branch("occupancy", &currentOccupancy);
    tree_eNeg->Branch("occupancyDev", &currentOccStdDev);
    tree_eNeg->Branch("occMax", &currentOccupancyMax);
    tree_eNeg->Branch("occMin", &currentOccupancyMin);
    tree_eNeg->Branch("occupancy_pixel", &currentOccupancy_pixel);
    tree_eNeg->Branch("occMax_pixel", &currentOccupancyMax_pixel);
    tree_eNeg->Branch("occMin_pixel", &currentOccupancyMin_pixel);
    tree_eNeg->Branch("occupancy_macroPixel", &currentOccupancy_macroPixel);
    tree_eNeg->Branch("occMax_macroPixel", &currentOccupancyMax_macroPixel);
    tree_eNeg->Branch("occMin_macroPixel", &currentOccupancyMin_macroPixel);
    tree_eNeg->Branch("occupancy_strip", &currentOccupancy_strip);
    tree_eNeg->Branch("occMax_strip", &currentOccupancyMax_strip);
    tree_eNeg->Branch("occMin_strip", &currentOccupancyMin_strip);

    // plot graph for barrel

    float radii_barrel[nLayers_barrel] = {25,      60,      100,     150,      270,      400,
                                          521.016, 742.396, 937.244, 1132.012, 1326.735, 1516.536};

    Double_t binEdges_barrel0[11] = {0., 68.5, 137., 205.5, 274, 342.5, 411., 479.5, 548., 616.5, 700.};
    Double_t binEdges_barrel1[21] = {0,    41.,  82.,  123., 164., 205., 246., 287., 328., 369., 410.,
                                     451., 492., 533., 574., 615., 656., 697., 738., 779., 820.};
    Double_t binEdges_barrel2[10] = {0.,       50.401,  148.5445, 247.7685, 344.537,
                                     442.3705, 537.783, 634.246,  728.3215, 800.};
    Double_t binEdges_barrel3[25] = {0,        49.4595,  148.856,  247.31,   346.0885,  443.9305, 542.0945,
                                     639.328,  736.8815, 833.5105, 930.4575, 1026.4845, 1122.83,  1218.26,
                                     1314.005, 1408.845, 1503.99,  1598.235, 1692.795,  1786.455, 1880.425,
                                     1973.505, 2066.89,  2159.39,  2220};

    float occupancy_barrel[nLayers_barrel];
    float occupancyErrorL_barrel[nLayers_barrel];
    float occupancyErrorH_barrel[nLayers_barrel];

    /// average over event Barrel
    int counter = 0;
    for (int i = 0; i < nLayers_barrel; i++) {
      std::string barrelLayerName_cl = "clusterSizes_h2_l" + std::to_string(i);
      std::string barrelLayerName_clo = "clusterOccupancy_h2_l" + std::to_string(i);
      std::string barrelLayerName_occ = "occupancy_h2_l" + std::to_string(i);
      TH2F* occupancy_h2 = nullptr;
      TH2F* clusterOccupancy_h2 = nullptr;
      TH2F* clusterSizes_h2 = nullptr;
      float max_occ = 20.;
      if (i == 0) {
        clusterSizes_h2 = new TH2F(barrelLayerName_cl.c_str(), "clusterSizes", 10, binEdges_barrel0, 100, 0., 100.);
        clusterOccupancy_h2 =
            new TH2F(barrelLayerName_clo.c_str(), "clusterOccupancy", 10, binEdges_barrel0, 100, 0., 10.);
        occupancy_h2 =
            new TH2F(barrelLayerName_occ.c_str(), "channelOccupancy", 10, binEdges_barrel0, 100, 0., max_occ);
      } else if (i >= 6) {
        float max_clo = 0.1;
        if (i < 8) max_occ = 1.;
        if (i == 9) max_occ = 60.;
        if (i >= 8) max_clo = 0.01;
        clusterSizes_h2 = new TH2F(barrelLayerName_cl.c_str(), "clusterSizes", 24, binEdges_barrel3, 100, 0., 100.);
        clusterOccupancy_h2 =
            new TH2F(barrelLayerName_clo.c_str(), "clusterOccupancy", 24, binEdges_barrel3, 100, 0., max_clo);
        occupancy_h2 =
            new TH2F(barrelLayerName_occ.c_str(), "channelOccupancy", 24, binEdges_barrel3, 100, 0., max_occ);
      } else if ((i == 4) || (i == 5)) {
        clusterSizes_h2 = new TH2F(barrelLayerName_cl.c_str(), "clusterSizes", 9, binEdges_barrel2, 100, 0., 100.);
        clusterOccupancy_h2 =
            new TH2F(barrelLayerName_clo.c_str(), "clusterOccupancy", 9, binEdges_barrel2, 100, 0., 1.);
        occupancy_h2 = new TH2F(barrelLayerName_occ.c_str(), "channelOccupancy", 9, binEdges_barrel2, 100, 0., 1.);
      } else {
        clusterSizes_h2 = new TH2F(barrelLayerName_cl.c_str(), "clusterSizes", 20, binEdges_barrel1, 100, 0., 100.);
        clusterOccupancy_h2 =
            new TH2F(barrelLayerName_clo.c_str(), "clusterOccupancy", 20, binEdges_barrel1, 100, 0., 1.);
        occupancy_h2 = new TH2F(barrelLayerName_occ.c_str(), "channelOccupancy", 20, binEdges_barrel1, 100, 0., 5.);
      }

      auto layer = eventInfo.layerInfo_barrel.at(i);
      // average over event
      layer.occupancy /= nFiles;
      layer.dataRate /= nFiles;
      layer.dataRatePerSurface /= nFiles;

      /*    std::cout << "DataRate for barrel layer " << i << " is: " << layer.dataRate
                    << ", conversionFactor: " << dataRateConversionFactor
                    << " total: " << layer.dataRate * dataRateConversionFactor
                    << ", dataRatePerSurface: " << layer.dataRatePerSurface
                    << ", converted: " << (layer.dataRatePerSurface * dataRateConversionFactor * 1024) << std::endl;*/
      // calculate standard deviations
      float stdOccupancy = 0;
      float stdDataRate = 0;
      layer.standardDev(stdOccupancy, stdDataRate);

      // fill treevalues
      currentOccupancy = (layer.occupancy * 100.);
      currentOccStdDev = (stdOccupancy * 100);
      currentOccupancyMax = (layer.occupancyMax * 100.);
      currentOccupancyMin = (layer.occupancyMin * 100);

      currentDataRate = (layer.dataRate * dataRateConversionFactor);
      currentDataRateDev = (stdDataRate * dataRateConversionFactor);
      currentDataRatePerSurface = (layer.dataRatePerSurface * dataRateConversionFactor * 1024);

      // std::cout << "(Event)occupancy for layer Event " << counter << " is " << currentOccupancy << " +- "
      //            << currentOccStdDev << ", average clusterSizes: " << layer.sizeX << "," << layer.sizeY << std::endl;

      // set occupancy
      occupancy_barrel[counter] = currentOccupancy;
      occupancyErrorL_barrel[counter] = layer.occupancyMin * 100.;
      occupancyErrorH_barrel[counter] = layer.occupancyMax * 100.;

      tree_barrel->Fill();

      std::string layerDirName = "dir_bl" + std::to_string(i);
      TDirectory* dir = outputFile.mkdir(layerDirName.c_str());
      dir->cd();

      const int numberOfRods = layers_barrel.at(i).occupancyPerRod.size();
      float occupancyPerRod[numberOfRods];
      float clusterOccupancyPerRod[numberOfRods];
      float clusterSizePerRod[numberOfRods];
      float rodPosZ[numberOfRods];

      int rodCounter = 0;
      // go through rods
      //    std::cout << "Barrel layer number: " << i << std::endl;
      for (auto& it : layers_barrel.at(i).occupancyPerRod) {
        // std::cout << "Rod number: " << rodCounter << ", zPos: " << it.first << std::endl;
        float occupancyAverageRod = 0;
        float clusterOccupancyAverageRod = 0;
        float clusterSizeAverageRod = 0;
        float previousClusterSize = it.second.at(0);
        // go through all surfaces of this rod
        for (int j = 0; j < it.second.size(); j++) {
          float occupancy = it.second.at(j);
          float clusterOccupancy = layers_barrel.at(i).clusterOccupancyPerRod.at(it.first).at(j);
          float clusterSize = layers_barrel.at(i).clusterSizePerRod.at(it.first).at(j);
          occupancyAverageRod += occupancy;
          clusterOccupancyAverageRod += clusterOccupancy;
          clusterSizeAverageRod += clusterSize;
          // fill histograms
          occupancy_h2->Fill(it.first, occupancy);
          clusterOccupancy_h2->Fill(it.first, clusterOccupancy);
          clusterSizes_h2->Fill(it.first, clusterSize);
        }
        // average
        if (it.second.size()) {
          occupancyAverageRod /= it.second.size();
          clusterOccupancyAverageRod /= it.second.size();
          clusterSizeAverageRod /= it.second.size();
        } else {
          std::cout << "No entries!" << std::endl;
        }
        occupancyPerRod[rodCounter] = occupancyAverageRod;
        clusterOccupancyPerRod[rodCounter] = clusterOccupancyAverageRod;
        clusterSizePerRod[rodCounter] = clusterSizeAverageRod;
        rodPosZ[rodCounter] = it.first;

        rodCounter++;
      }
      //   std::cout << "-------------Next Layer-------------" << std::endl;

      occupancy_h2->Write();
      clusterOccupancy_h2->Write();
      clusterSizes_h2->Write();

      TGraph* occupancy_bL = new TGraph(numberOfRods, rodPosZ, occupancyPerRod);
      occupancy_bL->SetTitle("channel occupancy barrel layer");
      occupancy_bL->GetXaxis()->SetTitle("z[mm]");
      occupancy_bL->GetYaxis()->SetTitle("#channel on/#channels per module");
      occupancy_bL->GetYaxis()->SetRangeUser(0., 3.5);
      occupancy_bL->Write();

      TGraph* clusterOccupancy_bL = new TGraph(numberOfRods, rodPosZ, clusterOccupancyPerRod);
      clusterOccupancy_bL->SetTitle("cluster occupancy barrel layer");
      clusterOccupancy_bL->GetXaxis()->SetTitle("z[mm]");
      clusterOccupancy_bL->GetYaxis()->SetTitle("#clusters/mm^{2}");
      clusterOccupancy_bL->GetYaxis()->SetRangeUser(0., 2.6);
      clusterOccupancy_bL->Write();

      TGraph* clusterSize_bL = new TGraph(numberOfRods, rodPosZ, clusterSizePerRod);
      clusterSize_bL->SetTitle("cluster size barrel layer");
      clusterSize_bL->GetXaxis()->SetTitle("z[mm]");
      clusterSize_bL->GetYaxis()->SetTitle("average cluster size");
      clusterSize_bL->GetYaxis()->SetRangeUser(0., 20.);
      clusterSize_bL->Write();

      outputFile.cd();

      counter++;
    }
    // --------------------------------------------------------
    // ----------------------Plots barrel----------------------
    // --------------------------------------------------------

    TGraphAsymmErrors* occupancies_barrel = new TGraphAsymmErrors(nLayers_barrel, radii_barrel, occupancy_barrel, 0, 0,
                                                                  occupancyErrorL_barrel, occupancyErrorH_barrel);
    occupancies_barrel->SetTitle("channel occupancy barrel layers");
    occupancies_barrel->GetXaxis()->SetTitle("r[mm]");
    occupancies_barrel->GetYaxis()->SetTitle("#channel on/#channels per module");
    occupancies_barrel->Write();

    TGraph* occupancies_barrel_noErrors = new TGraph(nLayers_barrel, radii_barrel, occupancy_barrel);
    occupancies_barrel_noErrors->SetTitle("channel occupancy barrel layers");
    occupancies_barrel_noErrors->GetXaxis()->SetTitle("r[mm]");
    occupancies_barrel_noErrors->GetYaxis()->SetTitle("#channel on/#channels per module");
    occupancies_barrel_noErrors->Write();

    /// average over event Endcaps
    ///  std::cout << "++++++++++++++++ECap" << std::endl;
    for (size_t i = 0; i < nLayers_ec; i++) {
      // pos EC
      // average over events
      eventInfo.layerInfo_ePos.at(i).occupancy /= nFiles;
      eventInfo.layerInfo_ePos.at(i).occupancy_pixel /= nFiles;
      eventInfo.layerInfo_ePos.at(i).occupancy_macroPixel /= nFiles;
      eventInfo.layerInfo_ePos.at(i).occupancy_strip /= nFiles;

      // calculate standard deviations
      float stdOccupancy = 0;
      eventInfo.layerInfo_ePos.at(i).standardDev(stdOccupancy);

      currentOccupancy = (eventInfo.layerInfo_ePos.at(i).occupancy * 100);
      currentOccStdDev = (stdOccupancy * 100);
      currentOccupancyMax = (eventInfo.layerInfo_ePos.at(i).occupancyMax * 100.);
      currentOccupancyMin = (eventInfo.layerInfo_ePos.at(i).occupancyMin * 100);

      currentOccupancy_pixel = (eventInfo.layerInfo_ePos.at(i).occupancy_pixel * 100);
      currentOccupancyMax_pixel = (eventInfo.layerInfo_ePos.at(i).occupancy_pixel_Max * 100.);
      currentOccupancyMin_pixel = (eventInfo.layerInfo_ePos.at(i).occupancy_pixel_Min * 100);

      currentOccupancy_macroPixel = (eventInfo.layerInfo_ePos.at(i).occupancy_macroPixel * 100);
      currentOccupancyMax_macroPixel = (eventInfo.layerInfo_ePos.at(i).occupancy_macroPixel_Max * 100.);
      currentOccupancyMin_macroPixel = (eventInfo.layerInfo_ePos.at(i).occupancy_macroPixel_Min * 100);

      currentOccupancy_strip = (eventInfo.layerInfo_ePos.at(i).occupancy_strip * 100);
      currentOccupancyMax_strip = (eventInfo.layerInfo_ePos.at(i).occupancy_strip_Max * 100.);
      currentOccupancyMin_strip = (eventInfo.layerInfo_ePos.at(i).occupancy_strip_Min * 100);

      /*    std::cout << "(Event)occupancy for layer pos" << i << " is " << currentOccupancy << " +- " <<
         currentOccStdDev
                    << ", average clusterSize: " << eventInfo.layerInfo_ePos.at(i).sizeX << ","
                    << eventInfo.layerInfo_ePos.at(i).sizeY << std::endl;*/

      tree_ePos->Fill();

      // average over events
      eventInfo.layerInfo_eNeg.at(i).occupancy /= nFiles;
      eventInfo.layerInfo_eNeg.at(i).occupancy_pixel /= nFiles;
      eventInfo.layerInfo_eNeg.at(i).occupancy_macroPixel /= nFiles;
      eventInfo.layerInfo_eNeg.at(i).occupancy_strip /= nFiles;

      // calculate standard deviations
      eventInfo.layerInfo_eNeg.at(i).standardDev(stdOccupancy);

      currentOccupancy = (eventInfo.layerInfo_eNeg.at(i).occupancy * 100);
      currentOccStdDev = (stdOccupancy * 100);
      currentOccupancyMax = (eventInfo.layerInfo_eNeg.at(i).occupancyMax * 100.);
      currentOccupancyMin = (eventInfo.layerInfo_eNeg.at(i).occupancyMin * 100);

      currentOccupancy_pixel = (eventInfo.layerInfo_eNeg.at(i).occupancy_pixel * 100);
      currentOccupancyMax_pixel = (eventInfo.layerInfo_eNeg.at(i).occupancy_pixel_Max * 100.);
      currentOccupancyMin_pixel = (eventInfo.layerInfo_eNeg.at(i).occupancy_pixel_Min * 100);

      currentOccupancy_macroPixel = (eventInfo.layerInfo_eNeg.at(i).occupancy_macroPixel * 100);
      currentOccupancyMax_macroPixel = (eventInfo.layerInfo_eNeg.at(i).occupancy_macroPixel_Max * 100.);
      currentOccupancyMin_macroPixel = (eventInfo.layerInfo_eNeg.at(i).occupancy_macroPixel_Min * 100);

      currentOccupancy_strip = (eventInfo.layerInfo_eNeg.at(i).occupancy_strip * 100);
      currentOccupancyMax_strip = (eventInfo.layerInfo_eNeg.at(i).occupancy_strip_Max * 100.);
      currentOccupancyMin_strip = (eventInfo.layerInfo_eNeg.at(i).occupancy_strip_Min * 100);

      /*  std::cout << "(Event)occupancy for layer neg" << i << " is " << currentOccupancy << " +- " << currentOccStdDev
                  << ", average clusterSize: " << eventInfo.layerInfo_eNeg.at(i).sizeX << ","
                  << eventInfo.layerInfo_eNeg.at(i).sizeY << std::endl;
  */
      tree_eNeg->Fill();

      ///   std::cout << "-----" << std::endl;

      // neg EC
    }

    // output for different regions in EC
    std::string layerDirName_ePixel = "dir_ePixel";
    TDirectory* dir_ePixel = outputFile.mkdir(layerDirName_ePixel.c_str());
    dir_ePixel->cd();

    float occupancyPerRod_pixel[nLayers_ec];
    float clusterOccupancyPerRod_pixel[nLayers_ec];
    float clusterSizePerRod_pixel[nLayers_ec];

    int rodCounter = 0;
    for (auto& it : eCapRegionInfo.at(0).occupancyPerRod) {

      // go through r
      float occupancyAverageRod = 0;
      float clusterOccupancyAverageRod = 0;
      float clusterSizeAverageRod = 0;
      for (int j = 0; j < it.second.size(); j++) {
        float occupancy = it.second.at(j);
        float clusterOccupancy = eCapRegionInfo.at(0).clusterOccupancyPerRod.at(it.first).at(j);
        float clusterSize = eCapRegionInfo.at(0).clusterSizePerRod.at(it.first).at(j);

        occupancyAverageRod += occupancy;
        clusterOccupancyAverageRod += clusterOccupancy;
        clusterSizeAverageRod += clusterSize;
      }
      // average
      if (it.second.size()) {
        occupancyAverageRod /= it.second.size();
        clusterOccupancyAverageRod /= it.second.size();
        clusterSizeAverageRod /= it.second.size();
      } else {
        std::cout << "No entries!" << std::endl;
      }
      /* float sumOccDev = 0.;
       for (int j = 0; j < it.second.size(); j++) {
         float diff = (it.second.at(j) - occupancyAverageRod);
         sumOccDev += (diff * diff);
       }
       if (it.second.size() > 1) {
         sumOccDev /= it.second.size();
       }*/
      //   float sdDevOccRod = sqrt(sumOccDev);
      occupancyPerRod_pixel[rodCounter] = occupancyAverageRod;
      clusterOccupancyPerRod_pixel[rodCounter] = clusterOccupancyAverageRod;
      clusterSizePerRod_pixel[rodCounter] = clusterSizeAverageRod;

      //    std::cout << "it.first: " << it.first << ", occupancy:" << occupancyAverageRod << " rodCounter: " <<
      //    rodCounter
      //              << std::endl;

      rodCounter++;
    }
    // std::cout << "-------------Next Layer-------------" << std::endl;

    TGraph* occupancy_ec_pixel = new TGraph(nLayers_ec, PosZ_ec, occupancyPerRod_pixel);
    occupancy_ec_pixel->SetTitle("channel occupancy endcap pixel area");
    occupancy_ec_pixel->GetXaxis()->SetTitle("z[mm]");
    occupancy_ec_pixel->GetYaxis()->SetTitle("#channel on/#channels per module");
    occupancy_ec_pixel->GetYaxis()->SetRangeUser(0., 3.5);
    occupancy_ec_pixel->Write();

    TGraph* clusterOccupancy_ec_pixel = new TGraph(nLayers_ec, PosZ_ec, clusterOccupancyPerRod_pixel);
    clusterOccupancy_ec_pixel->SetTitle("cluster occupancy endcap pixel area");
    clusterOccupancy_ec_pixel->GetXaxis()->SetTitle("z[mm]");
    clusterOccupancy_ec_pixel->GetYaxis()->SetTitle("#clusters/mm^{2}");
    clusterOccupancy_ec_pixel->GetYaxis()->SetRangeUser(0., 2.6);
    clusterOccupancy_ec_pixel->Write();

    TGraph* clusterSize_ec_pixel = new TGraph(nLayers_ec, PosZ_ec, clusterSizePerRod_pixel);
    clusterSize_ec_pixel->SetTitle("cluster size endcap pixel area");
    clusterSize_ec_pixel->GetXaxis()->SetTitle("z[mm]");
    clusterSize_ec_pixel->GetYaxis()->SetTitle("average cluster size");
    clusterSize_ec_pixel->GetYaxis()->SetRangeUser(0., 20.);
    clusterSize_ec_pixel->Write();

    outputFile.cd();

    std::string layerDirName_eMacroPixel = "dir_eMacroPixel";
    TDirectory* dir_eMacroPixel = outputFile.mkdir(layerDirName_eMacroPixel.c_str());
    dir_eMacroPixel->cd();

    float occupancyPerRod_macroPixel[nLayers_ec];
    float clusterOccupancyPerRod_macroPixel[nLayers_ec];
    float clusterSizePerRod_macroPixel[nLayers_ec];

    rodCounter = 0;
    for (auto& it : eCapRegionInfo.at(1).occupancyPerRod) {

      // go through rods
      float occupancyAverageRod = 0;
      float clusterOccupancyAverageRod = 0;
      float clusterSizeAverageRod = 0;
      for (int j = 0; j < it.second.size(); j++) {
        float occupancy = it.second.at(j);
        float clusterOccupancy = eCapRegionInfo.at(1).clusterOccupancyPerRod.at(it.first).at(j);
        float clusterSize = eCapRegionInfo.at(1).clusterSizePerRod.at(it.first).at(j);

        occupancyAverageRod += occupancy;
        clusterOccupancyAverageRod += clusterOccupancy;
        clusterSizeAverageRod += clusterSize;
      }
      // average
      if (it.second.size()) {
        occupancyAverageRod /= it.second.size();
        clusterOccupancyAverageRod /= it.second.size();
        clusterSizeAverageRod /= it.second.size();
      } else {
        std::cout << "No entries!" << std::endl;
      }
      occupancyPerRod_macroPixel[rodCounter] = occupancyAverageRod;
      clusterOccupancyPerRod_macroPixel[rodCounter] = clusterOccupancyAverageRod;
      clusterSizePerRod_macroPixel[rodCounter] = clusterSizeAverageRod;

      rodCounter++;
    }
    //    std::cout << "-------------Next Layer-------------" << std::endl;

    TGraph* occupancy_ec_macroPixel = new TGraph(nLayers_ec, PosZ_ec, occupancyPerRod_macroPixel);
    occupancy_ec_macroPixel->SetTitle("channel occupancy endcap macro pixel area");
    occupancy_ec_macroPixel->GetXaxis()->SetTitle("z[mm]");
    occupancy_ec_macroPixel->GetYaxis()->SetTitle("#channel on/#channels per module");
    occupancy_ec_macroPixel->GetYaxis()->SetRangeUser(0., 3.5);
    occupancy_ec_macroPixel->Write();

    TGraph* clusterOccupancy_ec_macroPixel = new TGraph(nLayers_ec, PosZ_ec, clusterOccupancyPerRod_macroPixel);
    clusterOccupancy_ec_macroPixel->SetTitle("cluster occupancy endcap macro pixel area");
    clusterOccupancy_ec_macroPixel->GetXaxis()->SetTitle("z[mm]");
    clusterOccupancy_ec_macroPixel->GetYaxis()->SetTitle("#clusters/mm^{2}");
    clusterOccupancy_ec_macroPixel->GetYaxis()->SetRangeUser(0., 2.6);
    clusterOccupancy_ec_macroPixel->Write();

    TGraph* clusterSize_ec_macroPixel = new TGraph(nLayers_ec, PosZ_ec, clusterSizePerRod_macroPixel);
    clusterSize_ec_macroPixel->SetTitle("cluster size endcap pixel area");
    clusterSize_ec_macroPixel->GetXaxis()->SetTitle("z[mm]");
    clusterSize_ec_macroPixel->GetYaxis()->SetTitle("average cluster size");
    clusterSize_ec_macroPixel->GetYaxis()->SetRangeUser(0., 20.);
    clusterSize_ec_macroPixel->Write();

    outputFile.cd();

    std::string layerDirName_eStrip = "dir_eStrip";
    TDirectory* dir_eStrip = outputFile.mkdir(layerDirName_eStrip.c_str());
    dir_eStrip->cd();

    float occupancyPerRod_strip[nLayers_ec];
    float clusterOccupancyPerRod_strip[nLayers_ec];
    float clusterSizePerRod_strip[nLayers_ec];

    rodCounter = 0;
    for (auto& it : eCapRegionInfo.at(2).occupancyPerRod) {

      // go through rods
      float occupancyAverageRod = 0;
      float clusterOccupancyAverageRod = 0;
      float clusterSizeAverageRod = 0;
      for (int j = 0; j < it.second.size(); j++) {
        float occupancy = it.second.at(j);
        float clusterOccupancy = eCapRegionInfo.at(2).clusterOccupancyPerRod.at(it.first).at(j);
        float clusterSize = eCapRegionInfo.at(2).clusterSizePerRod.at(it.first).at(j);

        occupancyAverageRod += occupancy;
        clusterOccupancyAverageRod += clusterOccupancy;
        clusterSizeAverageRod += clusterSize;
      }
      // average
      if (it.second.size()) {
        occupancyAverageRod /= it.second.size();
        clusterOccupancyAverageRod /= it.second.size();
        clusterSizeAverageRod /= it.second.size();
      } else {
        std::cout << "No entries!" << std::endl;
      }

      occupancyPerRod_strip[rodCounter] = occupancyAverageRod;
      clusterOccupancyPerRod_strip[rodCounter] = clusterOccupancyAverageRod;
      clusterSizePerRod_strip[rodCounter] = clusterSizeAverageRod;

      rodCounter++;
    }
    //  std::cout << "-------------Next Layer-------------" << std::endl;

    TGraph* occupancy_ec_strip = new TGraph(14, PosZ_ec_strip, occupancyPerRod_strip);
    occupancy_ec_strip->SetTitle("channel occupancy endcap macro pixel area");
    occupancy_ec_strip->GetXaxis()->SetTitle("z[mm]");
    occupancy_ec_strip->GetYaxis()->SetTitle("#channel on/#channels per module");
    occupancy_ec_strip->GetYaxis()->SetRangeUser(0., 3.5);
    occupancy_ec_strip->Write();

    TGraph* clusterOccupancy_ec_strip = new TGraph(14, PosZ_ec_strip, clusterOccupancyPerRod_strip);
    clusterOccupancy_ec_strip->SetTitle("cluster occupancy endcap macro pixel area");
    clusterOccupancy_ec_strip->GetXaxis()->SetTitle("z[mm]");
    clusterOccupancy_ec_strip->GetYaxis()->SetTitle("#clusters/mm^{2}");
    clusterOccupancy_ec_strip->GetYaxis()->SetRangeUser(0., 2.6);
    clusterOccupancy_ec_strip->Write();

    TGraph* clusterSize_ec_strip = new TGraph(14, PosZ_ec_strip, clusterSizePerRod_strip);
    clusterSize_ec_strip->SetTitle("cluster occupancy endcap pixel area");
    clusterSize_ec_strip->GetXaxis()->SetTitle("z[mm]");
    clusterSize_ec_strip->GetYaxis()->SetTitle("average cluster size");
    clusterSize_ec_strip->GetYaxis()->SetRangeUser(0., 20.);
    clusterSize_ec_strip->Write();

    //  std::cout << "# Pixels on: " << nPixelsOnPerEvent << ", #clusters: " << nClustersPerEvent << std::endl;

    outputFile.cd();

    //----------------------------Write out information per ring--------------------------

    float currentOccupancy_rings = 0;
    float currentOccStdDev_rings = 0;
    float currentDataRate_rings = 0;
    float currentDataRateStdDev_rings = 0;
    float currentDataRatePerSurface_rings = 0;
    // inner Ec
    TTree* tree_innerEc_rings = new TTree("tree_innerEc_rings", "tree_innerEc_rings");

    tree_innerEc_rings->SetDirectory(0);

    tree_innerEc_rings->Branch("occupancy", &currentOccupancy_rings);
    tree_innerEc_rings->Branch("occupancyDev", &currentOccStdDev_rings);
    tree_innerEc_rings->Branch("dataRate", &currentDataRate_rings);
    tree_innerEc_rings->Branch("dataRateDev", &currentDataRateStdDev_rings);
    tree_innerEc_rings->Branch("dataRatePerSurface", &currentDataRatePerSurface_rings);

    // average rings
    for (auto& ring : innerEndcapEventInfo) {
      ring.occupancy /= nFiles;
      ring.dataRate /= nFiles;
      ring.dataRatePerSurface /= nFiles;
      // set event info
      currentOccupancy_rings = ring.occupancy * 100.;
      float stdDev_occ = 0;
      float stdDev_rates = 0;
      ring.standardDev(stdDev_occ, stdDev_rates);
      currentOccStdDev_rings = stdDev_occ * 100.;
      // data rates
      currentDataRate_rings = ring.dataRate * dataRateConversionFactor;
      currentDataRateStdDev_rings = stdDev_rates * dataRateConversionFactor;

      currentDataRatePerSurface_rings = ring.dataRatePerSurface * dataRateConversionFactor * 1024;  // in Gb
      tree_innerEc_rings->Fill();
    }

    tree_innerEc_rings->Write();

    // outer Ec
    TTree* tree_outerEc_rings = new TTree("tree_outerEc_rings", "tree_outerEc_rings");

    tree_outerEc_rings->SetDirectory(0);

    tree_outerEc_rings->Branch("occupancy", &currentOccupancy_rings);
    tree_outerEc_rings->Branch("occupancyDev", &currentOccStdDev_rings);
    tree_outerEc_rings->Branch("dataRate", &currentDataRate_rings);
    tree_outerEc_rings->Branch("dataRateDev", &currentDataRateStdDev_rings);
    tree_outerEc_rings->Branch("dataRatePerSurface", &currentDataRatePerSurface_rings);

    // average rings
    for (auto& ring : outerEndcapEventInfo) {
      ring.occupancy /= nFiles;
      ring.dataRate /= nFiles;
      ring.dataRatePerSurface /= nFiles;
      // set event info
      currentOccupancy_rings = ring.occupancy * 100.;
      float stdDev_occ = 0;
      float stdDev_rates = 0;
      ring.standardDev(stdDev_occ, stdDev_rates);
      currentOccStdDev_rings = stdDev_occ * 100.;
      // data rates
      currentDataRate_rings = ring.dataRate * dataRateConversionFactor;
      currentDataRateStdDev_rings = stdDev_rates * dataRateConversionFactor;
      currentDataRatePerSurface_rings = ring.dataRatePerSurface * dataRateConversionFactor * 1024;  // in Gb
      tree_outerEc_rings->Fill();
    }

    tree_outerEc_rings->Write();

    // fwd0

    TTree* tree_fwd0Ec_rings = new TTree("tree_fwd0Ec_rings", "tree_fwd0Ec_rings");

    tree_fwd0Ec_rings->SetDirectory(0);

    tree_fwd0Ec_rings->Branch("occupancy", &currentOccupancy_rings);
    tree_fwd0Ec_rings->Branch("occupancyDev", &currentOccStdDev_rings);
    tree_fwd0Ec_rings->Branch("dataRate", &currentDataRate_rings);
    tree_fwd0Ec_rings->Branch("dataRateDev", &currentDataRateStdDev_rings);
    tree_fwd0Ec_rings->Branch("dataRatePerSurface", &currentDataRatePerSurface_rings);

    // average rings
    for (auto& ring : fwd0EndcapEventInfo) {
      ring.occupancy /= nFiles;
      ring.dataRate /= nFiles;
      ring.dataRatePerSurface /= nFiles;
      // set event info
      currentOccupancy_rings = ring.occupancy * 100.;
      float stdDev_occ = 0;
      float stdDev_rates = 0;
      ring.standardDev(stdDev_occ, stdDev_rates);
      currentOccStdDev_rings = stdDev_occ * 100.;
      // data rates
      currentDataRate_rings = ring.dataRate * dataRateConversionFactor;
      currentDataRateStdDev_rings = stdDev_rates * dataRateConversionFactor;
      currentDataRatePerSurface_rings = ring.dataRatePerSurface * dataRateConversionFactor * 1024;  // in Gb
      tree_fwd0Ec_rings->Fill();
    }

    tree_fwd0Ec_rings->Write();

    // fwd1

    TTree* tree_fwd1Ec_rings = new TTree("tree_fwd1Ec_rings", "tree_fwd1Ec_rings");

    tree_fwd1Ec_rings->SetDirectory(0);

    tree_fwd1Ec_rings->Branch("occupancy", &currentOccupancy_rings);
    tree_fwd1Ec_rings->Branch("occupancyDev", &currentOccStdDev_rings);
    tree_fwd1Ec_rings->Branch("dataRate", &currentDataRate_rings);
    tree_fwd1Ec_rings->Branch("dataRateDev", &currentDataRateStdDev_rings);
    tree_fwd1Ec_rings->Branch("dataRatePerSurface", &currentDataRatePerSurface_rings);

    // average rings
    for (auto& ring : fwd1EndcapEventInfo) {
      ring.occupancy /= nFiles;
      ring.dataRate /= nFiles;
      ring.dataRatePerSurface /= nFiles;
      // set event info
      currentOccupancy_rings = ring.occupancy * 100.;
      float stdDev_occ = 0;
      float stdDev_rates = 0;
      ring.standardDev(stdDev_occ, stdDev_rates);
      currentOccStdDev_rings = stdDev_occ * 100.;
      // data rates
      currentDataRate_rings = ring.dataRate * dataRateConversionFactor;
      currentDataRateStdDev_rings = stdDev_rates * dataRateConversionFactor;
      currentDataRatePerSurface_rings = ring.dataRatePerSurface * dataRateConversionFactor * 1024;  // in Gb
      tree_fwd1Ec_rings->Fill();
    }

    tree_fwd1Ec_rings->Write();

    eventTree->Write();

    outputFile.cd();

    tree_barrel->Write();
    tree_ePos->Write();
    tree_eNeg->Write();

    outputFile.Close();
  }  // check if files are present
}
