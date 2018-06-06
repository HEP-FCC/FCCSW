// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TTree.h"
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <numeric>

#include "TColor.h"
#include "TROOT.h"
#include "TStyle.h"

#include "ActsStyle.hpp"
#include "VolumesAndLayers.hpp"

// This root script creates different histograms displaying the sensitive
// material, the boundaries and the material of the detector in different views.
// The in file needs to be in the format created form the ExtrapolationTest.
// To plot two or three histograms of this kind  in one canvas the root script
// "compareHistogram.cpp" can be used.

void occupancies(std::string inFile = "clusters.root", std::string treeName = "clusters",
                 std::string outFile = "occupancies.root", bool energyCut = true) {

  std::cout << "Opening file: " << inFile << std::endl;
  TFile inputFile(inFile.c_str());
  std::cout << "Reading tree: " << treeName << std::endl;
  TTreeReader reader(treeName.c_str(), &inputFile);
  // @todo average over events and files
  TTreeReaderValue<int> _eventNr(reader, "event_nr");
  TTreeReaderValue<int> _volumeID(reader, "volumeID");
  TTreeReaderValue<int> _layerID(reader, "layerID");
  TTreeReaderValue<int> _surfaceID(reader, "surfaceID");
  TTreeReaderValue<float> _x(reader, "g_x");
  TTreeReaderValue<float> _y(reader, "g_y");
  TTreeReaderValue<float> _z(reader, "g_z");
  TTreeReaderValue<int> _nChannels(reader, "nChannels");
  TTreeReaderValue<int> _nChannelsOn(reader, "nChannelsOn");
  TTreeReaderValue<int> _nHitsPerCluster(reader, "nTracksPerCluster");
  TTreeReaderValue<int> _size_x(reader, "size_x");
  TTreeReaderValue<int> _size_y(reader, "size_y");

  utilities::setActsStyle();

  TProfile2D* channelOccupancy_h =
      new TProfile2D("channelOccupancy_h", "#channels on/#channels per module", 1000, -16150., 16150., 69, 0., 1600.);
  channelOccupancy_h->GetXaxis()->SetTitle("z[mm]");
  channelOccupancy_h->GetYaxis()->SetTitle("r[mm]");

  TProfile2D* clusterSizesPhi_h =
      new TProfile2D("clusterSizesPhi_h", "Cluster sizes per Surface", 1000, -16150., 16150., 69, 0., 1600.);
  clusterSizesPhi_h->GetXaxis()->SetTitle("z[mm]");
  clusterSizesPhi_h->GetYaxis()->SetTitle("r[mm]");

  TProfile2D* clusterSizesLong_h =
      new TProfile2D("clusterSizesLong_h", "Cluster sizes per Surface", 1000, -16150., 16150., 69, 0., 1600.);
  clusterSizesLong_h->GetXaxis()->SetTitle("z[mm]");
  clusterSizesLong_h->GetYaxis()->SetTitle("r[mm]");

  TProfile2D* nHitsPerCluster_h =
      new TProfile2D("nHitsPerCluster_h", "#Hits per Cluster", 1000, -16150., 16150., 69, 0., 1600.);
  nHitsPerCluster_h->GetXaxis()->SetTitle("z[mm]");
  nHitsPerCluster_h->GetYaxis()->SetTitle("r[mm]");

  TProfile2D* nHitsPerChannel_h =
      new TProfile2D("nHitsPerChannel_h", "#Hits per Channel", 1000, -16150., 16150., 69, 0., 1600.);
  nHitsPerChannel_h->GetXaxis()->SetTitle("z[mm]");
  nHitsPerChannel_h->GetYaxis()->SetTitle("r[mm]");

  TH2F* channelOccupancy_h_barrel =
      new TH2F("channelOccupancy_h_barrel", "#channel on/#channels per module in %", 69, 0., 1600., 100, 0., 25.);
  channelOccupancy_h->GetXaxis()->SetTitle("z[mm]");
  channelOccupancy_h->GetYaxis()->SetTitle("r[mm]");

  TProfile2D* mergedClusterRate_h =
      new TProfile2D("mergedClusterRate_h", "Merged Cluster Rate", 1000, -16150., 16150., 69, 0., 1600.);
  mergedClusterRate_h->GetXaxis()->SetTitle("z[mm]");
  mergedClusterRate_h->GetYaxis()->SetTitle("r[mm]");

  TH2F* mergedClusterRate_h_barrel =
      new TH2F("mergedClusterRate_h_barrel", "Merged Cluster Rate", 69, 0., 1600., 100, 0., 1.);
  mergedClusterRate_h->GetXaxis()->SetTitle("z[mm]");
  mergedClusterRate_h->GetYaxis()->SetTitle("r[mm]");
  // create map matching volume, layer and surfaceIDs
  std::unordered_map<int, VolumeHelper> IDMap;
  // fill parameters per surface
  while (reader.Next()) {
    // get the layer helper at that position
    LayerHelper& layerHelper = IDMap[*_volumeID].layerIDs[*_layerID];
    // get surface information for this layer
    SurfaceInformation& surfaceHelper = layerHelper.surfaceIDs[*_surfaceID];
    // create surface information
    surfaceHelper.nChannels = *_nChannels;
    surfaceHelper.nChannelsVec.push_back(*_nChannels);
    surfaceHelper.nChannelsOn += *_nChannelsOn;
    surfaceHelper.nTracksPerCluster.push_back(*_nHitsPerCluster);
    surfaceHelper.nHits += *_nHitsPerCluster;
    if (*_nHitsPerCluster > 1) surfaceHelper.nMergedClusters += 1;
    surfaceHelper.nClusters += 1;

    if (*_nChannelsOn > (*_size_x) * (*_size_y)) {
      throw std::runtime_error(
          std::string("ERROR, number of channels on (") + std::to_string(*_nChannelsOn) +
          std::string(")  > size_x*size_y (") + std::to_string((*_size_x) * (*_size_y)) +
          std::string(") of cluster. This should never happen, please check the clustering algorithm. "));
    }
    surfaceHelper.size_x.push_back(*_size_x);
    surfaceHelper.size_y.push_back(*_size_y);
    if (surfaceHelper.nChannelsOn >= surfaceHelper.nChannels) {
      throw std::runtime_error(
          std::string("ERROR, number of channels on (") + std::to_string(surfaceHelper.nChannelsOn) +
          std::string(")  > total number of channels (") + std::to_string(surfaceHelper.nChannels) +
          std::string(") of module. This should never happen, please check the clustering algorithm. "));
    }
    surfaceHelper.x.push_back(*_x);
    surfaceHelper.y.push_back(*_y);
    surfaceHelper.z.push_back(*_z);
  }

  std::cout << "Creating new output file: " << outFile << " and writing out histograms. " << std::endl;
  TFile outputFile(outFile.c_str(), "recreate");

  // for TGraphError
  // go through volumes
  // @todo
  size_t nLayers_barrel = 12;
  size_t nLayers_endcap = 40;

  float occupany_barrel[nLayers_barrel];
  float rPos_barrel[nLayers_barrel];
  float occupancyErrorL_barrel[nLayers_barrel];
  float occupancyErrorH_barrel[nLayers_barrel];

  float nHits_barrel[nLayers_barrel];
  float nHitsErrorL_barrel[nLayers_barrel];
  float nHitsErrorH_barrel[nLayers_barrel];

  float nHits_endcap[nLayers_endcap];
  float nHitsErrorL_endcap[nLayers_endcap];
  float nHitsErrorH_endcap[nLayers_endcap];
  float zPos_endcap[nLayers_endcap];

  float meanMergedClusters_barrel[nLayers_barrel];
  float mergedClustersErrorL_barrel[nLayers_barrel];
  float mergedClustersErrorH_barrel[nLayers_barrel];
  //
  // global layerID
  int globLid_barrel = 0;
  int globLid_endcap = 0;

  for (auto vIterator : IDMap) {
    auto vid = vIterator.first;
    std::string vidName = "vid_" + std::to_string(vid);
    // create a sub directory for the volume
    TDirectory* vdir = outputFile.mkdir(vidName.c_str());
    vdir->cd();
    auto layers = vIterator.second.layerIDs;

    for (auto layerIterator : layers) {

      // layer id
      int lid = layerIterator.first;
      // unique layer name
      std::string lidName = vidName + "_lid_" + std::to_string(lid);
      // get the bounds
      auto layerBounds = layerIterator.second.bounds();
      auto layerType = layerIterator.second.layerType(layerBounds);
      //   std::cout << "test1" << std::endl;
      // the layer directory
      std::string layerDirName = (layerType == 1) ? ("cyl_r(" + std::to_string(layerBounds[2]) + ")")
                                                  : ("disc_(" + std::to_string(layerBounds[0]) + ")");
      TDirectory* ldir = vdir->mkdir(layerDirName.c_str());
      ldir->cd();
      //   std::cout << "test2" << std::endl;
      // @todo check nBins
      auto nBins = layerIterator.second.nBins(74., 0.52, 30.);
      auto nBinsRZ = (layerType == 1) ? nBins.at(2) : nBins.at(0);
      layerIterator.second.initHists(lidName.c_str(), layerBounds, layerType, nBinsRZ, nBins.at(1));
      //   std::cout << "test3" << std::endl;
      auto surfaces = layerIterator.second.surfaceIDs;
      // occupancy per layer
      float meanOccupancy = 0;
      float minOccupancy = std::numeric_limits<double>::max();
      float maxOccupancy = std::numeric_limits<double>::min();
      // nhits per layer
      float meanNHits = 0;
      float minNHits = std::numeric_limits<double>::max();
      float maxNHits = std::numeric_limits<double>::min();
      // max nClusters per surface
      int nClustersPerSurfaceMax = std::numeric_limits<double>::min();
      // occupancy per layer at z0
      float meanOccupancyZ0 = 0;
      int z0Counter = 0;

      std::vector<float> occupancies;
      int nChannelsPerLayerOn = 0;
      int nChannelsPerLayer = 0;

      float meanMergedClusterRate = 0.;
      float minMergedClusterRate = std::numeric_limits<double>::max();
      float maxMergedClusterRate = std::numeric_limits<double>::min();

      for (auto surfaceIterator : surfaces) {
        std::string sidName = lidName + "_sid_" + std::to_string(surfaceIterator.first);
        float layerRadius = 0.5 * (layerBounds[2] + layerBounds[3]);
        // number of channels should all be the same on one surface - check
        surfaceIterator.second.checkChannels();
        // position
        // surface r position
        float surfR = surfaceIterator.second.positionRPhiZ().at(0);
        // surface z position
        float surfZ = surfaceIterator.second.positionRPhiZ().at(2);
        // surface phi position
        float surfPhi = surfaceIterator.second.positionRPhiZ().at(1);
        // channel ratio
        //   std::cout << "test5" << std::endl;
        float channelRatio =
            (float(surfaceIterator.second.nChannelsOn) / float(surfaceIterator.second.nChannels)) * 100;  // in percent

        nChannelsPerLayer += surfaceIterator.second.nChannels;
        nChannelsPerLayerOn += surfaceIterator.second.nChannelsOn;
        if (channelRatio >= 100)
          std::cout << "channelRatio >= 100%: "
                    << ", surfaceIterator.second.nChannels: " << surfaceIterator.second.nChannels
                    << ", surfaceIterator.second.nChannelsOn: " << surfaceIterator.second.nChannelsOn << std::endl;
        // cluster size
        auto clusterSize = surfaceIterator.second.nChannelsOn;
        // number of tracks per channel
        //  std::cout << "test6" << std::endl;
        float nHitsPerChannel = float(surfaceIterator.second.nHits) / float(surfaceIterator.second.nChannelsOn);
        // number of tracks per cluster
        float nHitsPerCluster = float(surfaceIterator.second.nHits) / float(surfaceIterator.second.nClusters);
        // fill the global histograms per surface
        //   std::cout << "test7" << std::endl;
        if (!channelOccupancy_h) std::cout << "!channelOccupancy_h" << std::endl;
        channelOccupancy_h->Fill(surfZ, surfR, channelRatio);
        //  std::cout << "test7.1" << std::endl;
        // fill channel occupancy per layer
        float surfRef = (layerType == 1) ? surfZ : surfR;
        // channel occupancy for barrel layers
        if (layerType == 1) channelOccupancy_h_barrel->Fill(surfR, channelRatio);
        // fill layer plots
        layerIterator.second.channel_occupany->Fill(surfRef, surfPhi, channelRatio);
        layerIterator.second.hitsPerSurface->Fill(surfRef, surfPhi, surfaceIterator.second.nHits);

        float clusterMergingRate =
            float(surfaceIterator.second.nMergedClusters);  // / float(surfaceIterator.second.nClusters);
        layerIterator.second.clusterMerging->Fill(clusterMergingRate);
        mergedClusterRate_h->Fill(surfZ, surfR, clusterMergingRate);
        if (layerType == 1) mergedClusterRate_h_barrel->Fill(surfR, clusterMergingRate);
        if (clusterMergingRate < minMergedClusterRate) minMergedClusterRate = clusterMergingRate;
        if (clusterMergingRate > maxMergedClusterRate) maxMergedClusterRate = clusterMergingRate;
        meanMergedClusterRate += clusterMergingRate;
        //   std::cout << "test8" << std::endl;
        // probably write out per cluster nor per surface

        nHitsPerCluster_h->Fill(surfZ, surfR, nHitsPerCluster);
        nHitsPerChannel_h->Fill(surfZ, surfR, nHitsPerChannel);
        // mean min max occupancy per layer
        meanOccupancy += channelRatio;

        if (channelRatio < minOccupancy) minOccupancy = channelRatio;
        if (channelRatio > maxOccupancy) maxOccupancy = channelRatio;

        // mean min max nhits per layer
        // todo change to nHits again
        meanNHits += surfaceIterator.second.nHits;
        if (surfaceIterator.second.nHits < minNHits) minNHits = surfaceIterator.second.nHits;
        if (surfaceIterator.second.nHits > maxNHits) maxNHits = surfaceIterator.second.nHits;

        if (surfaceIterator.second.nClusters > nClustersPerSurfaceMax)
          nClustersPerSurfaceMax = surfaceIterator.second.nClusters;
        occupancies.push_back(channelRatio);
        // cluster sizes
        // std::cout << "#cluster sizes: " << surfaceIterator.second.size_x.size() << ", "
        //           << surfaceIterator.second.size_y.size() << std::endl;
        for (size_t i = 0; i < surfaceIterator.second.size_x.size(); i++) {
          //  std::cout << "surfaceIterator.second.size_x.at(i)): " << surfaceIterator.second.size_x.at(i) << std::endl;
          layerIterator.second.clustersize_0->Fill(surfaceIterator.second.size_x.at(i));
          layerIterator.second.clustersize_1->Fill(surfaceIterator.second.size_y.at(i));

          clusterSizesPhi_h->Fill(surfZ, surfR, surfaceIterator.second.size_x.at(i));
          clusterSizesLong_h->Fill(surfZ, surfR, surfaceIterator.second.size_y.at(i));
          float phi = atan2(surfaceIterator.second.y.at(i), surfaceIterator.second.x.at(i));

          if (surfaceIterator.second.nTracksPerCluster.at(i) >= 2)
            layerIterator.second.clusterMerging_2D->Fill(surfaceIterator.second.z.at(i), phi);
          if (surfaceIterator.second.z.at(i) < 100. && surfaceIterator.second.z.at(i) > -100.) {
            meanOccupancyZ0 += channelRatio;
            z0Counter++;
          }
        }

      }  // surfaces
         //  std::cout << "test9" << std::endl;
      if (surfaces.size() > 1) {
        meanOccupancy /= surfaces.size();
        meanNHits /= surfaces.size();
        // meanMergedClusterRate /= surfaces.size();
      }
      if (z0Counter > 1) meanOccupancyZ0 /= z0Counter;

      layerIterator.second.channel_occupany->Write();
      layerIterator.second.hitsPerSurface->Write();
      layerIterator.second.clustersize_0->Write();
      layerIterator.second.clustersize_1->Write();
      layerIterator.second.clusterMerging_2D->Write();
      layerIterator.second.clusterMerging->Write();

      // calculate rms
      float occupancyRMS = 0;
      for (auto& i : occupancies) {
        float diff = (i - meanOccupancy);
        occupancyRMS += (diff * diff);
      }
      if (occupancies.size() > 1) occupancyRMS /= occupancies.size();
      occupancyRMS = sqrt(occupancyRMS);
      // just print for barrel
      if (layerType == 1) {

        occupany_barrel[globLid_barrel] = meanOccupancy;
        occupancyErrorL_barrel[globLid_barrel] = minOccupancy;
        occupancyErrorH_barrel[globLid_barrel] = maxOccupancy;

        meanMergedClusters_barrel[globLid_barrel] = meanMergedClusterRate;
        mergedClustersErrorL_barrel[globLid_barrel] = minMergedClusterRate;
        mergedClustersErrorH_barrel[globLid_barrel] = maxMergedClusterRate;

        rPos_barrel[globLid_barrel] = 0.5 * (layerBounds[2] + layerBounds[3]);
        std::cout << "**********Barrel layer at r = " << 0.5 * (layerBounds[2] + layerBounds[3]) << " **********"
                  << std::endl;
        std::cout << "#Channels per layer: " << nChannelsPerLayer << std::endl;
        if (surfaces.size() > 1) nChannelsPerLayer /= surfaces.size();
        std::cout << "#Channels per module: " << nChannelsPerLayer << std::endl;
        std::cout << "#ChannelsON per layer: " << nChannelsPerLayerOn << std::endl;
        if (surfaces.size() > 1) nChannelsPerLayerOn /= surfaces.size();
        std::cout << "#ChannelsON per module: " << nChannelsPerLayerOn << std::endl;
        std::cout << "meanOccupancy: " << meanOccupancy << ", sigma: " << occupancyRMS
                  << ", minOccupancy: " << minOccupancy << ", maxOccupancy: " << maxOccupancy << std::endl;
        std::cout << "meanOccupancy around z=0 (z>-100.mm&&z<100.mm) : " << meanOccupancyZ0 << std::endl;
        std::cout << "MeanNumber of hits from different tracks per Surface: " << meanNHits << std::endl;
        std::cout << "Max number of clusters per surface: " << nClustersPerSurfaceMax << std::endl;
        std::cout << "Merged Cluster Rate: " << meanMergedClusterRate << ", min: " << minMergedClusterRate
                  << ", max: " << maxMergedClusterRate << std::endl;
        std::cout << "**********************************************************************" << std::endl;

        nHits_barrel[globLid_barrel] = meanNHits;
        nHitsErrorL_barrel[globLid_barrel] = minNHits;
        nHitsErrorH_barrel[globLid_barrel] = maxNHits;

        globLid_barrel++;
      } else {
        nHits_endcap[globLid_endcap] = meanNHits;
        nHitsErrorL_endcap[globLid_endcap] = minNHits;
        nHitsErrorH_endcap[globLid_endcap] = maxNHits;

        zPos_endcap[globLid_endcap] = 0.5 * (layerBounds[0] + layerBounds[1]);

        std::cout << "**********Disc layer at r = " << 0.5 * (layerBounds[0] + layerBounds[1]) << " **********"
                  << std::endl;
        std::cout << "#Channels per layer: " << nChannelsPerLayer << std::endl;
        if (surfaces.size() > 1) nChannelsPerLayer /= surfaces.size();
        std::cout << "#Channels per module: " << nChannelsPerLayer << std::endl;
        std::cout << "#ChannelsON per layer: " << nChannelsPerLayerOn << std::endl;
        if (surfaces.size() > 1) nChannelsPerLayerOn /= surfaces.size();
        std::cout << "#ChannelsON per module: " << nChannelsPerLayerOn << std::endl;
        std::cout << "meanOccupancy: " << meanOccupancy << ", sigma: " << occupancyRMS
                  << ", minOccupancy: " << minOccupancy << ", maxOccupancy: " << maxOccupancy << std::endl;
        std::cout << "meanOccupancy around z=0 (z>-100.mm&&z<100.mm) : " << meanOccupancyZ0 << std::endl;
        std::cout << "MeanNumber of hits from different tracks per Surface: " << meanNHits << std::endl;
        std::cout << "Max number of clusters per surface: " << nClustersPerSurfaceMax << std::endl;
        std::cout << "Merged Cluster Rate: " << meanMergedClusterRate << ", min: " << minMergedClusterRate
                  << ", max: " << maxMergedClusterRate << std::endl;
        std::cout << "**********************************************************************" << std::endl;

        globLid_endcap++;
      }
    }  // layers
  }    // volumes

  outputFile.cd();

  TDirectory* coDir = outputFile.mkdir("channelOC_barrel");
  coDir->cd();

  TGraphAsymmErrors* occupancies_barrel = new TGraphAsymmErrors(nLayers_barrel, rPos_barrel, occupany_barrel, 0, 0,
                                                                occupancyErrorL_barrel, occupancyErrorH_barrel);
  occupancies_barrel->SetTitle("channel occupancy barrel layers");
  occupancies_barrel->GetXaxis()->SetTitle("r[mm]");
  occupancies_barrel->GetYaxis()->SetTitle("#channel on/#channels per module");
  occupancies_barrel->Write();

  outputFile.cd();

  TDirectory* hpmDir_barrel = outputFile.mkdir("hpm_barrel");
  hpmDir_barrel->cd();

  TGraphAsymmErrors* nHits_barrel_g =
      new TGraphAsymmErrors(nLayers_barrel, rPos_barrel, nHits_barrel, 0, 0, nHitsErrorL_barrel, nHitsErrorH_barrel);
  nHits_barrel_g->SetTitle("#Hits per module");
  nHits_barrel_g->GetXaxis()->SetTitle("r[mm]");
  nHits_barrel_g->GetYaxis()->SetTitle("#hits per module");
  nHits_barrel_g->Write();

  outputFile.cd();

  TDirectory* hpmDir_endcap = outputFile.mkdir("hpm_endcap");
  hpmDir_endcap->cd();

  TGraphAsymmErrors* nHits_endcap_g =
      new TGraphAsymmErrors(nLayers_endcap, zPos_endcap, nHits_endcap, 0, 0, nHitsErrorL_endcap, nHitsErrorH_endcap);
  nHits_endcap_g->SetTitle("#Hits per module");
  nHits_endcap_g->GetXaxis()->SetTitle("r[mm]");
  nHits_endcap_g->GetYaxis()->SetTitle("#hits per module");
  nHits_endcap_g->Write();

  outputFile.cd();

  // float meanMergedClusters_barrel[nLayers_barrel];
  // float mergedClustersErrorL_barrel[nLayers_barrel];
  // float mergedClustersErrorH_barrel[nLayers_barrel];
  TGraphAsymmErrors* mergedClusters_g =
      new TGraphAsymmErrors(nLayers_barrel, rPos_barrel, meanMergedClusters_barrel, 0, 0, 0, 0);
  mergedClusters_g->SetTitle("Meged Cluster Rate");
  mergedClusters_g->GetXaxis()->SetTitle("r[mm]");
  mergedClusters_g->GetYaxis()->SetTitle("#clusters with more than one particle/#clusters per module");
  mergedClusters_g->Write();

  /*  TGraphAsymmErrors* occupancies_barrel = new TGraphAsymmErrors(
        nLayers_barrel, rPos_barrel, occupany_barrel, 0, 0, occupancyErrorL_barrel, occupancyErrorH_barrel);
    occupancies_barrel->SetTitle("Channel Occupancies in Barrel");
    occupancies_barrel->GetXaxis()->SetTitle("r[mm]");
    occupancies_barrel->GetYaxis()->SetTitle("#channel on/#channels per module");
    occupancies_barrel->Write();*/

  channelOccupancy_h->Write();
  channelOccupancy_h_barrel->Write();
  clusterSizesPhi_h->Write();
  clusterSizesLong_h->Write();
  nHitsPerCluster_h->Write();
  nHitsPerChannel_h->Write();
  mergedClusterRate_h->Write();
  mergedClusterRate_h_barrel->Write();

  inputFile.Close();
  outputFile.Close();
}
