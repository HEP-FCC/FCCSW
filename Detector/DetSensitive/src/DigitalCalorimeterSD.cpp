#include "DetSensitive/DigitalCalorimeterSD.h"

// FCCSW
#include "DDSegmentation/BitField64.h"
#include "DetCommon/DetUtils.h"
#include "DDSegmentation/Segmentation.h"
#include "DDSegmentation/CartesianGridXYZ.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"


// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#include "iostream"
#include "map"
#include "unordered_map"

namespace det {
DigitalCalorimeterSD::DigitalCalorimeterSD(const std::string& aDetectorName,
  const std::string& aReadoutName,
  const dd4hep::Segmentation& aSeg)
  : G4VSensitiveDetector(aDetectorName), m_seg(aSeg), m_calorimeterCollection(nullptr) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);

  m_headerPrinted = false;
  m_countParticlesPerPixel = true;

}

DigitalCalorimeterSD::~DigitalCalorimeterSD(){
}

void DigitalCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection = new G4THitsCollection
    <dd4hep::sim::Geant4CalorimeterHit>(SensitiveDetectorName,collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection), m_calorimeterCollection);

  //this collection is just a temp and not added to the HitsCollection just used to store raw hits before summing together
  m_tempCollection = new G4THitsCollection
    <dd4hep::sim::Geant4CalorimeterHit>(SensitiveDetectorName,"temp");

  // m_pixelsOverThresholdFile.open("pixelsOverThreshold.txt", std::ios_base::app);.

  if(!m_headerPrinted){
    if(m_countParticlesPerPixel) {
        std::cout << "\n\n\n###############################" << std::endl;
        std::cout << "#\n#" << std::endl;
        std::cout << "##  Counting Pixels in DECAL Mode " << std::endl;
        std::cout << "#\n#\n###############################" << std::endl;
      }

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


  bool isIncident = false;

  // count up the number of particles which enter the epitaxial layer
  if(aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary){
    m_incidentParticles++;
    isIncident = true;

    G4int trackID = aStep->GetTrack()->GetTrackID();
    std::map<int, int>::iterator trackID_it = m_TrackIDsPerEvent.find(trackID);
    if(m_TrackIDsPerEvent.find(trackID) != m_TrackIDsPerEvent.end()) {
      (*trackID_it).second++;
    } else {
      m_TrackIDsPerEvent[trackID] = 1;
    }  
  }

  dd4hep::Position pos(prePos.x(), prePos.y(), prePos.z());
  auto hit = new dd4hep::sim::Geant4CalorimeterHit(pos);
  //hit->cellID  = segmentation::cellID(m_seg, *aStep);
  hit->cellID  = utils::cellID(m_seg, *aStep);

  // if we have a step which is not incident make energy negative
  // this allows me to count incident particles later on
  if(!isIncident) {
    edep *= -1;
  }
  hit->energyDeposit = edep;
  m_tempCollection->insert(hit);

  return true;
}



void DigitalCalorimeterSD::EndOfEvent(G4HCofThisEvent*) {

  int nHits = m_tempCollection->entries(); 
  
  std::cout << "Total number of TrackIDs in SD = " << m_TrackIDsPerEvent.size() << std::endl;

  // this map stored the energy per cell
  // map.first = cellID
  // map.second.first = number of particles into the cell
  // map.second.second = total energy deposited in the cell
  std::map<uint64_t, std::pair<int, dd4hep::sim::Geant4CalorimeterHit*> > counter;
  dd4hep::sim::Geant4CalorimeterHit* hit = nullptr;

  //std::cout << "####" <<  m_seg->type() << std::endl;

  // loop through all of the hits in an event and sum the energy deposited per cell
  for(int i=0; i<nHits; i++) {
    hit = dynamic_cast<dd4hep::sim::Geant4CalorimeterHit*> (m_tempCollection->GetHit(i));

    // this bit of code allows us to check that is incident and that we should count as new particles
    // we then make the value of energy +ve again for ease of adding
    bool isIncident = true;
    if(hit->energyDeposit < 0.0) {
      isIncident = false;
      hit->energyDeposit *= -1;
      //std::cout << "NON INCIDENT STEP" << std::endl;
    }

    // ensure that there are no longer energy less than zero
    if(hit->energyDeposit < 0.0) { 
      std::cout << "STILL A HIT WITH NEGATIVE EDEP MORON!" << std::endl;
    }

    uint64_t cellID = hit->cellID;

    // for the DEcal chip being designed we have a dead time. This bool ensures the pixel is not dead
    // if useMask=false then IsAllowedCellID always returns true
    if(IsAllowedCellID(cellID)){  

      std::map<uint64_t, std::pair<int, dd4hep::sim::Geant4CalorimeterHit*> >::iterator it = counter.find(cellID);

      if(counter.find(cellID) != counter.end()) {
        
        // incrimnet the number of incident particles
        if(isIncident) {
          (*it).second.first++;
        }
        (*it).second.second->energyDeposit += hit->energyDeposit;
      } else {
        // this ensures that we only count incident particles
        // if not incident set to zero so we do count incident later on but do not double count
        int nparts = 0;
        if(isIncident) {
          nparts = 1;
        }
        std::pair<int, dd4hep::sim::Geant4CalorimeterHit*> info = std::make_pair(nparts, hit);
        counter[cellID] = info;
      }
    }
  }

  // now loop through all the hits and check the Mask. Cannot do it before as may cut out multiple hits to cells
  bool useMask = false;
  for(int i=0; i<nHits && useMask; i++) {
    hit = dynamic_cast<dd4hep::sim::Geant4CalorimeterHit*> (m_tempCollection->GetHit(i));
    uint64_t cellID = hit->cellID;  
    AddCellIDMask(cellID);
  }

  // apply a threshold of 480 electrons to each cell
  double threshold = 0.0; //480*0.0000036; // 480 electrons in MeV which should be default value
  int pixelsOverThreshold = 0;
  int pixelsOverThresholdPerLayer[50];
  std::vector<int> mipsPerPixel[50];
  for(int n(0); n<50; ++n){
    pixelsOverThresholdPerLayer[n]=0;
    mipsPerPixel[n].clear();
  }

  dd4hep::sim::Geant4CalorimeterHit* digi = nullptr;
  std::map<uint64_t, std::pair<int, dd4hep::sim::Geant4CalorimeterHit*> >::iterator it;
  std::map<Strixel, int> StrixelHitCounter;
  for(it=counter.begin(); it!=counter.end(); it++)
	{
    if(useMask) {
      AddCellIDMask((*it).second.second->cellID);
    }
    
    // if the energy deposited in the cellID is gtreather than threshold then do something
    if((*it).second.second->energyDeposit>=threshold) {      
      pixelsOverThreshold++;
      // insert the collection to G4 so can be passed on later
      digi = dynamic_cast<dd4hep::sim::Geant4CalorimeterHit*> ((*it).second.second);
      // this line overwrites the time parameter with the number of particles per pixel
      std::vector<dd4hep::sim::Geant4Hit::MonteCarloContrib> conts;     
      dd4hep::sim::Geant4Hit::MonteCarloContrib cont(0,0,0,0);

      if(m_countParticlesPerPixel) {
        //digi->energyDeposit = (*it).second.first; // overwrite the energy deposit with the number of mips in the cell
        cont.time = (*it).second.first;
      }

      conts.push_back(cont);
      digi->truth = conts;
      m_calorimeterCollection->insert(digi); // this passes to ROOT the readout... interesting!

    }
  }

  std::cout << m_TrackIDsPerEvent.size() << " " << m_tempCollection->entries() << " " << m_incidentParticles << " " << m_calorimeterCollection->entries() << std::endl;

  UpdateCellIDMask(); 

  // below this point is DECal chip specific where the readout is reconfidured
  // not needed for the analogue SiW

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

//  std::cout << std::endl;
//  std::cout << nHits << " " << pixelsOverThreshold << std::endl;
//  std::cout << "Total number of unique cellIDs = " << pixelsOverThreshold << std::endl;
//  std::cout << "Total number of unique strixels = " << strixelHits << std::endl;
//  std::cout << "Total number of strixel pixels (3) = " << strixelPixelHits_3 << std::endl ;
//  std::cout << "Total number of strixel pixels (7) = " << strixelPixelHits_7 << std::endl;
//  std::cout << "Total number of strixel pixels (15) = " << strixelPixelHits_15 << std::endl;
//  std::cout << "Total number of strixel pixels (31) = " << strixelPixelHits_31 << std::endl;
//  std::cout << "Total number of unique pads = " << PadHitCounter.size() << std::endl << std::endl;


  std::cout << "Counters\t"<< m_incidentParticles << "\t" << counter.size() << "\t" << pixelsOverThreshold << "\t" << strixelHits << "\t" << strixelPixelHits_3 << "\t" << strixelPixelHits_7 << "\t" << strixelPixelHits_15 << "\t" << strixelPixelHits_31 << "\t" << PadHitCounter.size() << std::endl;

 



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


}

