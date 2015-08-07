
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef GenEvent_BeamParameters_H
#define GenEvent_BeamParameters_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/DataObject.h"
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
#endif
#include "GaudiKernel/boost_allocator.h"
#include <ostream>

// Forward declarations

namespace LHCb
{

  // Forward declarations
  
  // Class ID definition
  static const CLID CLID_BeamParameters = 200;
  
  // Namespace for locations in TDS
  namespace BeamParametersLocation {
    static const std::string Default = "Gen/BeamParameters";
  }
  

  /** @class BeamParameters BeamParameters.h
   *
   * Parameters of the beam
   *
   * @author P. Robbe
   * created Wed Jul 22 09:45:44 2015
   *
   */

  class BeamParameters: public DataObject
  {
  public:

    /// Default Constructor
    BeamParameters() : m_energy(3.5*Gaudi::Units::TeV),
                       m_sigmaS(0.0),
                       m_epsilonN(0.0),
                       m_revolutionFrequency(11.245*Gaudi::Units::kilohertz),
                       m_totalXSec(91.1*Gaudi::Units::millibarn),
                       m_horizontalCrossingAngle(0.130*Gaudi::Units::mrad),
                       m_verticalCrossingAngle(0.130*Gaudi::Units::mrad),
                       m_horizontalBeamlineAngle(0.0),
                       m_verticalBeamlineAngle(0.0),
                       m_betaStar(3.0*Gaudi::Units::m),
                       m_bunchSpacing(50.*Gaudi::Units::ns),
                       m_beamSpot(0.0, 0.0, 0.0),
                       m_luminosity(0.2e30/(Gaudi::Units::cm2*Gaudi::Units::s)) {}
  
    /// Default Destructor
    virtual ~BeamParameters() {}
  
    // Retrieve pointer to class definition structure
    virtual const CLID& clID() const;
    static const CLID& classID();
  
    /// Fill the ASCII output stream
   virtual std::ostream& fillStream(std::ostream& s) const;
  
    /// Mean number of interactions per event (nu)
    double nu() const;
  
    /// emittance
    double emittance() const;
  
    /// sigma X of the luminous region
    double sigmaX() const;
  
    /// sigma Y of the luminous region
    double sigmaY() const;
  
    /// sigma Z of the luminous region
    double sigmaZ() const;
  
    /// Smearing of the angle between the beams
    double angleSmear() const;
  
    /// Retrieve const  Beam energy
    double energy() const;
  
    /// Update  Beam energy
    void setEnergy(double value);
  
    /// Retrieve const  RMS of the bunch length
    double sigmaS() const;
  
    /// Update  RMS of the bunch length
    void setSigmaS(double value);
  
    /// Retrieve const  Normalized emittance
    double epsilonN() const;
  
    /// Update  Normalized emittance
    void setEpsilonN(double value);
  
    /// Retrieve const  Revolution frequency
    double revolutionFrequency() const;
  
    /// Retrieve const  Total cross-section
    double totalXSec() const;
  
    /// Update  Total cross-section
    void setTotalXSec(double value);
  
    /// Retrieve const  Horizontal crossing angle
    double horizontalCrossingAngle() const;
  
    /// Update  Horizontal crossing angle
    void setHorizontalCrossingAngle(double value);
  
    /// Retrieve const  Vertical crossing angle
    double verticalCrossingAngle() const;
  
    /// Update  Vertical crossing angle
    void setVerticalCrossingAngle(double value);
  
    /// Retrieve const  Horizontal beamline angle
    double horizontalBeamlineAngle() const;
  
    /// Update  Horizontal beamline angle
    void setHorizontalBeamlineAngle(double value);
  
    /// Retrieve const  Vertical beamline angle
    double verticalBeamlineAngle() const;
  
    /// Update  Vertical beamline angle
    void setVerticalBeamlineAngle(double value);
  
    /// Retrieve const  Beta star
    double betaStar() const;
  
    /// Update  Beta star
    void setBetaStar(double value);
  
    /// Retrieve const  Bunch spacing
    double bunchSpacing() const;
  
    /// Update  Bunch spacing
    void setBunchSpacing(double value);
  
    /// Retrieve const  Luminous region mean position
    const Gaudi::XYZPoint& beamSpot() const;
  
    /// Update  Luminous region mean position
    void setBeamSpot(const Gaudi::XYZPoint& value);
  
    /// Retrieve const  Luminosity
    double luminosity() const;
  
    /// Update  Luminosity
    void setLuminosity(double value);
  
  
  #ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      using pool = boost::singleton_pool< BeamParameters, sizeof(BeamParameters),
                                          boost::default_user_allocator_new_delete,
                                          boost::details::pool::null_mutex, 128 >;
      return ( sizeof(BeamParameters) == size ? pool::malloc() : ::operator new(size) );
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      using pool = boost::singleton_pool< BeamParameters, sizeof(BeamParameters),
                                          boost::default_user_allocator_new_delete,
                                          boost::details::pool::null_mutex, 128 >;
      pool::is_from(p) ? pool::free(p) : ::operator delete(p);
    }
  
    /// placement operator new
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new ( size, pObj );
    }
  
    /// placement operator delete
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete ( p, pObj );
    }
  #endif
  protected:

  private:

    double          m_energy;                  ///< Beam energy
    double          m_sigmaS;                  ///< RMS of the bunch length
    double          m_epsilonN;                ///< Normalized emittance
    double          m_revolutionFrequency;     ///< Revolution frequency
    double          m_totalXSec;               ///< Total cross-section
    double          m_horizontalCrossingAngle; ///< Horizontal crossing angle
    double          m_verticalCrossingAngle;   ///< Vertical crossing angle
    double          m_horizontalBeamlineAngle; ///< Horizontal beamline angle
    double          m_verticalBeamlineAngle;   ///< Vertical beamline angle
    double          m_betaStar;                ///< Beta star
    double          m_bunchSpacing;            ///< Bunch spacing
    Gaudi::XYZPoint m_beamSpot;                ///< Luminous region mean position
    double          m_luminosity;              ///< Luminosity
  
  }; // class BeamParameters

  inline std::ostream& operator<< (std::ostream& str, const BeamParameters& obj)
  {
    return obj.fillStream(str);
  }
  
} // namespace LHCb;

// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& LHCb::BeamParameters::clID() const
{
  return LHCb::BeamParameters::classID();
}

inline const CLID& LHCb::BeamParameters::classID()
{
  return CLID_BeamParameters;
}

inline std::ostream& LHCb::BeamParameters::fillStream(std::ostream& s) const
{
  s << "{ " << "energy :	" << (float)m_energy << std::endl
            << "sigmaS :	" << (float)m_sigmaS << std::endl
            << "epsilonN :	" << (float)m_epsilonN << std::endl
            << "revolutionFrequency :	" << (float)m_revolutionFrequency << std::endl
            << "totalXSec :	" << (float)m_totalXSec << std::endl
            << "horizontalCrossingAngle :	" << (float)m_horizontalCrossingAngle << std::endl
            << "verticalCrossingAngle :	" << (float)m_verticalCrossingAngle << std::endl
            << "horizontalBeamlineAngle :	" << (float)m_horizontalBeamlineAngle << std::endl
            << "verticalBeamlineAngle :	" << (float)m_verticalBeamlineAngle << std::endl
            << "betaStar :	" << (float)m_betaStar << std::endl
            << "bunchSpacing :	" << (float)m_bunchSpacing << std::endl
            << "beamSpot :	" << m_beamSpot << std::endl
            << "luminosity :	" << (float)m_luminosity << std::endl << " }";
  return s;
}


inline double LHCb::BeamParameters::energy() const 
{
  return m_energy;
}

inline void LHCb::BeamParameters::setEnergy(double value) 
{
  m_energy = value;
}

inline double LHCb::BeamParameters::sigmaS() const 
{
  return m_sigmaS;
}

inline void LHCb::BeamParameters::setSigmaS(double value) 
{
  m_sigmaS = value;
}

inline double LHCb::BeamParameters::epsilonN() const 
{
  return m_epsilonN;
}

inline void LHCb::BeamParameters::setEpsilonN(double value) 
{
  m_epsilonN = value;
}

inline double LHCb::BeamParameters::revolutionFrequency() const 
{
  return m_revolutionFrequency;
}

inline double LHCb::BeamParameters::totalXSec() const 
{
  return m_totalXSec;
}

inline void LHCb::BeamParameters::setTotalXSec(double value) 
{
  m_totalXSec = value;
}

inline double LHCb::BeamParameters::horizontalCrossingAngle() const 
{
  return m_horizontalCrossingAngle;
}

inline void LHCb::BeamParameters::setHorizontalCrossingAngle(double value) 
{
  m_horizontalCrossingAngle = value;
}

inline double LHCb::BeamParameters::verticalCrossingAngle() const 
{
  return m_verticalCrossingAngle;
}

inline void LHCb::BeamParameters::setVerticalCrossingAngle(double value) 
{
  m_verticalCrossingAngle = value;
}

inline double LHCb::BeamParameters::horizontalBeamlineAngle() const 
{
  return m_horizontalBeamlineAngle;
}

inline void LHCb::BeamParameters::setHorizontalBeamlineAngle(double value) 
{
  m_horizontalBeamlineAngle = value;
}

inline double LHCb::BeamParameters::verticalBeamlineAngle() const 
{
  return m_verticalBeamlineAngle;
}

inline void LHCb::BeamParameters::setVerticalBeamlineAngle(double value) 
{
  m_verticalBeamlineAngle = value;
}

inline double LHCb::BeamParameters::betaStar() const 
{
  return m_betaStar;
}

inline void LHCb::BeamParameters::setBetaStar(double value) 
{
  m_betaStar = value;
}

inline double LHCb::BeamParameters::bunchSpacing() const 
{
  return m_bunchSpacing;
}

inline void LHCb::BeamParameters::setBunchSpacing(double value) 
{
  m_bunchSpacing = value;
}

inline const Gaudi::XYZPoint& LHCb::BeamParameters::beamSpot() const 
{
  return m_beamSpot;
}

inline void LHCb::BeamParameters::setBeamSpot(const Gaudi::XYZPoint& value) 
{
  m_beamSpot = value;
}

inline double LHCb::BeamParameters::luminosity() const 
{
  return m_luminosity;
}

inline void LHCb::BeamParameters::setLuminosity(double value) 
{
  m_luminosity = value;
}

inline double LHCb::BeamParameters::nu() const 
{

          return luminosity() * totalXSec() / revolutionFrequency() ;
        
}

inline double LHCb::BeamParameters::emittance() const 
{

	  double beta = sqrt( energy() * energy() - 938.272013 * Gaudi::Units::MeV * 938.272013 * Gaudi::Units::MeV ) / energy() ; 
	  double gamma = 1./sqrt(1. - beta * beta ) ; 
	  return epsilonN()/gamma/beta ;
	
}

inline double LHCb::BeamParameters::sigmaX() const 
{

          return sqrt( betaStar() * emittance() / 2. ) ;
        
}

inline double LHCb::BeamParameters::sigmaY() const 
{

          return sqrt( betaStar() * emittance() /2. ) ;
        
}

inline double LHCb::BeamParameters::sigmaZ() const 
{

          return sigmaS() / sqrt(2.) ;
        
}

inline double LHCb::BeamParameters::angleSmear() const 
{

          if ( 0.0 != betaStar() ) return ( emittance() / betaStar() ) ;
          else return 0.0 ;
        
}



#endif ///GenEvent_BeamParameters_H
