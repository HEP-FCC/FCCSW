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

#include "averageOccupanciesHelper.hpp"

void averageOccupancies(std::string dirname = "/afs/cern.ch/user/j/jhrdinka/public/FCCSW/testRootScript/",
                        std::string outFile = "occupancies.root") {

  // --------------------------------------------------------
  // -------------Declare input file and branches------------
  // --------------------------------------------------------
  const char* ext = ".root";
  std::string treeName = "clusters";

  TSystemDirectory dir(dirname.data(), dirname.data());
  TList* files = dir.GetListOfFiles();

  double totalNumberOfModules = 49114;
  double frequency = 40e6;
  double factorToTByte = 1e-12;
  double bitToByte = 0.125;
  double dataRateFactor = (totalNumberOfModules * frequency * factorToTByte * bitToByte);

  // --------------------------------------------------------
  // ----------------Constants and definitions---------------
  // --------------------------------------------------------

  // masks for layers
  long long int mask = 0xf;
  long long int layerMaskBarrel = 0x1f0;
  long long int posNegMask = 0x10;
  long long int layerMaskEC = 0x3e0;

  // number of barrels
  const int nLayers_barrel = 12;
  const int nLayers_ec = 20;

  // number of events = number of files
  int nFiles = 0;

  std::vector<int> nModules_barrel = {280, 640, 1040, 1520, 578, 850, 1598, 2162, 2726, 3290, 3854, 4512};
  std::vector<int> nModules_ec = {108, 108, 108, 108, 108, 948, 948, 948, 948, 948,
                                  948, 388, 524, 684, 868, 868, 868, 868, 868, 868};

  float pitchSizes_barrel[12] = {0.00125, 0.00125, 0.00125, 0.00125, 0.01332, 0.01332,
                                 0.01332, 0.01332, 1.665,   1.665,   1.665,   1.665};

  float pitchSizes_barrelPhi[12] = {0.025,  0.025,  0.025,  0.025,  0.0333, 0.0333,
                                    0.0333, 0.0333, 0.0333, 0.0333, 0.0333, 0.0333};

  float pitchSizes_barrelZ[12] = {0.05, 0.05, 0.05, 0.05, 0.4, 0.4, 0.4, 0.4, 50, 50, 50, 50};

  // --------------------------------------------------------
  // ---------------------Plot defintions--------------------
  // --------------------------------------------------------

  // histograms
  TProfile2D* channelOccupancy_h =
      new TProfile2D("channelOccupancy_h", "#channels on/#channels per module", 1000, -16150., 16150., 69, 0., 1600.);
  channelOccupancy_h->GetXaxis()->SetTitle("z[mm]");
  channelOccupancy_h->GetYaxis()->SetTitle("r[mm]");

  TH2F* channelOccupancy_h_barrel =
      new TH2F("channelOccupancy_h_barrel", "#channel on/#channels per module in %", 69, 0., 1600., 100, 0., 25.);
  channelOccupancy_h->GetXaxis()->SetTitle("z[mm]");
  channelOccupancy_h->GetYaxis()->SetTitle("r[mm]");

  TProfile2D* mergedClusterRate_h =
      new TProfile2D("mergedClusterRate_h", "Merged Cluster Rate", 1000, -16150., 16150., 69, 0., 1600.);
  mergedClusterRate_h->GetXaxis()->SetTitle("z[mm]");
  mergedClusterRate_h->GetYaxis()->SetTitle("r[mm]");

  std::vector<LayerInformation> layerInfo_barrel_glob(nLayers_barrel);

  for (int i = 0; i < nLayers_barrel; i++) {
    std::string s_eta_name = "sizes0_eta_bl" + std::to_string(i);
    std::string s1_eta_name = "sizes1_eta_bl" + std::to_string(i);
    std::string s2_eta_name = "sizes_eta_bl" + std::to_string(i);

    std::string s_theta_name = "sizes0_theta_bl" + std::to_string(i);
    std::string s1_theta_name = "sizes1_theta_bl" + std::to_string(i);
    std::string s2_theta_name = "sizes_theta_bl" + std::to_string(i);

    std::string s_phi_name = "sizes0_phi_bl" + std::to_string(i);
    std::string s1_phi_name = "sizes1_phi_bl" + std::to_string(i);
    std::string s2_phi_name = "sizes_phi_bl" + std::to_string(i);

    auto lBarrel = &layerInfo_barrel_glob.at(i);

    // eta
    lBarrel->clusterSizes0_eta_h = new TProfile(s1_eta_name.c_str(), "cluster sizes in r/#phi", 100., -4.2, 4.2);
    lBarrel->clusterSizes0_eta_h->GetXaxis()->SetTitle("#eta");
    lBarrel->clusterSizes0_eta_h->GetYaxis()->SetTitle("cluster size l0");

    lBarrel->clusterSizes1_eta_h = new TProfile(s2_eta_name.c_str(), "cluster sizes in z", 100., -4.2, 4.2);
    lBarrel->clusterSizes1_eta_h->GetXaxis()->SetTitle("#eta");
    lBarrel->clusterSizes1_eta_h->GetYaxis()->SetTitle("cluster size l1");

    lBarrel->clusterSizes_eta_h = new TProfile(s_eta_name.c_str(), "cluster sizes", 100., -4.2, 4.2);
    lBarrel->clusterSizes_eta_h->GetXaxis()->SetTitle("#eta");
    lBarrel->clusterSizes_eta_h->GetYaxis()->SetTitle("cluster size");

    // theta
    lBarrel->clusterSizes0_theta_h = new TProfile(s1_theta_name.c_str(), "cluster sizes in r/#phi", 100., 0., 3.41);
    lBarrel->clusterSizes0_theta_h->GetXaxis()->SetTitle("#theta");
    lBarrel->clusterSizes0_theta_h->GetYaxis()->SetTitle("cluster size l0");

    lBarrel->clusterSizes1_theta_h = new TProfile(s2_theta_name.c_str(), "cluster sizes in z", 100., 0., 3.41);
    lBarrel->clusterSizes1_theta_h->GetXaxis()->SetTitle("#theta");
    lBarrel->clusterSizes1_theta_h->GetYaxis()->SetTitle("cluster size l1");

    lBarrel->clusterSizes_theta_h = new TProfile(s_theta_name.c_str(), "cluster sizes", 100., 0., 3.41);
    lBarrel->clusterSizes_theta_h->GetXaxis()->SetTitle("#theta");
    lBarrel->clusterSizes_theta_h->GetYaxis()->SetTitle("cluster size");

    // phi
    lBarrel->clusterSizes0_phi_h = new TProfile(s1_phi_name.c_str(), "cluster sizes in r/#phi", 100., -3.41, 3.41);
    lBarrel->clusterSizes0_phi_h->GetXaxis()->SetTitle("#phi");
    lBarrel->clusterSizes0_phi_h->GetYaxis()->SetTitle("cluster size l0");

    lBarrel->clusterSizes1_phi_h = new TProfile(s2_phi_name.c_str(), "cluster sizes in z", 100., -3.41, 3.41);
    lBarrel->clusterSizes1_phi_h->GetXaxis()->SetTitle("#phi");
    lBarrel->clusterSizes1_phi_h->GetYaxis()->SetTitle("cluster size l1");

    lBarrel->clusterSizes_phi_h = new TProfile(s_phi_name.c_str(), "cluster sizes", 100., -3.41, 3.41);
    lBarrel->clusterSizes_phi_h->GetXaxis()->SetTitle("#phi");
    lBarrel->clusterSizes_phi_h->GetYaxis()->SetTitle("cluster size");
  }

  // Event information
  EventInformation eventInfo;

  if (files) {
    TSystemFile* file;
    TString fname;
    TIter next(files);
    while ((file = (TSystemFile*)next())) {
      fname = dirname + file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(ext)) {
        // current file
        std::cout << "opening file: " << fname.Data() << std::endl;
        TFile inputFile(fname.Data());
        TTreeReader reader(treeName.c_str(), &inputFile);

        if (nFiles == 10) {
          std::cout << "100 files" << std::endl;
          break;
        }

        nFiles++;
        // number of events per file
        int maxEvent = -1;

        TTreeReaderValue<int> _eventNr(reader, "event_nr");
        TTreeReaderValue<long long int> _moduleID(reader, "moduleID");
        TTreeReaderValue<int> _nChannels(reader, "nChannels");
        TTreeReaderValue<int> _nChannelsOn(reader, "nChannelsOn");
        TTreeReaderValue<float> _sX(reader, "s_x");
        TTreeReaderValue<float> _sY(reader, "s_y");
        TTreeReaderValue<float> _sZ(reader, "s_z");
        TTreeReaderValue<std::vector<float>> _x(reader, "g_x");
        TTreeReaderValue<std::vector<float>> _y(reader, "g_y");
        TTreeReaderValue<std::vector<float>> _z(reader, "g_z");

        //**     TTreeReaderValue<std::vector<short int>> _nTracksPerCluster(reader, "tracksPerCluster");
        // for data rates
        //**   TTreeReaderValue<std::vector<short int>> _nCells(reader, "nCells");
        TTreeReaderValue<int> _nChannels_l0(reader, "nChannels_l0");
        TTreeReaderValue<int> _nChannels_l1(reader, "nChannels_l1");

        TTreeReaderValue<std::vector<short int>> _sizeX(reader, "size_x");
        TTreeReaderValue<std::vector<short int>> _sizeY(reader, "size_y");
        //  TTreeReaderValue<int> _energy(reader, "energy");

        utilities::setActsStyle();

        /// Layer information
        std::vector<LayerInformation> layerInfo_barrel(nLayers_barrel);
        std::vector<LayerInformation> layerInfo_ePos(nLayers_ec);
        std::vector<LayerInformation> layerInfo_eNeg(nLayers_ec);

        // --------------------------------------------------------
        // --------------------------READ--------------------------
        // --------------------------------------------------------

        while (reader.Next()) {

          TVector3 surfacePosition((*_sX), (*_sY), (*_sZ));
          //  if (std::fabs(surfacePosition.Eta()) > 1.5) continue;

          // module information
          // Fill module information

          if ((*_eventNr) > maxEvent) maxEvent = *_eventNr;
          // layer information
          // current system id
          long long int systemID = ((*_moduleID) & mask);
          if (systemID > 4) std::cout << "ERROR, systemID too high: " << systemID << std::endl;
          LayerInformation* layerInfo = nullptr;
          if ((systemID == 0) || (systemID == 1)) {
            //-------------------barrel-------------------
            // calculate global barrel layer ID
            long long int barrelLayerID = (((*_moduleID) & layerMaskBarrel) >> 4) + 6 * systemID;
            // create layer information
            layerInfo = &layerInfo_barrel.at(barrelLayerID);
          } else {

            //-------------------endcap-------------------
            // layer offset
            int layerOffset = 0;
            // pos endcap
            if (systemID == 3) {
              // outer
              layerOffset = 5;
            } else if (systemID == 4) {
              // fwd
              layerOffset = 11;
            }
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
          }
          float surfR = surfacePosition.Perp();
          // calculate occupancy
          float channelRatio = float(*_nChannelsOn) / float(*_nChannels);

          channelOccupancy_h->Fill((*_sZ), surfR, channelRatio * 100);
          channelOccupancy_h_barrel->Fill(surfR, channelRatio * 100);
          layerInfo->occupancy += channelRatio;
          if (channelRatio > layerInfo->occupancyMax) layerInfo->occupancyMax = channelRatio;
          if (channelRatio < layerInfo->occupancyMin) layerInfo->occupancyMin = channelRatio;

          layerInfo->dataRate += (float(*_nChannelsOn) * (std::log2(*_nChannels_l0) + std::log2(*_nChannels_l1)));

          //-------------------Module level-------------------
          // average module cluster size

          //   float pathLength_module = 0;
          //   float nTracks_module = 0;
          double mergedClusters_module = 0;
          // go through information per cluster
          for (size_t i = 0; i < (*_sizeX).size(); i++) {
            layerInfo->nClusters++;
            //-------------------Cluster level-------------------
            if ((systemID == 0) || (systemID == 1)) {
              long long int barrelLayerID = (((*_moduleID) & layerMaskBarrel) >> 4) + 6 * systemID;
              // cluster position
              TVector3 clusterPos((*_x).at(i), (*_y).at(i), (*_z).at(i));
              // eta
              layerInfo_barrel_glob.at(barrelLayerID).clusterSizes0_eta_h->Fill(clusterPos.Eta(), (*_sizeX).at(i));
              layerInfo_barrel_glob.at(barrelLayerID).clusterSizes1_eta_h->Fill(clusterPos.Eta(), (*_sizeY).at(i));
              //**   layerInfo_barrel_glob.at(barrelLayerID).clusterSizes_eta_h->Fill(clusterPos.Eta(),
              //(*_nCells).at(i));
              // theta
              layerInfo_barrel_glob.at(barrelLayerID).clusterSizes0_theta_h->Fill(clusterPos.Theta(), (*_sizeX).at(i));
              layerInfo_barrel_glob.at(barrelLayerID).clusterSizes1_theta_h->Fill(clusterPos.Theta(), (*_sizeY).at(i));
              //**    layerInfo_barrel_glob.at(barrelLayerID).clusterSizes_theta_h->Fill(clusterPos.Theta(),
              //(*_nCells).at(i));
              // phi
              layerInfo_barrel_glob.at(barrelLayerID).clusterSizes0_phi_h->Fill(clusterPos.Phi(), (*_sizeX).at(i));
              layerInfo_barrel_glob.at(barrelLayerID).clusterSizes1_phi_h->Fill(clusterPos.Phi(), (*_sizeY).at(i));
              //**    layerInfo_barrel_glob.at(barrelLayerID).clusterSizes_phi_h->Fill(clusterPos.Phi(),
              //(*_nCells).at(i));

              float x = float((*_sizeX).at(i)) * float(pitchSizes_barrelPhi[barrelLayerID]);
              float y = float((*_sizeY).at(i)) * float(pitchSizes_barrelZ[barrelLayerID]);
            }

            layerInfo->sizeX += float((*_sizeX).at(i));
            layerInfo->sizeY += float((*_sizeY).at(i));

            //**        if ((*_nTracksPerCluster).at(i) > 1) {
            // per module
            mergedClusters_module += 1;
            // per layer
            layerInfo->mergedClusterRate += 1;

            //              nTracks_module += (*_nTracksPerCluster).at(i);
            //**     }

            //**   layerInfo->nCells += float((*_nCells).at(i));
          }  // module
             // average path length per module
             // number of particles per module

          // average per surface - wrong
          /*    if ((*_sizeX).size()) {
                sizeX_module /= (*_sizeX).size();
                sizeY_module /= (*_sizeY).size();
                mergedClusters_module /= ((*_nTracksPerCluster).size());
                /// tododo  nCells_module /= (*_nCells).size();
              }*/

          layerInfo->nSurfaces += 1;

          // merged cluster rate

          // per module
          //**    if ((*_nTracksPerCluster).size()) {
          //**       mergedClusters_module /= ((*_nTracksPerCluster).size());
          //** }
          mergedClusterRate_h->Fill((*_sZ), surfR, float(mergedClusters_module) * 100.);

        }  // after reading in

        maxEvent += 1;
        if (maxEvent <= 0) return;
        if (maxEvent > 1) {
          std::cout << "Error, currently more than one event within the same file can not be averaged with this "
                       "script, please change script!"
                    << std::endl;
          return;
        }

        // --------------------------------------------------------
        // ---------------------Average barrel---------------------
        // --------------------------------------------------------

        ////   std::cout << "=============================Barrel=============================" << std::endl;
        size_t counter = 0;
        for (auto& layer : layerInfo_barrel) {
          // average over surfaces
          layer.occupancy /= nModules_barrel[counter];
          layer.dataRate /= nModules_barrel[counter];

          //  layer.occupancy /= layer.nSurfaces;
          //  layer.dataRate /= layer.nSurfaces;
          // over layer
          layer.sizeX /= layer.nClusters;
          layer.sizeY /= layer.nClusters;
          layer.nCells /= layer.nClusters;
          layer.mergedClusterRate /= layer.nClusters;

          /* layer.occupancy /= layer.nSurfaces;
           layer.sizeX /= layer.nSurfaces;
           layer.sizeY /= layer.nSurfaces;
           layer.nCells /= layer.nSurfaces;*/

          ////    std::cout << "occupancy for layer " << counter << " is " << layer.occupancy * 100
          /////               << ", average clusterSizes: " << layer.sizeX << "," << layer.sizeY << std::endl;

          // set event info to module averaged layer info of this event
          eventInfo.layerInfo_barrel.at(counter).add(layer);

          counter++;
        }

        // --------------------------------------------------------
        // -----------------------Average EC-----------------------
        // --------------------------------------------------------

        /////  std::cout << "=============================ECap=============================" << std::endl;
        for (size_t i = 0; i < nLayers_ec; i++) {
          // pos EC
          // average over surfaces
          layerInfo_ePos.at(i).occupancy /= nModules_ec[i];
          layerInfo_ePos.at(i).dataRate /= nModules_ec[i];
          //  layerInfo_ePos.at(i).occupancy /= layerInfo_ePos.at(i).nSurfaces;
          //  layerInfo_ePos.at(i).dataRate /= layerInfo_ePos.at(i).nSurfaces;

          layerInfo_ePos.at(i).sizeX /= layerInfo_ePos.at(i).nClusters;
          layerInfo_ePos.at(i).sizeY /= layerInfo_ePos.at(i).nClusters;
          layerInfo_ePos.at(i).nCells /= layerInfo_ePos.at(i).nClusters;
          layerInfo_ePos.at(i).mergedClusterRate /= layerInfo_ePos.at(i).nClusters;

          layerInfo_eNeg.at(i).occupancy /= nModules_ec[i];
          layerInfo_eNeg.at(i).dataRate /= nModules_ec[i];
          //  layerInfo_eNeg.at(i).occupancy /= layerInfo_ePos.at(i).nSurfaces;
          //   layerInfo_eNeg.at(i).dataRate /= layerInfo_ePos.at(i).nSurfaces;

          layerInfo_eNeg.at(i).sizeX /= layerInfo_eNeg.at(i).nClusters;
          layerInfo_eNeg.at(i).sizeY /= layerInfo_eNeg.at(i).nClusters;
          layerInfo_eNeg.at(i).nCells /= layerInfo_eNeg.at(i).nClusters;
          layerInfo_eNeg.at(i).mergedClusterRate /= layerInfo_eNeg.at(i).nClusters;

          /*
                layerInfo_ePos.at(i).occupancy /= layerInfo_ePos.at(i).nSurfaces;
                layerInfo_ePos.at(i).sizeX /= layerInfo_ePos.at(i).nSurfaces;
                layerInfo_ePos.at(i).sizeY /= layerInfo_ePos.at(i).nSurfaces;
                layerInfo_ePos.at(i).nCells /= layerInfo_ePos.at(i).nSurfaces;

                layerInfo_eNeg.at(i).occupancy /= layerInfo_eNeg.at(i).nSurfaces;
                layerInfo_eNeg.at(i).sizeX /= layerInfo_eNeg.at(i).nSurfaces;
                layerInfo_eNeg.at(i).sizeY /= layerInfo_eNeg.at(i).nSurfaces;
                layerInfo_eNeg.at(i).nCells /= layerInfo_eNeg.at(i).nSurfaces;
      */
          /*     std::cout << "occupancy for layer pos" << i << " is " << layerInfo_ePos.at(i).occupancy * 100
                         << ", average clusterSize: " << layerInfo_ePos.at(i).sizeX << "," << layerInfo_ePos.at(i).sizeY
                         << std::endl;
               std::cout << "occupancy for layer neg" << i << " is " << layerInfo_eNeg.at(i).occupancy * 100
                         << ", average clusterSize: " << layerInfo_eNeg.at(i).sizeX << "," << layerInfo_eNeg.at(i).sizeY
                         << std::endl;

               std::cout << "-----" << std::endl;*/

          // set event info to module averaged layer info of this event
          eventInfo.layerInfo_ePos.at(i).add(layerInfo_ePos.at(i));
          eventInfo.layerInfo_eNeg.at(i).add(layerInfo_eNeg.at(i));

          // neg EC
        }

        inputFile.Close();

      }  // check if root file
    }    // go through files
  }      // check if files are present in directory

  /// AVERAGE EVENTS

  std::cout << "Creating new output file: " << outFile << " and writing out histograms. " << std::endl;
  TFile outputFile(outFile.c_str(), "recreate");

  channelOccupancy_h->Write();
  channelOccupancy_h_barrel->Write();
  mergedClusterRate_h->Write();

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
  float currentSizeX = 0;
  float currentSizeXStdDev = 0;
  float currentSizeY = 0;
  float currentSizeYStdDev = 0;
  float currentnCells = 0;
  float currentMergedClusterRate = 0;
  float currentDataRate = 0;

  tree_barrel->Branch("occupancy", &currentOccupancy);
  tree_barrel->Branch("occupancyDev", &currentOccStdDev);
  tree_barrel->Branch("sizeX", &currentSizeX);
  tree_barrel->Branch("sizeXDev", &currentSizeXStdDev);
  tree_barrel->Branch("sizeY", &currentSizeY);
  tree_barrel->Branch("sizeYDev", &currentSizeYStdDev);
  tree_barrel->Branch("nCells", &currentnCells);
  tree_barrel->Branch("mergedClusterRate", &currentMergedClusterRate);
  tree_barrel->Branch("dataRate", &currentDataRate);

  tree_ePos->Branch("occupancy", &currentOccupancy);
  tree_ePos->Branch("occupancyDev", &currentOccStdDev);
  tree_ePos->Branch("sizeX", &currentSizeX);
  tree_ePos->Branch("sizeXDev", &currentSizeXStdDev);
  tree_ePos->Branch("sizeY", &currentSizeY);
  tree_ePos->Branch("sizeYDev", &currentSizeYStdDev);
  tree_ePos->Branch("nCells", &currentnCells);
  tree_ePos->Branch("mergedClusterRate", &currentMergedClusterRate);
  tree_ePos->Branch("dataRate", &currentDataRate);

  tree_eNeg->Branch("occupancy", &currentOccupancy);
  tree_eNeg->Branch("occupancyDev", &currentOccStdDev);
  tree_eNeg->Branch("sizeX", &currentSizeX);
  tree_eNeg->Branch("sizeXDev", &currentSizeXStdDev);
  tree_eNeg->Branch("sizeY", &currentSizeY);
  tree_eNeg->Branch("sizeYDev", &currentSizeYStdDev);
  tree_eNeg->Branch("nCells", &currentnCells);
  tree_eNeg->Branch("mergedClusterRate", &currentMergedClusterRate);
  tree_eNeg->Branch("dataRate", &currentDataRate);

  float radii_barrel[nLayers_barrel] = {25,      60,      100,     150,      270,      400,
                                        521.016, 742.396, 937.244, 1132.012, 1326.735, 1516.536};

  float occupancy_barrel[nLayers_barrel];
  float occupancyErrorL_barrel[nLayers_barrel];
  float occupancyErrorH_barrel[nLayers_barrel];
  std::cout << "================================================================" << std::endl;
  std::cout << "=============================Event==============================" << std::endl;
  std::cout << "================================================================" << std::endl;

  std::cout << "++++++++++++++++Barrel" << std::endl;
  size_t counter = 0;
  for (auto& layer : eventInfo.layerInfo_barrel) {
    // average over event
    layer.occupancy /= nFiles;
    layer.sizeX /= nFiles;
    layer.sizeY /= nFiles;
    layer.nCells /= nFiles;
    layer.mergedClusterRate /= nFiles;
    layer.dataRate /= nFiles;

    // calculate standard deviations
    float stdOccupancy = 0;
    float stdSizesX = 0;
    float stdSizesY = 0;
    layer.standardDev(stdOccupancy, stdSizesX, stdSizesY);

    currentOccupancy = (layer.occupancy * 100.);
    currentOccStdDev = (stdOccupancy * 100);
    currentSizeX = layer.sizeX;
    currentSizeXStdDev = stdSizesX;
    currentSizeY = layer.sizeY;
    currentSizeYStdDev = stdSizesY;
    currentnCells = layer.nCells;
    currentMergedClusterRate = layer.mergedClusterRate * 100;
    currentDataRate = (layer.dataRate * dataRateFactor);

    std::cout << "(Event)occupancy for layer Event " << counter << " is " << currentOccupancy << " +- "
              << currentOccStdDev << ", average clusterSizes: " << layer.sizeX << "," << layer.sizeY << std::endl;

    // set occupancy
    occupancy_barrel[counter] = currentOccupancy;
    occupancyErrorL_barrel[counter] = layer.occupancyMin * 100.;
    occupancyErrorH_barrel[counter] = layer.occupancyMax * 100.;

    tree_barrel->Fill();

    counter++;
  }

  int bLayerCounter = 0;
  for (auto layer : layerInfo_barrel_glob) {
    std::string layerDirName = "dir_bl" + std::to_string(bLayerCounter);

    bLayerCounter++;
    TDirectory* dir = outputFile.mkdir(layerDirName.c_str());
    dir->cd();
    layer.clusterSizes0_eta_h->Write();
    layer.clusterSizes1_eta_h->Write();
    layer.clusterSizes_eta_h->Write();

    layer.clusterSizes0_theta_h->Write();
    layer.clusterSizes1_theta_h->Write();
    layer.clusterSizes_theta_h->Write();

    layer.clusterSizes0_phi_h->Write();
    layer.clusterSizes1_phi_h->Write();
    layer.clusterSizes_phi_h->Write();
  }

  outputFile.cd();
  // --------------------------------------------------------
  // ----------------------Plots barrel----------------------
  // --------------------------------------------------------

  TGraphAsymmErrors* occupancies_barrel = new TGraphAsymmErrors(
      nLayers_barrel, radii_barrel, occupancy_barrel, 0, 0, occupancyErrorL_barrel, occupancyErrorH_barrel);
  occupancies_barrel->SetTitle("channel occupancy barrel layers");
  occupancies_barrel->GetXaxis()->SetTitle("r[mm]");
  occupancies_barrel->GetYaxis()->SetTitle("#channel on/#channels per module");
  occupancies_barrel->Write();

  // --------------------------------------------------------
  // ----------------------Plots Endcap----------------------
  // --------------------------------------------------------

  std::cout << "++++++++++++++++ECap" << std::endl;
  for (size_t i = 0; i < nLayers_ec; i++) {
    // pos EC
    // average over events
    eventInfo.layerInfo_ePos.at(i).occupancy /= nFiles;
    eventInfo.layerInfo_ePos.at(i).sizeX /= nFiles;
    eventInfo.layerInfo_ePos.at(i).sizeY /= nFiles;
    eventInfo.layerInfo_ePos.at(i).nCells /= nFiles;
    eventInfo.layerInfo_ePos.at(i).mergedClusterRate /= nFiles;
    eventInfo.layerInfo_ePos.at(i).dataRate /= nFiles;

    // calculate standard deviations
    float stdOccupancy = 0;
    float stdSizesX = 0;
    float stdSizesY = 0;
    eventInfo.layerInfo_ePos.at(i).standardDev(stdOccupancy, stdSizesX, stdSizesY);

    currentOccupancy = (eventInfo.layerInfo_ePos.at(i).occupancy * 100);
    currentOccStdDev = (stdOccupancy * 100);

    currentSizeX = eventInfo.layerInfo_ePos.at(i).sizeX;
    currentSizeXStdDev = stdSizesX;
    currentSizeY = eventInfo.layerInfo_ePos.at(i).sizeY;
    currentSizeYStdDev = stdSizesY;
    currentnCells = eventInfo.layerInfo_ePos.at(i).nCells;
    currentMergedClusterRate = eventInfo.layerInfo_ePos.at(i).mergedClusterRate * 100;
    currentDataRate = (eventInfo.layerInfo_ePos.at(i).dataRate * dataRateFactor);

    std::cout << "(Event)occupancy for layer pos" << i << " is " << currentOccupancy << " +- " << currentOccStdDev
              << ", average clusterSize: " << eventInfo.layerInfo_ePos.at(i).sizeX << ","
              << eventInfo.layerInfo_ePos.at(i).sizeY << std::endl;

    tree_ePos->Fill();

    eventInfo.layerInfo_eNeg.at(i).occupancy /= nFiles;
    eventInfo.layerInfo_eNeg.at(i).sizeX /= nFiles;
    eventInfo.layerInfo_eNeg.at(i).sizeY /= nFiles;
    eventInfo.layerInfo_eNeg.at(i).nCells /= nFiles;
    eventInfo.layerInfo_eNeg.at(i).mergedClusterRate /= nFiles;
    eventInfo.layerInfo_eNeg.at(i).dataRate /= nFiles;
    eventInfo.layerInfo_eNeg.at(i).standardDev(stdOccupancy, stdSizesX, stdSizesY);

    currentOccupancy = (eventInfo.layerInfo_eNeg.at(i).occupancy * 100);
    currentOccStdDev = (stdOccupancy * 100);

    currentSizeX = eventInfo.layerInfo_eNeg.at(i).sizeX;
    currentSizeXStdDev = stdSizesX;
    currentSizeY = eventInfo.layerInfo_eNeg.at(i).sizeY;
    currentSizeYStdDev = stdSizesY;
    currentnCells = eventInfo.layerInfo_eNeg.at(i).nCells;
    currentMergedClusterRate = eventInfo.layerInfo_eNeg.at(i).mergedClusterRate * 100;
    currentDataRate = (eventInfo.layerInfo_eNeg.at(i).dataRate * dataRateFactor);

    std::cout << "(Event)occupancy for layer neg" << i << " is " << currentOccupancy << " +- " << currentOccStdDev
              << ", average clusterSize: " << eventInfo.layerInfo_eNeg.at(i).sizeX << ","
              << eventInfo.layerInfo_eNeg.at(i).sizeY << std::endl;

    tree_eNeg->Fill();

    std::cout << "-----" << std::endl;

    // neg EC
  }

  tree_barrel->Write();
  tree_ePos->Write();
  tree_eNeg->Write();

  outputFile.Close();
}

/* ModuleInformation& moduleHelper = moduleInfo.at(reader.GetCurrentEntry());
     //  if ((*_nChannels) == 0) throw std::runtime_error("Number of channels == 0");
     if ((*_nChannelsOn) > (*_nChannels)) throw std::runtime_error("Number of activated channels > number of channels");
     // calculate occupancy average
     moduleHelper.occupancy = ((*_nChannels) == 0)
         ? moduleHelper.occupancy
         : (moduleHelper.occupancy + float(*_nChannelsOn) / float(*_nChannels));
     // calculated occupancy min and max
     if (moduleHelper.occupancy > moduleHelper.occupancyMax) moduleHelper.occupancyMax = moduleHelper.occupancy;
     if (moduleHelper.occupancy < moduleHelper.occupancyMin) moduleHelper.occupancyMin = moduleHelper.occupancy;



     moduleInfo.push_back(moduleHelper);
 */

/* for (auto& module : moduleInfo) {
   // average first
   module.occupancy /= maxEvent;

   // current system id
   long long unsigned systemID = module.moduleID & mask;
   // check where we are
   if ((systemID == 0) || (systemID == 1)) {
     // barrel
     long long unsigned barrelLayerID = ((module.moduleID & layerMaskBarrel) >> 4) + 6 * systemID;
   } else {
     // endcap
   }
 }*/
