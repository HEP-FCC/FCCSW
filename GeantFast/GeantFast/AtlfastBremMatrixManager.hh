#ifndef ATLFAST_BREMMATRIXMANAGER_H
#define ATLFAST_BREMMATRIXMANAGER_H

#include "AtlfastBremBinData.hh"
#include "G4Track.hh"

#include "AtlfastBinID.hh"

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
   * Used by tracksmearer to provide smear matrices
   * corresponding to given track trajectories. It reads a flat
   * file containing smear matrix data and creates a BinData object for
   * every eta/rT bin.
   */
  class BremMatrixManager
  {

    public:

      /** Default Constructor */
      BremMatrixManager( string, int );

      /** Default Destructor */
      ~BremMatrixManager();

      /** returns modified track parameters according to input track */
      G4Track* getBremTrack( const G4Track& track ) const;


    private:

      /** reads file, creates BinData objects and asks them to calculate their matrices */
      void initialise();
      void fillVector( ifstream&, vector< vector<double> >&, int );
      void makeHeader();

      /** returns BremBinData object corresponding to track trajectory */
      BremBinData* getBinData( const G4Track& track ) const;

      /** name of flat file containing smear matrix data. */
      string m_file;

      /** BinData objects, paired to a BinID */
      map<BinID, BremBinData*> m_binData;

      int m_randSeed;
      vector<double> m_etaBoundaries;
      vector<double> m_rTBoundaries;
      int m_nRTBins;
      int m_nEtaBins;

  };

}

#endif
