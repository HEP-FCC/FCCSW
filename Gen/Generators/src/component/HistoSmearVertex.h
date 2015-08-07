#ifndef HISTOSMEARVERTEX_H
#define HISTOSMEARVERTEX_H 1

// Include files
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IVertexSmearingTool.h"
#include "TH3.h"

/** @class SmearVertex SmearVertex.h "SmearVertex.h"
 *
 *  Tool to smear vertex with external input distribution.
 *  Concrete implementation of a IVertexSmearingTool.
 *
 *  @author Dominik Mitzel
 *  @date   2014-08-02
 */
class HistoSmearVertex : public GaudiTool, virtual public IVertexSmearingTool {
 public:
  /// Standard constructor
  HistoSmearVertex( const std::string& type , const std::string& name,
                    const IInterface* parent ) ;
  virtual ~HistoSmearVertex( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( );

  /** Implements IVertexSmearingTool::smearVertex.
   */
  virtual StatusCode smearVertex( LHCb::HepMCEvent * theEvent );

 private:
  /// Direction of the beam to take into account TOF vs nominal IP8, can have
  /// only values -1 or 1, or 0 to switch off the TOF and set time of
  /// interaction to zero (default = 1, as for beam 1)
  int m_zDir;
  TH3* m_hist;
  std::string m_inputFileName;
  std::string m_histoPath;
};
#endif // PARTICLEGUNS_SMEARVERTEX_H
