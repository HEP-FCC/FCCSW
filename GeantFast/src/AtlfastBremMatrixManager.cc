#include "AtlfastBremMatrixManager.hh"
#include "G4PrimaryParticle.hh"
#include <iostream>

namespace Atlfast
{
   using std::abs;
   using std::ifstream;

/** @brief Provides smear matrices corresponding to given track trajectories.
 *
 * Used by tracksmearer to provide smear matrices
 * corresponding to given track trajectories. It reads a flat
 * file containing smear matrix data and creates a BinData object for
 * every eta/rT bin.
 */


   //-----------------------------------------------------------
   // PUBLIC: Constructor
   //-----------------------------------------------------------
   BremMatrixManager::BremMatrixManager( string paramFile, int randSeed ) :
      m_file( paramFile ),
      m_randSeed( randSeed )
   {
      G4cout<<"Constructing BremMatrixManager with parameter file "
            << m_file
            << G4endl;
      this->initialise();
      G4cout<<"Constructed BremMatrixManager" << G4endl;
   }


   //-----------------------------------------------------------
   // PUBLIC: Destructor
   //-----------------------------------------------------------
   BremMatrixManager::~BremMatrixManager()
   {
      map<BinID, BremBinData*>::iterator iter = m_binData.begin();
      map<BinID, BremBinData*>::iterator end = m_binData.end();
      for ( ; iter != end; ++iter )
      {
         delete ( iter->second );
      }
   }

   //------------------------------------------------------------
   // PRIVATE: initialise : read file and construct data bins
   //------------------------------------------------------------
   void BremMatrixManager::initialise()
   {
      // open file
      ifstream input;
      input.open( m_file.c_str() );

      if (input)
      {
         G4cout<< "BremMatrixManager: File " << m_file << " open." << G4endl;

         double pTMin, etaMin, etaMax, rtBoundary;

         // read some parameters
         input >> pTMin;
         input >> etaMin;
         input >> etaMax;
         input >> m_nEtaBins;
         input >> m_nRTBins;

         // construct vector<binboundaries>
         double etaStep = (etaMax - etaMin) / double(m_nEtaBins);
         for ( int i = 0; i < m_nEtaBins; i++ )
         {
            m_etaBoundaries.push_back( etaMin + etaStep/2. + double(i)*etaStep );
         }

         for ( int i = 0; i < m_nRTBins + 1; i++ )
         {
            input >> rtBoundary;
            m_rTBoundaries.push_back(rtBoundary);
         }

         // parameters are stored in rT bin blocks----
         // with parameters in format:
         // power function start points: C0(d0, phi0, q/pT),
         //                              C1(d0, phi0, q/pT), etc.
         // power function slopes: C0(d0, phi0, q/pT),
         //                        C1(d0, phi0, q/pT), etc.
         // Gaussian sigmas: C0(d0, phi0, q/pT),
         //                  C1(d0, phi0, q/pT), etc.
         //
         // coefficients C0,C1,C2,C3,C4 define pT-dependence of the respective quantities
         // according to f(pT) = C0 + C1/sqrt(pT) + C2/pT + C3/pT/sqrt(pT) + C4/pT^2

         //start bin id ints at zero
         int iBin = 0;

         for ( int rt = 0; rt < m_nRTBins; rt++ )
         {
            // make vectors to hold all resolution parameters for this rT bin
            vector<double> empty;

            vector< vector<double> >  startPointsC0( m_nEtaBins, empty );
            vector< vector<double> >  startPointsC1( m_nEtaBins, empty );
            vector< vector<double> >  startPointsC2( m_nEtaBins, empty );
            vector< vector<double> >  startPointsC3( m_nEtaBins, empty );
            vector< vector<double> >  startPointsC4( m_nEtaBins, empty );

            vector< vector<double> >  slopesC0( m_nEtaBins, empty );
            vector< vector<double> >  slopesC1( m_nEtaBins, empty );
            vector< vector<double> >  slopesC2( m_nEtaBins, empty );
            vector< vector<double> >  slopesC3( m_nEtaBins, empty );
            vector< vector<double> >  slopesC4( m_nEtaBins, empty );

            vector< vector<double> >  sigmasC0( m_nEtaBins, empty );
            vector< vector<double> >  sigmasC1( m_nEtaBins, empty );
            vector< vector<double> >  sigmasC2( m_nEtaBins, empty );
            vector< vector<double> >  sigmasC3( m_nEtaBins, empty );
            vector< vector<double> >  sigmasC4( m_nEtaBins, empty );

            //read eta bin number of values for each Ci, i=0,1,2,3, and parameter
            // read start point data
            fillVector( input, startPointsC0, 3 );
            fillVector( input, startPointsC1, 3 );
            fillVector( input, startPointsC2, 3 );
            fillVector( input, startPointsC3, 3 );
            fillVector( input, startPointsC4, 3 );

            // read slope data
            fillVector( input, slopesC0, 3 );
            fillVector( input, slopesC1, 3 );
            fillVector( input, slopesC2, 3 );
            fillVector( input, slopesC3, 3 );
            fillVector( input, slopesC4, 3 );

            // read Gaussian sigma data
            fillVector( input, sigmasC0, 3 );
            fillVector( input, sigmasC1, 3 );
            fillVector( input, sigmasC2, 3 );
            fillVector( input, sigmasC3, 3 );
            fillVector( input, sigmasC4, 3 );

            // DATA READING FINISHED FOR THIS RT BIN

            // got all values, now make rT/eta bins and resolution objects
            for ( int i = 0; i < m_nEtaBins - 1; ++i )
            {
               double etaLow = m_etaBoundaries[i];
               double etaHigh = m_etaBoundaries[i+1];

               // make bin id with rT and eta boundaries
               BinID rTEtaBin( iBin, m_rTBoundaries[rt], m_rTBoundaries[rt+1], etaLow, etaHigh );

               // make parameter resolutions for each parameter
               vector<ParameterResolutions*> startPoints;
               vector<ParameterResolutions*> slopes;
               vector<ParameterResolutions*> sigmas;
               for ( int param = 0; param < 3; param++ )
               {
                  vector<BinID> startPointCoeffBins;
                  startPointCoeffBins.push_back( BinID( 0, startPointsC0[i][param], startPointsC0[i+1][param] ) );
                  startPointCoeffBins.push_back( BinID( 0, startPointsC1[i][param], startPointsC1[i+1][param] ) );
                  startPointCoeffBins.push_back( BinID( 0, startPointsC2[i][param], startPointsC2[i+1][param] ) );
                  startPointCoeffBins.push_back( BinID( 0, startPointsC3[i][param], startPointsC3[i+1][param] ) );
                  startPointCoeffBins.push_back( BinID( 0, startPointsC4[i][param], startPointsC4[i+1][param] ) );
                  startPoints.push_back( new ParameterResolutions( startPointCoeffBins, etaLow, etaHigh ) );

                  vector<BinID> slopeCoeffBins;
                  slopeCoeffBins.push_back( BinID( 0, slopesC0[i][param], slopesC0[i+1][param] ) );
                  slopeCoeffBins.push_back( BinID( 0, slopesC1[i][param], slopesC1[i+1][param] ) );
                  slopeCoeffBins.push_back( BinID( 0, slopesC2[i][param], slopesC2[i+1][param] ) );
                  slopeCoeffBins.push_back( BinID( 0, slopesC3[i][param], slopesC3[i+1][param] ) );
                  slopeCoeffBins.push_back( BinID( 0, slopesC4[i][param], slopesC4[i+1][param] ) );
                  slopes.push_back( new ParameterResolutions( slopeCoeffBins, etaLow, etaHigh ) );

                  vector<BinID> sigmaCoeffBins;
                  sigmaCoeffBins.push_back( BinID( 0, sigmasC0[i][param], sigmasC0[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmasC1[i][param], sigmasC1[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmasC2[i][param], sigmasC2[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmasC3[i][param], sigmasC3[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmasC4[i][param], sigmasC4[i+1][param] ) );
                  sigmas.push_back( new ParameterResolutions( sigmaCoeffBins, etaLow, etaHigh ) );
               }

               // Now make a bin data with edges rT and eta, containing the
               // data for bremsstrahlung corrections
               BremBinData* binData = new BremBinData( rTEtaBin, startPoints, slopes, sigmas, m_randSeed );

               // enter bin data into map
               m_binData[rTEtaBin] = binData;

               // increment bin ID
               iBin++;

            } // end of eta bin loop
         } // end of rT bin loop

         // close file
         input.close();

      }
      else  // no input file
      {
         G4cout << "BremMatrixManager: no data file ( " << m_file << " )!!!!"
                << G4endl;
      }

   }

   //-----------------------------------------------------------
   // PRIVATE: fillVector
   //             reads n x M parameters into member variable
   //-----------------------------------------------------------
   void BremMatrixManager::fillVector( ifstream& input,
                                       vector< vector<double> >& myVector,
                                       int M = 3 )
   {
      double res;
      for ( int param = 0; param < M; param++ )
      {
         vector< vector<double> >::iterator binIter = myVector.begin();
         for ( ; binIter != myVector.end(); binIter++ )
         {
            input >> res;
            binIter->push_back(res);
         }

      }
   }


   //-----------------------------------------------------------
   // PUBLIC: getMatrix : returns the sigma matrix corresponding
   //                     to a given track trajectory
   //-----------------------------------------------------------
   G4Track* BremMatrixManager::getBremTrack( const G4Track& track ) const
   {

      BremBinData* binData = getBinData(track);

      G4Track* newTrack = binData->getBremTrack(track);

      return newTrack;
   }


   //-----------------------------------------------------------
   // Private: getBinData : returns the BremBinData of bin corresponding
   //                       to a given track trajectory
   //-----------------------------------------------------------
   BremBinData* BremMatrixManager::getBinData( const G4Track& track ) const
   {
      vector<double> rTEta;
      double rT = abs( track.GetVertexPosition().perp() );
      double eta = abs( track.GetVertexMomentumDirection().pseudoRapidity() );

      // validity check
      double rTLow = ( (m_binData.begin())->first ).low(0);
      double rTHigh = ( (m_binData.rbegin())->first ).high(0);
      double etaLow = ( (m_binData.begin())->first ).low(1);
      double etaHigh = ( (m_binData.rbegin())->first ).high(1);

      if ( rT < rTLow )  rT = rTLow;
      if ( rT > rTHigh ) rT = rTHigh;
      if ( eta < etaLow )  eta = etaLow;
      if ( eta > etaHigh ) eta = etaHigh;

      // find BinID
      rTEta.push_back(rT);
      rTEta.push_back(eta);

      map<BinID, BremBinData*>::const_iterator binIter = m_binData.begin();
      map<BinID, BremBinData*>::const_iterator binEnd  = m_binData.end();

      for ( ; binIter != binEnd; ++binIter )
      {
         if ( binIter->first.isInBin(rTEta) )
         {
            return binIter->second;
         }
      }
      // OOPS! couldn't fin bin
      G4cout<<"WARNING: BremMatrixManager - No bin; rT " << rT << ", eta " << eta
            << G4endl;

      return ( m_binData.begin() )->second;
   }


} // namespace
