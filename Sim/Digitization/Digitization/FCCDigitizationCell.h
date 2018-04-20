#ifndef DIGITIZATION_FCCDIGITIZATIONCELL_H
#define DIGITIZATION_FCCDIGITIZATIONCELL_H

#include "ACTS/Digitization/DigitizationCell.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "datamodel/TrackHit.h"

/** Some usefull structs for Digitization
 *
 *
 *  @author Julia Hrdinka
 *  @date   2018-02
 */

namespace sim {

/// @struct FCCDigitizationCell
/// @brief wrapper of Acts::DigitizationCell to store hit information
struct FCCDigitizationCell : Acts::DigitizationCell {
  /// connection to FCC track hits
  std::vector<fcc::TrackHit> trackHits;
  /// constructor
  /// @param the FCC track hits of the digitization cell
  /// @param ch0 first channel number
  /// @param ch1 second channel number
  /// @param d data of cell (energy for analogue readout, 1 for digital readout)
  FCCDigitizationCell(std::vector<fcc::TrackHit> th, size_t ch0, size_t ch1, float d = 0.)
      : DigitizationCell(ch0, ch1, d), trackHits(th) {}
  // copy constructor
  FCCDigitizationCell(const FCCDigitizationCell& dc) : DigitizationCell(dc), trackHits(dc.trackHits) {}
  // to merge cells in case they are at the same position
  void addCell(const FCCDigitizationCell& dc, bool analogueReadout) {
    for (auto& th : dc.trackHits)
      trackHits.push_back(th);
    if (analogueReadout) data += dc.data;
  }
};

/// Definition of measurement
template <Acts::ParID_t... params>
using Measurement_t = Acts::Measurement<Identifier, params...>;
/// @struct FCCPlanarCluster
/// @brief an FCC measurement - needed for studies
struct FCCPlanarCluster : Acts::Measurement_t<Acts::ParDef::eLOC_0, Acts::ParDef::eLOC_1> {
  /// the energy deposited in the cluster
  double energy;
  /// the number of track contributing to this cluster
  size_t nTracks;

  /// constructor
  /// @param en the energy deposited in the cluster
  /// @param ntracks the number of track contributing to this cluster
  /// @param mSurface the corresponding surface
  /// @param cIdentifier the channel identifier
  /// @param cov the covariances of the measurement
  /// @param loc0 the local position of the cluster - first coordinate
  /// @param loc1 the local position of the cluster - second coordinate
  /// @param dCells the cells contributing to the cluster
  FCCPlanarCluster(const double& en, const size_t& ntracks, const Acts::Surface& mSurface,
                   const Identifier& cIdentifier, Acts::ActsSymMatrixD<2> cov, double loc0, double loc1,
                   std::vector<FCCDigitizationCell> dCells)
      : Acts::Measurement_t<Acts::ParDef::eLOC_0, Acts::ParDef::eLOC_1>(mSurface, cIdentifier, std::move(cov), loc0,
                                                                        loc1),
        energy(en),
        nTracks(ntracks),
        m_digitizationCells(dCells) {}

  /// access to the digitization cells
  /// @return the vector to the digitization cells
  const std::vector<FCCDigitizationCell>& digitizationCells() const { return m_digitizationCells; }

private:
  /// the fcc digitization cells
  std::vector<FCCDigitizationCell> m_digitizationCells;
};
}
#endif  // DIGITIZATION_FCCDIGITIZATIONCELL_H
