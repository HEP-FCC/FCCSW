#include "AtlfastElectronMatrixManager.hh"
#include "G4PrimaryParticle.hh"
#include <iostream>

namespace Atlfast
{
   using std::abs;
   using std::ifstream;

/** @brief Provides smear matrices corresponding to given track trajectories.
 *
 * Used by Atlfast::TrackSmearer. It reads a flat file containing smear matrix
 * data and creates a BinData object for every eta/rT bin.
 */

   //-----------------------------------------------------------
   // PUBLIC: Constructor
   //-----------------------------------------------------------
   ElectronMatrixManager::ElectronMatrixManager() 
   {
      G4cout  << "Constructed ElectronMatrixManager" << G4endl;
   }


   //-----------------------------------------------------------
   // PUBLIC: Destructor
   //-----------------------------------------------------------
   ElectronMatrixManager::~ElectronMatrixManager()
   {
      delete m_bremMgr;
      delete m_correlatedData;
      map<BinID, IBinData*>::iterator iter = m_binData.begin();
      map<BinID, IBinData*>::iterator end = m_binData.end();
      for ( ; iter != end; ++iter )
      {
         delete ( iter->second );
      }
   }

   //------------------------------------------------------------
   // PUBLIC: Constructor : read file and construct data bins
   //------------------------------------------------------------
   ElectronMatrixManager::ElectronMatrixManager(string aFileName, string /*aFileNameBrem*/, int aRandSeed)
   {
      m_randSeed = aRandSeed;
      m_file = aFileName;
      // m_bremFile = aFileNameBrem;
      // m_bremMgr = new BremMatrixManager( m_bremFile, m_randSeed );
      m_correlatedData = new CorrelatedData(m_randSeed);
      // open file
      ifstream input;
      input.open( m_file.c_str() );

      if (input)
      {
         G4cout << "ElectronMatrixManager: File " << m_file << " open." << G4endl;

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
         // core  resolutions:
         //               C0(d0, z0, phi0, cot(theta0), q/pT),
         //               C1(d0, z0, phi0, cot(theta0), q/pT), etc.
         // tails resolutions:
         //               C0(d0, z0, phi0, cot(theta0), q/pT),
         //               C1(d0, z0, phi0, cot(theta0), q/pT), etc.
         // core fractions:
         //               C0(d0, z0, phi0, cot(theta0), q/pT),
         //               C1(d0, z0, phi0, cot(theta0), q/pT), etc.
         // correlation coefficients rho:
         //               C0(rho(d0,phi0), rho(d0,q/pT), rho(phi0,q/pT), rho(z0,cot(theta0))),
         //               C1(rho(d0,phi0), rho(d0,q/pT), rho(phi0,q/pT), rho(z0,cot(theta0))), etc.
         //
         // coefficients C0,C1,C2,C3,C4 define pT-dependence of the respective quantities
         // according to f(pT) = C0 + C1/sqrt(pT) + C2/pT + C3/pT/sqrt(pT) + C4/pT^2

         //start bin id ints at zero
         int iBin = 0;

         for ( int rt = 0; rt < m_nRTBins; rt++ )
         {
            // make vectors to hold all resolution parameters for this rT bin
            vector<double> empty;

            vector< vector<double> >  coreC0( m_nEtaBins, empty );
            vector< vector<double> >  coreC1( m_nEtaBins, empty );
            vector< vector<double> >  coreC2( m_nEtaBins, empty );
            vector< vector<double> >  coreC3( m_nEtaBins, empty );
            vector< vector<double> >  coreC4( m_nEtaBins, empty );

            vector< vector<double> >  tailsC0( m_nEtaBins, empty );
            vector< vector<double> >  tailsC1( m_nEtaBins, empty );
            vector< vector<double> >  tailsC2( m_nEtaBins, empty );
            vector< vector<double> >  tailsC3( m_nEtaBins, empty );
            vector< vector<double> >  tailsC4( m_nEtaBins, empty );

            vector< vector<double> >  fractionsC0( m_nEtaBins, empty );
            vector< vector<double> >  fractionsC1( m_nEtaBins, empty );
            vector< vector<double> >  fractionsC2( m_nEtaBins, empty );
            vector< vector<double> >  fractionsC3( m_nEtaBins, empty );
            vector< vector<double> >  fractionsC4( m_nEtaBins, empty );

            vector< vector<double> >  correlationsC0( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC1( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC2( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC3( m_nEtaBins, empty );
            vector< vector<double> >  correlationsC4( m_nEtaBins, empty );


            //read eta bin number of values for each Ci, i=0,1,2,3,4, and parameter
            // read core data
            fillVector( input, coreC0, 5 );
            fillVector( input, coreC1, 5 );
            fillVector( input, coreC2, 5 );
            fillVector( input, coreC3, 5 );
            fillVector( input, coreC4, 5 );

            // read tail data
            fillVector( input, tailsC0, 5 );
            fillVector( input, tailsC1, 5 );
            fillVector( input, tailsC2, 5 );
            fillVector( input, tailsC3, 5 );
            fillVector( input, tailsC4, 5 );

            // read fractions data
            fillVector( input, fractionsC0, 5 );
            fillVector( input, fractionsC1, 5 );
            fillVector( input, fractionsC2, 5 );
            fillVector( input, fractionsC3, 5 );
            fillVector( input, fractionsC4, 5 );

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
               double etaLow = m_etaBoundaries[i];
               double etaHigh = m_etaBoundaries[i+1];

               // make bin id with rT and eta boundaries
               BinID rTEtaBin( iBin, m_rTBoundaries[rt], m_rTBoundaries[rt+1], etaLow, etaHigh );

               // make parameter resolutions for each parameter
               vector<ParameterResolutions*> core;
               vector<ParameterResolutions*> tails;
               vector<ParameterResolutions*> fractions;
               vector<ParameterResolutions*> correlations;
               for ( int param = 0; param < 5; param++ )
               {
                  vector<BinID> coreCoeffBins;
                  coreCoeffBins.push_back( BinID( 0, coreC0[i][param], coreC0[i+1][param] ) );
                  coreCoeffBins.push_back( BinID( 0, coreC1[i][param], coreC1[i+1][param] ) );
                  coreCoeffBins.push_back( BinID( 0, coreC2[i][param], coreC2[i+1][param] ) );
                  coreCoeffBins.push_back( BinID( 0, coreC3[i][param], coreC3[i+1][param] ) );
                  coreCoeffBins.push_back( BinID( 0, coreC4[i][param], coreC4[i+1][param] ) );
                  core.push_back( new ParameterResolutions( coreCoeffBins, etaLow, etaHigh ) );

                  vector<BinID> tailsCoeffBins;
                  tailsCoeffBins.push_back( BinID( 0, tailsC0[i][param], tailsC0[i+1][param] ) );
                  tailsCoeffBins.push_back( BinID( 0, tailsC1[i][param], tailsC1[i+1][param] ) );
                  tailsCoeffBins.push_back( BinID( 0, tailsC2[i][param], tailsC2[i+1][param] ) );
                  tailsCoeffBins.push_back( BinID( 0, tailsC3[i][param], tailsC3[i+1][param] ) );
                  tailsCoeffBins.push_back( BinID( 0, tailsC4[i][param], tailsC4[i+1][param] ) );
                  tails.push_back( new ParameterResolutions( tailsCoeffBins, etaLow, etaHigh ) );

                  vector<BinID> fractionsCoeffBins;
                  fractionsCoeffBins.push_back( BinID( 0, fractionsC0[i][param], fractionsC0[i+1][param] ) );
                  fractionsCoeffBins.push_back( BinID( 0, fractionsC1[i][param], fractionsC1[i+1][param] ) );
                  fractionsCoeffBins.push_back( BinID( 0, fractionsC2[i][param], fractionsC2[i+1][param] ) );
                  fractionsCoeffBins.push_back( BinID( 0, fractionsC3[i][param], fractionsC3[i+1][param] ) );
                  fractionsCoeffBins.push_back( BinID( 0, fractionsC4[i][param], fractionsC4[i+1][param] ) );
                  fractions.push_back( new ParameterResolutions( fractionsCoeffBins, etaLow, etaHigh ) );
               }

               for ( int param = 0; param < 4; param++ )
               {
                  vector<BinID> correlationsCoeffBins;
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC0[i][param], correlationsC0[i+1][param] ) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC1[i][param], correlationsC1[i+1][param] ) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC2[i][param], correlationsC2[i+1][param] ) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC3[i][param], correlationsC3[i+1][param] ) );
                  correlationsCoeffBins.push_back( BinID( 0, correlationsC4[i][param], correlationsC4[i+1][param] ) );
                  correlations.push_back( new ParameterResolutions( correlationsCoeffBins, etaLow, etaHigh ) );
               }

               // Now make a bin data with edges rT and eta, containing the
               // correlation data
               ElectronBinData* binData = new ElectronBinData( rTEtaBin, core, tails, fractions, correlations, m_randSeed );

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
         G4cout << "ElectronMatrixManager: no data file ( " << m_file << " )!!!!"
                << G4endl;
      }

   }

   //-----------------------------------------------------------
   // PRIVATE: fillVector
   //             reads n x M parameters into member variable
   //-----------------------------------------------------------
   void ElectronMatrixManager::fillVector( ifstream& input,
                                           vector< vector<double> >& myVector,
                                           int M = 5 )
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
   vector<double> ElectronMatrixManager::getVariables( const G4Track& track,
                                                       CLHEP::HepSymMatrix& returnSigma ) const
   {
      CLHEP::HepSymMatrix sigma;
      vector<double> variables;

      // // get bremsstrahlung corrections
      // G4Track* bremTrack = m_bremMgr->getBremTrack(track);

      // get data for gaussian smearing
      IBinData* binData = getBinData(track);
      sigma = binData->getMatrix(track);

      // do Dcorset and Dcorgen to get smeared parameters
      variables = m_correlatedData->generate( m_correlatedData->root(sigma) );

      // // add bremsstahlung and gaussian smearing effects
      // variables[0] += bremTrkParam.impactParameter();
      // variables[2] += bremTrkParam.phi();
      // variables[4] += bremTrkParam.invPtCharge();

      // adjust covariance matrix
      // (1,3) ... cov(d0,phi0)
      sigma(1,3) = sigma(1,3) / std::sqrt( sigma(1,1) * sigma(3,3) )
         * std::abs( variables[0] * variables[2] );
      // (1,5) ... cov(d0,q/pT)
      sigma(1,5) = sigma(1,5) / std::sqrt( sigma(1,1) * sigma(5,5) )
         * std::abs( variables[0] * variables[4] )  ;
      // (3,5) ... cov(phi0,q/pT)
      sigma(3,5) = sigma(3,5) / std::sqrt( sigma(3,3) * sigma(5,5) )
         * std::abs( variables[2] * variables[4] )  ;
      sigma(3,1) = sigma(1,3);
      sigma(5,1) = sigma(1,5);
      sigma(5,3) = sigma(3,5);
      sigma(1,1) = std::pow( variables[0], 2 );  // (1,1) ... cov(d0,d0)
      sigma(3,3) = std::pow( variables[2], 2 );  // (3,3) ... cov(phi0,phi0)
      sigma(5,5) = std::pow( variables[4], 2 );  // (5,5) ... cov(q/pT,q/pT)

      returnSigma = sigma;
      return variables;
   }


   //-----------------------------------------------------------
   // Private: getBinData : returns the IBinData of bin corresponding
   //                     to a given track trajectory
   //-----------------------------------------------------------
   IBinData* ElectronMatrixManager::getBinData( const G4Track& track ) const
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
      G4cout << "WARNING: ElectronMatrixManager - No bin; rT " << rT << ", eta " << eta
             << G4endl;

      return ( m_binData.begin() )->second;
   }

} // namespace
