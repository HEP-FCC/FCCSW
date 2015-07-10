//
//  printTuple.cxx
//  
//
//  Created by Julia Hrdinka on 19/03/15.
//
//

#include <stdio.h>
/*

void printTuple (int i=0){
    cout << "Test: " << i << std::endl;

}
*/
void printTuple (const char* file, const char* draw, int color = 0, const char* drawcase = "", const char* drawopt = "" ){
    
    TNtuple *ntuple = new TNtuple("name","title", "x:y:z");
    ntuple->ReadFile(file);
    ntuple->SetMarkerColor(color);
    ntuple->Draw(draw, drawcase, drawopt);
}

//example: .x printTuple.cxx(“modules.dat","x:y:z",1)


/*Det::DetDiscLayer* detdisclayer = dynamic_cast<Det::DetDiscLayer*>(ext);
 //DiscLayer
 if (detelement.isValid() && detdisclayer) {
 //l m_out << "detdisclayer" << std::endl;
 DD4hep::Geometry::PlacedVolume placedvolume = detelement.placement();
 TGeoNode* geonode = placedvolume.ptr();
 if (geonode) {
 TGeoShape* geoshape = geonode->GetVolume()->GetShape();
 const Double_t* rotation    = (geonode->GetMatrix()->GetRotationMatrix());
 const Double_t* translation = (geonode->GetMatrix()->GetTranslation());
 std::shared_ptr<Alg::Transform3D> transf(new Alg::Transform3D(
 rotation[0], rotation[1], rotation[2], translation[0],
 rotation[3], rotation[4], rotation[5], translation[1],
 rotation[6], rotation[7], rotation[8], translation[2]));
 (*transf) = (*transform)*(*transf);
 if(geoshape && geoshape->IsA() == TGeoConeSeg::Class()){
 //weitere if Bedingung zur Unterscheidung zu Disc -> inner r und z klein??
 TGeoConeSeg* disc = dynamic_cast<TGeoConeSeg*>(geoshape);
 if (disc) {
 double minPhi   = detdisclayer->min();
 double maxPhi   = detdisclayer->max();
 int binsPhi = detdisclayer->modulesPhi();
 double step = (maxPhi-minPhi)/binsPhi;
 double minPhiCorrected = minPhi-0.5*step;
 double maxPhiCorrected = maxPhi+0.5*step;
 if (minPhiCorrected < -M_PI) {
 minPhiCorrected += step;
 maxPhiCorrected += step;
 }
 
 std::vector<std::pair<float,float>> rValues = detdisclayer->rValues();
 std::vector<float> newrValues = orderRValues(rValues);
 //now create BinnedArray with BinUtility for a Cylinder and Surfaces, for the Layer
 Trk::BinUtility* currentBinUtility = new Trk::BinUtility(newrValues,Trk::open,Trk::binR);
 (*currentBinUtility) += Trk::BinUtility(binsPhi,minPhiCorrected,maxPhiCorrected,Trk::closed,Trk::binPhi);
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> surfaces;
 //translateModule
 //                m_file << "transf layer before: " << *transf << std::endl;
 translateModule(detelement,surfaces,transf);
 //                 m_file << "transf layer after: " << *transf << std::endl;
 if (!surfaces.empty()) {
 //-changed
 //-possibility to have more than one surface at the same phi and r position layered in z
 //sort surfaces after same r and phi
 //                       std::cout << "Svc::numberOfSurfaces: " << surfaces.size() << std::endl;
 std::vector<std::shared_ptr<const Reco::Surface>> same;
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> keys_dedupR;
 //sort in Phi
 std::stable_sort(surfaces.begin(),surfaces.end(),
 [](const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& a,
 const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& b)
 {return (a.second.Phi()<b.second.Phi());});
 
 //put surfaces with same Phi in a vector
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> samePhi;
 std::vector<std::pair<std::shared_ptr<const std::vector<std::shared_ptr<const Reco::Surface>>>,Alg::Point3D>> ordered;
 //get key values in Phi
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> keys_dedupP;
 std::unique_copy(begin(surfaces),
 end(surfaces),
 back_inserter(keys_dedupR),
 [](const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& a,
 const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& b)
 {return ((sqrt(a.second.X()*a.second.X()+a.second.Y()*a.second.Y()))==(sqrt(b.second.X()*b.second.X()+b.second.Y()*b.second.Y())));}
 );
 
 std::cout << "keys size() Phi: " << keys_dedupR.size() << std::endl;
 //////getting rValues/////
 
 */

/*            std::vector<float> newRValues;
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>> keys_dedupR1;
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>> keys_dedupR2;
 for (auto& it : keys_dedupR) keys_dedupR1.push_back(std::make_pair(it.first,sqrt(it.second.Perp2())));
 
 std::unique_copy(begin(keys_dedupR1),
 end(keys_dedupR1),
 back_inserter(keys_dedupR2),
 [](const std::pair<std::shared_ptr<const Reco::Surface>,float>& a,
 const std::pair<std::shared_ptr<const Reco::Surface>,float>& b)
 {return (a.second==b.second);}
 );
 for (std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>>::iterator it = keys_dedupR2.begin(); it!=keys_dedupR2.end(); it++) {
 std::shared_ptr<const Reco::TrapezoidSurface> trapezoidsurf = std::dynamic_pointer_cast<const Reco::TrapezoidSurface> (it->first);
 newRValues.push_back(it->second-trapezoidsurf->getHalfY());
 if ((it+1)==keys_dedupR2.end()) newRValues.push_back(it->second+trapezoidsurf->getHalfY());
 
 }
 newRValues.clear();*/
///////            ///////

//make vector<vector>
/*                  for (const auto& itkeysP : keys_dedupP)
 {
 std::cout << "itkeys: " << sqrt(itkeysP.second.Phi())<< " center: " << itkeysR.second<< std::endl;
 std::pair<std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>>::iterator,
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>>::iterator> boundsP;
 boundsP = std::equal_range(surfaces.begin(), surfaces.end(), itkeysP,
 [](const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& a,
 const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& b){
 return (a.second.Phi()<b.second.Phi());});
 
 for (std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>>::iterator samePit=boundsP.first; samePit!=(boundsP.second); ++samePit)
 {
 sameP.push_back(*sameRit);
 }
 
 std::cout << "same Phi size: " << sameP.size() << std::endl;
 
 //now sort in R
 std::stable_sort(sameP.begin(),sameP.end(),
 [](const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& a,
 const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& b)
 {return (a.second.Perp2()<b.second.Perp2());});
 
 //put surfaces with same phi in vector
 std::vector<std::shared_ptr<const Reco::Surface>> same;
 //    std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> sameP;
 //        std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> keys_dedupP;
 keys_dedupR.clear();
 std::unique_copy(begin(sameP),
 end(sameP),
 back_inserter(keys_dedupR),
 [](const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& a,
 const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& b)
 {return (a.second.Perp2()==b.second.Perp2());}
 );
 
 
 for (const auto& itkeysR : keys_dedupR) {
 std::pair<std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>>::iterator,
 std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>>::iterator> boundsR;
 boundsR = std::equal_range(sameP.begin(), sameP.end(), itkeysR,
 [](const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& a,
 const std::pair<std::shared_ptr<const Reco::Surface>,Alg::Point3D>& b){
 return (a.second.Perp2()<b.second.Perp2());});
 
 for (std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>>::iterator sameRit=boundsR.first; samePit!=(boundsR.second); ++sameRit)
 {
 same.push_back(sameRit->first);
 }
 
 std::cout << "same: " << same.size()<< std::endl;
 
 ordered.push_back(std::make_pair(std::make_shared<std::vector<std::shared_ptr<const Reco::Surface>>>(same),itkeysR.second));
 same.clear();
 }
 
 sameP.clear();
 }
 
 //             std::cout << "minPhi: " << minPhi << " maxPhi: " << maxPhi << " front: " << keys_dedupP.front().second.Phi() << " back: " << keys_dedupP.back().second.Phi() << " binsPhi: " << binsPhi << " keys: " << keys_dedupP.size() << std::endl;
 
 //now create BinnedArray with BinUtility for a CylinderLayer with the Surfaces, for the Layer
 Trk::BinnedArray2D<std::vector<std::shared_ptr<const Reco::Surface>>>* bin = new Trk::BinnedArray2D<std::vector<std::shared_ptr<const Reco::Surface>>>(ordered,currentBinUtility);
 //- changed
 std::shared_ptr<const Reco::DiscLayer> disclayer = std::make_shared<const Reco::DiscLayer>(transf,disc,bin);
 if (disclayer) {
 Alg::Point3D center = disclayer->center();
 std::pair<std::shared_ptr<const Reco::Layer>, Alg::Point3D> layer (disclayer,center);
 layers.push_back(layer);
 }//if disclayer
 }
 } //if disc
 
 } //if shape
 //           transform = 0;
 } //if node
 //           ++m_counter;
 } //if detdisclayer
 
 */

