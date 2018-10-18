#ifndef DIGITIZATION_FCCDIGITIZATIONCELL_H
#define DIGITIZATION_FCCDIGITIZATIONCELL_H

#include "ACTS/Digitization/DigitizationCell.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "datamodel/TrackHit.h"

/** Some usefull structs for digitization to wrap the Acts::Implementation with an fcc specific one.
 *
 *
 *  @author Julia Hrdinka
 *  @date   2018-02
 */

namespace sim {

/// @struct FCCDigitizationCell
/// @brief wrapper of Acts::DigitizationCell to store hit information from fcc
struct FCCDigitizationCell : Acts::DigitizationCell {
  /// track IDs of this cell
  std::vector<unsigned> trackIDs;
  /// time of this cell
  float time = 0;
  /// count how often this cell was hit in the same event
  size_t mergedCellsCount = 0;
  /// constructor
  /// @param the FCC track hits of the digitization cell
  /// @param ch0 first channel number
  /// @param ch1 second channel number
  /// @param d data of cell (energy for analogue readout, 1 for digital readout)
  FCCDigitizationCell(std::vector<unsigned> TrackIDs, float Time, size_t Channel0, size_t Channel1, float Data = 0.)
      : DigitizationCell(Channel0, Channel1, Data), trackIDs(TrackIDs), time(Time) {}
  // copy constructor
  FCCDigitizationCell(const FCCDigitizationCell& dc)
      : DigitizationCell(dc), trackIDs(dc.trackIDs), time(dc.time), mergedCellsCount(dc.mergedCellsCount) {}
  // to merge cells in case they are at the same position
  void addCell(const FCCDigitizationCell& dc) {
    for (auto& tID : dc.trackIDs)
      trackIDs.push_back(tID);
    data += dc.data;
    time += dc.time;
    mergedCellsCount++;
  }
  // @todo remove readout boolean in acts
  /// total deposited energy in this cell
  float depositedEnergy() const { return data; }
  /// averaged time of this cell
  float averagedTime() const { return ((mergedCellsCount > 1) ? (time / mergedCellsCount) : time); }
};

/// Definition of measurement
template <Acts::ParID_t... params>
using Measurement_t = Acts::Measurement<Identifier, params...>;
/// @struct FCCPlanarCluster
/// @brief an FCC measurement - needed for studies
struct FCCPlanarCluster : Acts::Measurement_t<Acts::ParDef::eLOC_0, Acts::ParDef::eLOC_1> {
  /// the energy deposited in the cluster
  float energy;
  /// the time information of the cluster
  float time;
  /// the tracks IDs contributing to this cluster
  std::vector<unsigned> trackIDs;

  /// constructor
  /// @param en the energy deposited in the cluster
  /// @param ntracks the number of track contributing to this cluster
  /// @param mSurface the corresponding surface
  /// @param cIdentifier the channel identifier
  /// @param cov the covariances of the measurement
  /// @param loc0 the local position of the cluster - first coordinate
  /// @param loc1 the local position of the cluster - second coordinate
  /// @param dCells the cells contributing to the cluster
  FCCPlanarCluster(float Energy, float Time, const std::vector<unsigned>& TrackIDs, const Acts::Surface& mSurface,
                   const Identifier& cIdentifier, Acts::ActsSymMatrixD<2> cov, double loc0, double loc1,
                   std::vector<FCCDigitizationCell> dCells)
      : Acts::Measurement_t<Acts::ParDef::eLOC_0, Acts::ParDef::eLOC_1>(mSurface, cIdentifier, std::move(cov), loc0,
                                                                        loc1),
        energy(Energy),
        time(Time),
        trackIDs(TrackIDs),
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
