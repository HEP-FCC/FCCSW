#ifndef ATLFAST_MUONMATRIXMANAGER_H
#define ATLFAST_MUONMATRIXMANAGER_H

#include "AtlfastMuonBinData.hh"
#include "G4Track.hh"

#include "AtlfastBinID.hh"
#include "AtlfastCorrelatedData.hh"

#include <vector>
#include <string>
#include <map>
#include <utility>

//===========================================================
//
//  Matrix Manager
//
//  Used by tracksmearer to provide smear matrix data
//  corresponding to a given track trajectory
//
//===========================================================


namespace Atlfast
{
   using std::string;
   using std::ifstream;
   using std::map;

   /** @brief Provides smear matrices corresponding to given track trajectories.
    *
    * Used by Atlfast::Tracksmearer. It reads a flat file containing smear matrix
    * data and creates a BinData object for every eta/rT bin.
    */
   class MuonMatrixManager
   {

   public:

      /** returns correlation matrix corresponding to given track trajectory */
      vector<double> getVariables( const G4Track& track,
                                   CLHEP::HepSymMatrix& usedSigma ) const;
      /** Default Constructor */
      MuonMatrixManager( );

      MuonMatrixManager( string, int);

      /** Default Destructor */
      ~MuonMatrixManager();
   private:
      void fillVector( ifstream&, vector< vector<double> >&, int );

      /** returns BinData object corresponding to track trajectory */
      IBinData* getBinData( const G4Track& track ) const;

      /** name of flat file containing smear matrix data. */
      string m_file;

      /** BinData objects, paired to a BinID */
      map<BinID, IBinData*> m_binData;

      /** object which performs dcorset/dcorgen */
      CorrelatedData* m_correlatedData;

      int m_randSeed;
      vector<double> m_etaBoundaries;
      vector<double> m_rTBoundaries;
      int m_nRTBins;
      int m_nEtaBins;

   };

}

#endif

