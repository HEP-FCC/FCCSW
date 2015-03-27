#include "AtlfastMuonMatrixManager.hh"
#include "G4PrimaryParticle.hh"
#include <iostream>

namespace Atlfast
{
   using std::abs;
   using std::ifstream;

/** @brief Provides smear matrices corresponding to given track trajectories.
 *
 * Used by Atlfast::Tracksmearer. It reads a flat file containing smear matrix
 * data and creates a BinData object for every eta/rT bin.
 */

   //-----------------------------------------------------------
   // PUBLIC: Constructor
   //-----------------------------------------------------------
   MuonMatrixManager::MuonMatrixManager()
   {
      G4cout  << "Constructed MuonMatrixManager with NO input data file" << G4endl;
   }


   //-----------------------------------------------------------
   // PUBLIC: Destructor
   //-----------------------------------------------------------
   MuonMatrixManager::~MuonMatrixManager()
   {
      delete m_correlatedData;

      map< BinID, IBinData* >::iterator iter = m_binData.begin();
      map< BinID, IBinData* >::iterator end  = m_binData.end();
      for ( ; iter != end; ++iter )  delete ( iter->second );
   }

   //------------------------------------------------------------
   // PUBLIC: Constructor : read file and construct data bins
   //------------------------------------------------------------
   MuonMatrixManager::MuonMatrixManager(string aFileName, int aRandSeed)
   {
      m_randSeed = aRandSeed;
      m_file = aFileName;
      m_correlatedData = new CorrelatedData(m_randSeed);
      // open file
      ifstream input;
      input.open( m_file.c_str() );

      if (input)
      {
         G4cout << "MuonMatrixManager: File " << m_file << " open." << G4endl;

         double pTMin, etaMin, etaMax, rTBoundary;

         // read some parameters
         input >> pTMin;
         input >> etaMin;
         input >> etaMax;
         input >> m_nEtaBins;
         input >> m_nRTBins;

         // construct vector<binboundaries>
         double etaStep = ( etaMax - etaMin )/ double( m_nEtaBins );
         for ( int i = 0; i < m_nEtaBins; i++ )
         {
            m_etaBoundaries.push_back( etaMin + etaStep/2. + double(i)*etaStep );
         }
         for ( int i = 0; i < m_nRTBins + 1; i++ )
         {
            input >> rTBoundary;
            m_rTBoundaries.push_back( rTBoundary );
         }

         // parameters are stored in rT bin blocks----
         // with parameters in format:
         // Gaussian sigmas:
         //              C0(d0, z0, phi0, cot(theta0), q/pT),
         //              C1(d0, z0, phi0, cot(theta0), q/pT), etc.
         // correlation coefficients rho:
         //              C0(rho(d0,phi0), rho(d0,q/pT), rho(phi0,q/pT), rho(z0,cot(theta0))),
         //              C1(rho(d0,phi0), rho(d0,q/pT), rho(phi0,q/pT), rho(z0,cot(theta0))), etc.
         //
         // coefficients C0,C1,C2,C3,C4 define pT-dependence of the respective quantities
         // according to f(pT) = C0 + C1/sqrt(pT) + C2/pT + C3/pT/sqrt(pT) + C4/pT^2

         // start bin id ints at zero
         int iBin = 0;

         for ( int rt = 0; rt < m_nRTBins; rt++ )
         {
            // make vectors to hold all resolution parameters for this rt bin
            vector<double> empty;
            vector< vector<double> >  sigmaC0( m_nEtaBins, empty );
            vector< vector<double> >  sigmaC1( m_nEtaBins, empty );
            vector< vector<double> >  sigmaC2( m_nEtaBins, empty );
            vector< vector<double> >  sigmaC3( m_nEtaBins, empty );
            vector< vector<double> >  sigmaC4( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC0( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC1( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC2( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC3( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC4( m_nEtaBins, empty );

            // read eta bin number of values for each C and parameter
            // read Gaussian sigmas
            fillVector( input, sigmaC0, 5 );
            fillVector( input, sigmaC1, 5 );
            fillVector( input, sigmaC2, 5 );
            fillVector( input, sigmaC3, 5 );
            fillVector( input, sigmaC4, 5 );
            // read correlations data
            fillVector( input, correlationsC0, 4 );
            fillVector( input, correlationsC1, 4 );
            fillVector( input, correlationsC2, 4 );
            fillVector( input, correlationsC3, 4 );
            fillVector( input, correlationsC4, 4 );

            // DATA READING FINISHED FOR THIS RT BIN
            // got all values, now make rT/eta bins and resolution objects
            for ( int i = 0; i < m_nEtaBins - 1; ++i )
            {
               double etaLow  = m_etaBoundaries[i];
               double etaHigh = m_etaBoundaries[i+1];

               // make bin id with rT and eta boundaries
               BinID rTEtaBin( iBin, m_rTBoundaries[rt], m_rTBoundaries[rt+1], etaLow, etaHigh );

               //make parameter resolutions for each parameter
               vector<ParameterResolutions*> sigmas;
               vector<ParameterResolutions*> correlations;
               for ( int param = 0; param < 5; param++ )
               {
                  vector<BinID> sigmaCoeffBins;
                  sigmaCoeffBins.push_back( BinID( 0, sigmaC0[i][param], sigmaC0[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmaC1[i][param], sigmaC1[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmaC2[i][param], sigmaC2[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmaC3[i][param], sigmaC3[i+1][param] ) );
                  sigmaCoeffBins.push_back( BinID( 0, sigmaC4[i][param], sigmaC4[i+1][param] ) );
                  sigmas.push_back( new ParameterResolutions( sigmaCoeffBins, etaLow, etaHigh ) );
               }
               for ( int param = 0; param < 4; param++ )
               {
                  vector<BinID> correlationsCoeffBins;
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC0[i][param], correlationsC0[i+1][param]) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC1[i][param], correlationsC1[i+1][param]) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC2[i][param], correlationsC2[i+1][param]) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC3[i][param], correlationsC3[i+1][param]) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC4[i][param], correlationsC4[i+1][param]) );
                  correlations.push_back( new ParameterResolutions( correlationsCoeffBins, etaLow, etaHigh ) );
               }

               // Now make a bin data with edges rT and eta, containing the
               // correlation data
               MuonBinData* binData = new MuonBinData( rTEtaBin, sigmas, correlations );

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
         G4cout << "MuonMatrixManager: no data file ( " << m_file << " )!!!!"
                << G4endl;
      }
   }

   //-----------------------------------------------------------
   // PRIVATE: fillVector
   //             reads n x M parameters into member variable
   //-----------------------------------------------------------
   void MuonMatrixManager::fillVector( ifstream& input,
                                       vector< vector<double> >& myVector,
                                       int M=5 )
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
   // PUBLIC: getMatrix : returns the Sigma (=covariance) matrix
   //                  corresponding to a given track trajectory
   //-----------------------------------------------------------
   vector<double> MuonMatrixManager::getVariables( const G4Track& track,
                                                   CLHEP::HepSymMatrix& returnSigma ) const
   {
      CLHEP::HepSymMatrix Sigma;
      vector<double> variables;

      IBinData* binData = getBinData(track);
      Sigma = binData->getMatrix(track);

      // do Dcorset and Dcorgen to get smear parameters
      variables = m_correlatedData->generate( m_correlatedData->root(Sigma) );

      returnSigma = Sigma;
      return variables;
   }


   //-----------------------------------------------------------
   // Private: getBinData : returns the IBinData of bin
   //                 corresponding to a given track trajectory
   //-----------------------------------------------------------
   IBinData* MuonMatrixManager::getBinData( const G4Track& track ) const
   {
      vector<double> rTEta;
      double rT = abs( track.GetVertexPosition().perp() );
      double eta = abs( track.GetVertexMomentumDirection().pseudoRapidity() );

      // validity check
      double rTLow   = ( (m_binData.begin())->first ).low(0);
      double rTHigh  = ( (m_binData.rbegin())->first ).high(0);
      double etaLow  = ( (m_binData.begin())->first ).low(1);
      double etaHigh = ( (m_binData.rbegin())->first ).high(1);

      if ( rT < rTLow ) rT = rTLow;
      if ( rT > rTHigh) rT = rTHigh;
      if ( eta < etaLow ) eta = etaLow;
      if ( eta > etaHigh) eta = etaHigh;

      // find BinID
      rTEta.push_back(rT);
      rTEta.push_back(eta);

      map<BinID, IBinData*>::const_iterator binIter = m_binData.begin();
      map<BinID, IBinData*>::const_iterator binEnd  = m_binData.end();

      for ( ; binIter != binEnd; ++binIter )
      {
         if ( binIter->first.isInBin(rTEta) )
         {
            return binIter->second;
         }
      }

      // OOPS! couldn't fin bin
      G4cout << "WARNING: MuonMatrixManager - No bin; rT " << rT << ", eta " << eta
             << G4endl;

      return ( m_binData.begin() )->second;
   }

}// namespace

