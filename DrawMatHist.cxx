//
//  DrawMatHist.cxx
//  
//
//  Created by Julia Hrdinka on 28/05/15.
//
//

void DrawMatHist ()
{
    TCanvas* c1 = new TCanvas();
    TFile G4f("geant4detector.root");
    TProfile* p2 = (TProfile*)G4f.Get("tInX01");
    
    TProfile* p1 = (TProfile*)G4f.Get("G4::tInX0");
 //   TProfile* p2 = (TProfile*)f.Get("tInX0");
   
    p2->SetFillColor(46);
    p2->SetLineColor(46);
    p2->SetMarkerColor(46);
    p2->Draw();

    p1->SetFillColor(41);
    p1->SetMarkerColor(41);
    p1->SetLineColor(41);
    p1->Draw("same");


 //    TFile f("hitproperies.root");;

TCanvas* c2 = new TCanvas();
TProfile* p2 = (TProfile*)G4f.Get("Pathlength1");

TProfile* p1 = (TProfile*)G4f.Get("G4::Pathlength");
//   TProfile* p2 = (TProfile*)f.Get("tInX0");

p2->SetFillColor(38);
p2->SetLineColor(38);
p2->SetMarkerColor(38);
p2->Draw();

p1->SetFillColor(42);
p1->SetMarkerColor(42);
p1->SetLineColor(42);
p1->Draw("same");

TCanvas* c3 = new TCanvas();
TProfile* p2 = (TProfile*)G4f.Get("Hits");

TProfile* p1 = (TProfile*)G4f.Get("G4::Hits");
//   TProfile* p2 = (TProfile*)f.Get("tInX0");

p2->SetFillColor(49);
p2->SetLineColor(49);
p2->SetMarkerColor(49);
p2->Draw();

p1->SetFillColor(44);
p1->SetMarkerColor(44);
p1->SetLineColor(44);
p1->Draw("same");





    Det::DetDiscLayer* detdisclayer = dynamic_cast<Det::DetDiscLayer*>(ext);
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
                        std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>> keys_dedupP;
                        std::vector<std::pair<std::shared_ptr<const Reco::Surface>,float>> newsurfaces;
                        for (auto& it : surfaces) newsurfaces.push_back(std::make_pair(surfaces.first,sqrt(surfaces.second.Perp2())));
                        //sort in R
                        std::stable_sort(newsurfaces.begin(),newsurfaces.end(),
                                         [](const std::pair<std::shared_ptr<const Reco::Surface>,float>& a,
                                            const std::pair<std::shared_ptr<const Reco::Surface>,float>& b)
                                         {return (a.second < b.second);});
        //                std::cout << "SORT IN R:" << std::endl;
       //                 for (auto& surfit : surfaces)std::cout << sqrt(surfit.second.X()*surfit.second.X()+surfit.second.Y()*surfit.second.Y()) << std::endl;
                        //put surfaces with same r in a vector
                        std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> sameR;
                        std::vector<std::pair<std::shared_ptr<const std::vector<std::shared_ptr<const Reco::Surface>>>,Alg::Point3D>> ordered;
                        //get key values in r
                        std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>> keys_dedupR;
                        std::unique_copy(begin(newsurfaces),
                                         end(newsurfaces),
                                         back_inserter(keys_dedupR),
                                         [](const std::pair<std::shared_ptr<const Reco::Surface>,float>& a,
                                            const std::pair<std::shared_ptr<const Reco::Surface>,float>& b)
                                         {return (a.second==b.second);}
                                         );
                        
                        std::cout << "keys size() R: " << keys_dedupR.size() << std::endl;
                        //////getting rValues/////
                        
   /*                     std::vector<float> newRValues;
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
                        for (const auto& itkeysR : keys_dedupR)
                        {
              //              std::cout << std::setprecision(6) << "itkeys: " << sqrt(itkeysR.second.Perp2())<< " center: " << itkeysR.second<< std::endl;
                            std::pair<std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>>::iterator,
                            std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>>::iterator> boundsR;
                            boundsR = std::equal_range(newsurfaces.begin(), newsurfaces.end(), itkeysR,
                                                       [](const std::pair<std::shared_ptr<const Reco::Surface>,float>& a,
                                                          const std::pair<std::shared_ptr<const Reco::Surface>float>& b){
                                                           return (a.second<b.second);});
                            
                            for (std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>>::iterator sameRit=boundsR.first; sameRit!=(boundsR.second); ++sameRit)
                            {
                                sameR.push_back(std::make_pair(sameRit->first,sameRit->first->center()));
                            }
                            
                            std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>> newsameR;
                            for(auto& it : sameR) newsameR.push_back(std::make_pair(it.first,it.second.Phi()));
                            std::cout << "same r size: " << sameR.size() << std::endl;
                            
                            
                            //now sort in Phi
                            std::stable_sort(newsameR.begin(),newsameR.end(),
                                             [](const std::pair<std::shared_ptr<const Reco::Surface>,float>& a,
                                                const std::pair<std::shared_ptr<const Reco::Surface>,float>& b)
                                             {return (a.second<b.second);});
                            
                            //put surfaces with same phi in vector
                            std::vector<std::shared_ptr<const Reco::Surface>> same;
                            //    std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> sameP;
                            //        std::vector<std::pair<std::shared_ptr<const Reco::Surface>, Alg::Point3D>> keys_dedupP;
                            keys_dedupP.clear();
                            std::unique_copy(begin(newsameR),
                                             end(newsameR),
                                             back_inserter(keys_dedupP),
                                             [](const std::pair<std::shared_ptr<const Reco::Surface>,float>& a,
                                                const std::pair<std::shared_ptr<const Reco::Surface>,float>& b)
                                             {return (a.second==b.second);}
                                             );
                            
                            
                            for (const auto& itkeysP : keys_dedupP) {
                                std::pair<std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>>::iterator,
                                std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>>::iterator> boundsP;
                                boundsP = std::equal_range(sameR.begin(), sameR.end(), itkeysP,
                                                           [](const std::pair<std::shared_ptr<const Reco::Surface>,float>& a,
                                                              const std::pair<std::shared_ptr<const Reco::Surface>,float>& b){
                                                               return (a.second<b.second);});
                                
                                for (std::vector<std::pair<std::shared_ptr<const Reco::Surface>, float>>::iterator samePit=boundsP.first; samePit!=(boundsP.second); ++samePit)
                                {
                                    same.push_back(samePit->first);
                                }
                                
                                std::cout << "same: " << same.size()<< std::endl;
                                
                                ordered.push_back(std::make_pair(std::make_shared<std::vector<std::shared_ptr<const Reco::Surface>>>(same),itkeysP.first->center()));
                                same.clear();
                            }
                            
                            sameR.clear();
                        }
                        
                        std::cout << "minPhi: " << minPhi << " maxPhi: " << maxPhi << " front: " << keys_dedupP.front().second.Phi() << " back: " << keys_dedupP.back().second.Phi() << " binsPhi: " << binsPhi << " keys: " << keys_dedupP.size() << std::endl;
                        
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