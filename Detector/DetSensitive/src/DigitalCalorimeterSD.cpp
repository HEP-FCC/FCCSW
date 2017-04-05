#include "DetSensitive/DigitalCalorimeterSD.h"

// FCCSW
//#include "DetSensitive/SegmentationHelper.h"
#include "DDSegmentation/BitField64.h"
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
//#include "G4SystemOfUnits.hh"

#include "iostream"
#include "map"
#include "unordered_map"

namespace det {
DigitalCalorimeterSD::DigitalCalorimeterSD(const std::string& aDetectorName,
  const std::string& aReadoutName,
  const DD4hep::Geometry::Segmentation& aSeg)
  : G4VSensitiveDetector(aDetectorName), m_seg(aSeg), m_calorimeterCollection(nullptr) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);

  m_headerPrinted = false;

  m_pixelsOverThresholdFileName = aReadoutName+"_pixelsOverThreshold.txt";
  m_pixelsOverThresholdPerLayerFileName = aReadoutName+"_pixelsOverThresholdPerLayer.txt";
  m_mipsPerPixelFileName = aReadoutName+"_mipsPerPixel.txt";
  m_padMultiplicityFileName = aReadoutName+"_padMultiplicity.txt";

  //m_rootFile = new TFile("test.root");
  //m_padMultiplicity = new TH1F("padMultiplicity", "Number of 5x5mm2 pads fired per event", 10000,0,10000);
}

DigitalCalorimeterSD::~DigitalCalorimeterSD(){
  m_pixelsOverThresholdFile.close();
  m_padMultiplicityFile.close();
  //m_rootFile->Close();
  std::cout << std::endl;
}

void DigitalCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection = new G4THitsCollection
    <DD4hep::Simulation::Geant4CalorimeterHit>(SensitiveDetectorName,collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection), m_calorimeterCollection);

  //this collection is just a temp and not added to the HitsCollection just used to store raw hits before summing together
  m_tempCollection = new G4THitsCollection
    <DD4hep::Simulation::Geant4CalorimeterHit>(SensitiveDetectorName,"temp");

  // m_pixelsOverThresholdFile.open("pixelsOverThreshold.txt", std::ios_base::app);.

  if(!m_headerPrinted){
    std::cout << "incident particles\tunique cellIDs\tcells over thresh\tunique strixels\tstrixel pixels (3)\tstrixel pixels (7)\tstrixel pixels (15)\tstrixel pixels (31)" << std::endl;
    m_headerPrinted=true;
  }
  m_incidentParticles = 0;

  m_TrackIDsPerEvent.clear();

}

bool DigitalCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.)
    return false;

      CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
    CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();

  if(aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary){
    m_incidentParticles++;

    G4int trackID = aStep->GetTrack()->GetTrackID();
    std::map<int, int>::iterator trackID_it = m_TrackIDsPerEvent.find(trackID);
    if(m_TrackIDsPerEvent.find(trackID) != m_TrackIDsPerEvent.end()) {
      (*trackID_it).second++;
    } else {
      m_TrackIDsPerEvent[trackID] = 1;
    }
  

    // as in DD4hep::Simulation::Geant4GenericSD<Calorimeter>
    double delta_r = sqrt(pow(postPos.x(),2) + pow(postPos.y(),2)) - sqrt(pow(prePos.x(),2) + pow(prePos.y(),2));
    if(delta_r<0) {
      std::map<int, int>::iterator trackID_it = m_TrackIDsPerEvent.find(trackID);
      (*trackID_it).second = -1e6;
      //std::cout << "BACKWARDS STEP" << std::endl;
    }
  }

  DD4hep::Simulation::Position pos(prePos.x(), prePos.y(), prePos.z());
  auto hit = new DD4hep::Simulation::Geant4CalorimeterHit(pos);
  //hit->cellID  = segmentation::cellID(m_seg, *aStep);
  hit->cellID  = utils::cellID(m_seg, *aStep);
  hit->energyDeposit = edep;
  m_tempCollection->insert(hit);
  return true;
}

bool DigitalCalorimeterSD::IsAllowedCellID(unsigned long cid) {
  // check if the cellID occurred in the previous event
  // if the counter is zero the cell is allowed and can remove cell from the mask
  std::map<uint64_t, int>::iterator valid_it = m_CellIDsMaskedFromPreviousEvent.find(cid);
  if(m_CellIDsMaskedFromPreviousEvent.find(cid) != m_CellIDsMaskedFromPreviousEvent.end()) {
    if((*valid_it).second > 0) {
      //std::cout << "CellID " << cid << " is inactive for the next " << (*valid_it).second << " events" << std::endl;
      return false;
    } else {
      m_CellIDsMaskedFromPreviousEvent.erase(valid_it);
      return true;
    }
  } else {
    return true;
  }  
}

void DigitalCalorimeterSD::AddCellIDMask(unsigned long cid) {
  // look for the cellID in the std::map. If it exists then reset the dead time
  // if it does not exist then add it to the map
  int nEventsMasked = 40;
  std::map<uint64_t, int>::iterator add_it = m_CellIDsMaskedFromPreviousEvent.find(cid);
  if(m_CellIDsMaskedFromPreviousEvent.find(cid) != m_CellIDsMaskedFromPreviousEvent.end()) {
    (*add_it).second = nEventsMasked;
  } else {
    m_CellIDsMaskedFromPreviousEvent[cid] = nEventsMasked;
  }
}

void DigitalCalorimeterSD::UpdateCellIDMask() {
  // loop through the mask and remove one from each mask ID.
  // if it reaches zero then remove from the list.
  std::map<uint64_t, int>::iterator update_it;
  for (update_it = m_CellIDsMaskedFromPreviousEvent.begin(); update_it != m_CellIDsMaskedFromPreviousEvent.end(); update_it++){
    (*update_it).second -= 1;
  } 
  for (update_it = m_CellIDsMaskedFromPreviousEvent.begin(); update_it != m_CellIDsMaskedFromPreviousEvent.end(); update_it++){
    if((*update_it).second == 0) m_CellIDsMaskedFromPreviousEvent.erase(update_it);
  }
  std::cout << "There are currently " << m_CellIDsMaskedFromPreviousEvent.size() << " pixels masked out" << std::endl;
}



void DigitalCalorimeterSD::EndOfEvent(G4HCofThisEvent*) {

  int nHits = m_tempCollection->entries(); 
  
 // std::cout << "Total Number of hits = " << nHits << std::endl;
  std::cout << "Total number of TrackIDs in SD = " << m_TrackIDsPerEvent.size() << std::endl;
  std::map<int, int>::iterator trackID_it;
  int fwd_tracks = 0;
  for(trackID_it=m_TrackIDsPerEvent.begin(); trackID_it!=m_TrackIDsPerEvent.end(); trackID_it++){
    if((*trackID_it).second>0) {
      fwd_tracks++;
    }
  }
  std::cout << "Total number of only forward track IDs in SD = " << fwd_tracks << std::endl;

  // the key here will be the cellID (unique to each layer and cell depending on segmentation)
  // second is a pair of number particles and total energy
  std::map<uint64_t, std::pair<int, DD4hep::Simulation::Geant4CalorimeterHit*> > counter;
  DD4hep::Simulation::Geant4CalorimeterHit* hit = nullptr;

  std::cout << "####" <<  m_seg->type() << std::endl;
  auto decoderBarrel = m_seg->decoder();
  //std::cout << "Barrel segmentation of type " << m_seg->type() << std::endl;
  for(int i=0; i<nHits; i++) {
     hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*> (m_tempCollection->GetHit(i));

    uint64_t cellID = hit->cellID;

    //if(IsAllowedCellID(cellID)){  
    if(cellID==cellID){  
      // this map stored the energy per cell
      std::map<uint64_t, std::pair<int, DD4hep::Simulation::Geant4CalorimeterHit*> >::iterator it = counter.find(cellID);

      if(counter.find(cellID) != counter.end()) {
        (*it).second.first++;
        (*it).second.second->energyDeposit += hit->energyDeposit;
      } else {
        std::pair<int, DD4hep::Simulation::Geant4CalorimeterHit*> info = std::make_pair(1, hit);
        counter[cellID] = info;
      }
    }
  }

  // now loop through all the hits and check the Mask. Cannot do it before as may cut out multiple hits to cells
  for(int i=0; i<nHits; i++) {
    hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*> (m_tempCollection->GetHit(i));
    uint64_t cellID = hit->cellID;  
    AddCellIDMask(cellID);
  }

  double threshold = 480*0.0000036; // 350 electrons in MeV which should be default value
  int pixelsOverThreshold = 0;
  int pixelsOverThresholdPerLayer[50];
  std::vector<int> mipsPerPixel[50];
  for(int n(0); n<50; ++n){
    pixelsOverThresholdPerLayer[n]=0;
    mipsPerPixel[n].clear();
  }

  DD4hep::Simulation::Geant4CalorimeterHit* digi = nullptr;
  std::map<uint64_t, std::pair<int, DD4hep::Simulation::Geant4CalorimeterHit*> >::iterator it;
  std::map<Strixel, int> StrixelHitCounter;
  for(it=counter.begin(); it!=counter.end(); it++)
	{


    // if the energy deposited in the cellID is gtreather than threshold then do something
    if((*it).second.second->energyDeposit>threshold) {
      
      pixelsOverThreshold++;

      // insert the collection to G4 so can be passed on later
      digi = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*> ((*it).second.second);
      //digi->energyDeposit = (*it).second.second->energyDeposit; //(*it).second.first; // overwrite the energy deposit with the number of mips in the cell
      m_calorimeterCollection->insert(digi); // this passes to ROOT the readout... interesting!
      

      uint64_t cellID = (*it).first;

      decoderBarrel->setValue(cellID);
      int layer_id = (*decoderBarrel)["layer"];

      if(m_seg->type() == "CartesianGridXYZ") {
        
        int px = (*decoderBarrel)["x"];
        int py = (*decoderBarrel)["y"];
        int pz = (*decoderBarrel)["z"];

        // convert pixel to strixel
        Strixel strixel;
        strixel.layer = layer_id;
        strixel.x = px;
        strixel.y = py;
        strixel.z = floor(pz/100); //group in z the pixels into a strixel

        std::map<Strixel, int>::iterator strixel_it = StrixelHitCounter.find(strixel);
        if(StrixelHitCounter.find(strixel) != StrixelHitCounter.end()) {
          (*strixel_it).second++;
        } else {
          StrixelHitCounter[strixel] = 1;
        }
      }

      
//      std::cout << "layer=" << layer_id
//        << "\npx=" << px << ", py=" << py << ", pz=" << pz 
//        << "\nsx=" << strixel.x << ", sy=" << strixel.y << ", sz=" << strixel.z
//        << "\nc=" << pos_round_circ << ", pad.r=" << pad.r << ", pad.z=" << pad.z
//        << std::endl << std::endl;

      pixelsOverThresholdPerLayer[layer_id-1]++;
      mipsPerPixel[layer_id-1].push_back((*it).second.first);

    }
  }

  UpdateCellIDMask();

  int strixelHits = StrixelHitCounter.size();
  int strixelPixelHits_3 = 0;
  int strixelPixelHits_7 = 0;  
  int strixelPixelHits_15 = 0;
  int strixelPixelHits_31 = 0;
  //int strixelMaxPixels = 3;

  std::map<Pad, std::vector<int> > PadHitCounter; 
  std::map<Strixel, int>::iterator it_strixel;
  for(it_strixel=StrixelHitCounter.begin(); it_strixel!=StrixelHitCounter.end(); ++it_strixel) {

    // convert strixel to pad
    double x1 = (*it_strixel).first.x;
    double y1 = (*it_strixel).first.y;
    double r1 = pow((x1*x1+y1*y1),0.5);

    double x0 = r1;
    double y0 = 0.0; // set r0 to be at y=0 x=radius
    double cord = pow(pow((x1-x0),2)+pow((y1-y0),2),0.5);
    double theta = 2*asin(cord/(2*r1));
    double pos_round_circ = theta*r1;

    // for pad counting
    std::vector<int> counter;
    counter.resize(6);
    counter.at(0) = 1;

    int npixels = (*it_strixel).second;

    if(npixels<3) {
      strixelPixelHits_3 += npixels;
      counter.at(1) = npixels;
    }
    else {
      strixelPixelHits_3 += 3;
      counter.at(1) = 3;
    }

    if(npixels<7) {
      strixelPixelHits_7 += npixels;
      counter.at(2) = npixels;
    }
    else { 
      strixelPixelHits_7 += 7;
      counter.at(2) = 7;
    }
    if(npixels<15) {
      strixelPixelHits_15 += npixels;
      counter.at(3) = npixels;
    }
    else { 
      strixelPixelHits_15 += 15; 
      counter.at(3) = 15;
    }  

    if(npixels<31) {
      strixelPixelHits_31 += npixels;
      counter.at(4) = npixels;
    }
    else  {
      strixelPixelHits_31 += 31;  
      counter.at(4) = 31;
    }

    counter.at(5) = npixels;
    
    //------------------------------------------------- 
    Pad pad;
    pad.layer = (*it_strixel).first.layer;
    pad.r = floor(pos_round_circ/5.0);
    pad.z = (*it_strixel).first.z;

    std::map<Pad, std::vector<int> >::iterator it_pad = PadHitCounter.find(pad);
    if( PadHitCounter.find(pad) != PadHitCounter.end() ) {
      // add to the relevant int part
      (*it_pad).second.at(0) += counter.at(0);  // count hits in pad limited by 1 hit per column
      (*it_pad).second.at(1) += counter.at(1);  // count hits in pad limited by 3 hits per column
      (*it_pad).second.at(2) += counter.at(2);  // count hits in pad limited by 7 hits per column
      (*it_pad).second.at(3) += counter.at(3);  // count hits in pad limited by 15 hits per column
      (*it_pad).second.at(4) += counter.at(4);  // count hits in pad limited by 31 hits per column
      (*it_pad).second.at(5) += counter.at(5);  // count all hits in pad
    } else {
      PadHitCounter[pad] = counter;
    }
   
  }
  
  // now we loop through the map of pads to get total hits
  if(!m_padMultiplicityFile.is_open()) {
    std::map<Pad, std::vector<int> >::iterator it_pad;  
    m_padMultiplicityFile.open(m_padMultiplicityFileName, std::ios_base::app);
    for(it_pad=PadHitCounter.begin(); it_pad!=PadHitCounter.end(); ++it_pad) {
      
      //std::cout << (*it_pad).second.at(1) << " ";
      m_padMultiplicityFile << (*it_pad).second.at(1) << " ";

    }
    m_padMultiplicityFile << "\n";
    m_padMultiplicityFile.close();
  }

/*  std::cout << std::endl;
  std::cout << nHits << " " << pixelsOverThreshold << std::endl;
  std::cout << "Total number of unique cellIDs = " << pixelsOverThreshold << std::endl;
  std::cout << "Total number of unique strixels = " << strixelHits << std::endl;
  std::cout << "Total number of strixel pixels (3) = " << strixelPixelHits_3 << std::endl ;
  std::cout << "Total number of strixel pixels (7) = " << strixelPixelHits_7 << std::endl;
  std::cout << "Total number of strixel pixels (15) = " << strixelPixelHits_15 << std::endl;
  std::cout << "Total number of strixel pixels (31) = " << strixelPixelHits_31 << std::endl;
  std::cout << "Total number of unique pads = " << PadHitCounter.size() << std::endl << std::endl;
*/

  std::cout << m_incidentParticles << "\t" << counter.size() << "\t" << pixelsOverThreshold << "\t" << strixelHits << "\t" << strixelPixelHits_3 << "\t" << strixelPixelHits_7 << "\t" << strixelPixelHits_15 << "\t" << strixelPixelHits_31 << "\t" << PadHitCounter.size() << std::endl;


/* 
  
  if(!m_pixelsOverThresholdFile.is_open()){
    m_pixelsOverThresholdFile.open(m_pixelsOverThresholdFileName, std::ios_base::app);
    m_pixelsOverThresholdFile << pixelsOverThreshold << " ";
    m_pixelsOverThresholdFile.close();
  }

  if(!m_pixelsOverThresholdPerLayerFile.is_open()){
    m_pixelsOverThresholdPerLayerFile.open(m_pixelsOverThresholdPerLayerFileName, std::ios_base::app);
    for(int l(0); l<50; l++){
      m_pixelsOverThresholdPerLayerFile << pixelsOverThresholdPerLayer[l] << " ";
    }
    m_pixelsOverThresholdPerLayerFile << "\n";
    m_pixelsOverThresholdPerLayerFile.close();
  }

  if(!m_mipsPerPixelFile.is_open()){
    m_mipsPerPixelFile.open(m_mipsPerPixelFileName, std::ios_base::app);
    for(int l(0); l<50; l++){
      m_mipsPerPixelFile << "L" << l << " ";  
      for(unsigned p(0); p<mipsPerPixel[l].size(); p++){
        m_mipsPerPixelFile << mipsPerPixel[l][p] << " ";
      }
      m_mipsPerPixelFile << "\n";
    }
    m_mipsPerPixelFile.close();
  }  
*/
  

 // std::cout << pixelsOverThreshold << std::endl;
 //  for(int n(0); n<35; ++n){
 //   std::cout << pixelsOverThresholdPerLayer[n] << " ";}
 // std::cout << std::endl;

}


}

