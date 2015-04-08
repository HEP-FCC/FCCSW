#include "AtlfastCorrelatedData.hh"
#include "G4ios.hh"
#include <cmath>

namespace Atlfast{
   using std::abs;
   using std::pair;
   using std::vector;
   using std::cout;
   using std::endl;

   /** @brief Class to smear parameters that are correlated. */

   /** Constructor accepts random seed */
   CorrelatedData::CorrelatedData(int randSeed) {
      m_randomEngine = new CLHEP::HepJamesRandom( randSeed );
      m_ellipse = pair<double,double>(0.27597,0.27846);
      m_stFactors = pair<double, double>(0.449871,-0.386595);
      m_abFactors = pair<double, double>(0.19600,0.25472);
   }

   vector<double> CorrelatedData::normal(int nDev) const{
      vector<double> deviates;
      double deviate;
      for (int i=0; i<nDev; i++)
      {
         bool success=false;
         while (success == false) {
            pair<double,double> randoms( m_randomEngine->flat(),
                                         m_randomEngine->flat() );
            success = makeDeviate(randoms, deviate);
         }

         deviates.push_back(deviate);
      }
      return deviates;
   }

   bool CorrelatedData::makeDeviate(pair<double,double> randoms,
                                    double& deviate) const{

      double v, x, y, q;
      bool success = false;

      //some maths stuff
      v = 1.7156*(randoms.second - 0.5);
      x = randoms.first - m_stFactors.first;
      y = abs(v) - m_stFactors.second;
      q = x*x + y*( (m_abFactors.first)*y - (m_abFactors.second)*x);
      if (  (q <= m_ellipse.second) &&
            (v*v <= -4*log( (randoms.first) ) * (randoms.first) * (randoms.first) )
         ) success = true;
      if ( (q < m_ellipse.first) || success )
      {
         deviate = v/(randoms.first);
         return true;
      }
      else
         return false;
   }

   /** Generates smeared parameters from input matrix */
   vector<double> CorrelatedData::generate(const CLHEP::HepMatrix& matrix) const{

      int size = matrix.num_col();
      if( size != matrix.num_row() ) {
         cout << "CorrelatedData: WARNING: Matrix not square; using sub matrix"
              << endl;
         size = size < matrix.num_row() ? size : matrix.num_row();
      }

      vector<double> generated(size,0.0);
      vector<double> norm = normal(size);

      for (int i = 0; i < size; i++) {
         generated[i]=0;
         for (int j = 0; j < i+1; j++){
            generated[i] += matrix[i][j]*norm[j];
         }
      }
      return generated;
   }
   /** Generates smeared parameter from input number */
   double CorrelatedData::generate(double element) const{
      CLHEP::HepMatrix matrix(1,1,0);
      matrix(1,1) = element;
      return (generate(matrix)).front();
   }

   /** Returns square root matrix of original matrix */
   CLHEP::HepMatrix CorrelatedData::root(const CLHEP::HepMatrix& matrix) const{
      int size = matrix.num_col();
      if( size != matrix.num_row() ) {
         cout << "CorrelatedData: WARNING: Matrix not square; using sub matrix"
              << endl;
         size = size < matrix.num_row() ? size : matrix.num_row();
      }

      CLHEP::HepMatrix sqRoot(size, size, 0);
      double ck, rootElementSquared;

      // FIND SQUARE ROOT OF MATRIX

      for (int j = 0; j < size; j++) {
         // diagonals
         ck = 0;
         for (int k = 0; k < j; k++){
            ck += sqRoot[j][k]*sqRoot[j][k];
         }
         if ( ( rootElementSquared = abs(matrix[j][j]) - ck ) < 0 )  {
            sqRoot[j][j] = 1;
            cout << "CorrelatedData: WARNING: CRITICAL error in 'root(matrix)'"
                 << endl;
         }else{
            sqRoot[j][j] = sqrt(rootElementSquared);}
         //off diagonals
         for (int l = j+1; l < size; l++){
            ck = 0;
            for (int m = 0; m < j; m++){
               ck += sqRoot[l][m]*sqRoot[j][m];
            }
            sqRoot[l][j] = (matrix[l][j] - ck)/sqRoot[j][j];
         }
      }
      return sqRoot;
   }

}//end of namespace
