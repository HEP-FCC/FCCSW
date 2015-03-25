#ifndef ParameterRESOLUTION__H
#define ParameterRESOLUTION__H

#include "AtlfastBinID.hh"
#include "G4Track.hh"

namespace Atlfast
{
  class ParameterResolutions
  {
    public:

      ParameterResolutions() {}
      ParameterResolutions(std::vector<BinID>& bins, double low, double high)
        : m_coefficientBins(bins), m_etaLow(low), m_etaHigh(high)
      {
        m_powerSeries.push_back(0);
        m_powerSeries.push_back(0.5);
        m_powerSeries.push_back(1);
        m_powerSeries.push_back(1.5);
        m_powerSeries.push_back(2);
      }

      double resolution(const G4Track&) const;

    private:

      double interpolate(double, double, double) const;
      double calculateResolution(double, const std::vector<double>&) const;

      std::vector<BinID> m_coefficientBins;
      std::vector<double> m_powerSeries;
      double m_etaLow;
      double m_etaHigh;
  };

}

#endif
