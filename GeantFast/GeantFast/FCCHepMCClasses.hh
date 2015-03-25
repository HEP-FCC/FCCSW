//
// Header file with all the includes used to create the HepMC ROOT dictionary
//

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenRanges.h"
#include "HepMC/Units.h"
#include <vector>
#include <map>
std::map< int,HepMC::GenVertex*,std::greater<int> > dicta;
std::map< int,HepMC::GenVertex*,std::less<int> > dictb;
std::vector<HepMC::GenParticle*> dictc;
enum HepMC::Units::MomentumUnit dictd;
enum HepMC::Units::LengthUnit dicte;
std::map<std::string,unsigned long> dictf;
std::map<int,HepMC::GenParticle*> dictg;
std::map<int,int> dicth;
std::map<std::string,unsigned long> dicti;
std::pair<string,unsigned long> dictj;
std::pair<int,HepMC::GenVertex*> dictk;
std::pair<int,HepMC::GenParticle*> dictl;
