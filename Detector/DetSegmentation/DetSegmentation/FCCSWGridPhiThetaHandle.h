#ifndef DD4HEP_DDCORE_GRIDPHITHETA_H
#define DD4HEP_DDCORE_GRIDPHITHETA_H 1

// FCCSW
#include "DetSegmentation/FCCSWGridPhiTheta.h"

// DD4hep
#include "DD4hep/Segmentations.h"
#include "DD4hep/detail/SegmentationsInterna.h"

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

/// Namespace for base segmentations


// Forward declarations
class Segmentation;
template <typename T>
class SegmentationWrapper;

/// We need some abbreviation to make the code more readable.
typedef Handle<SegmentationWrapper<DDSegmentation::FCCSWGridPhiTheta>> FCCSWGridPhiThetaHandle;

/// Implementation class for the grid phi-theta segmentation.
/**
 *  Concrete user handle to serve specific needs of client code
 *  which requires access to the base functionality not served
 *  by the super-class Segmentation.
 *
 *  Note:
 *  We only check the validity of the underlying handle.
 *  If for whatever reason the implementation object is not valid
 *  This is not checked.
 *  In principle this CANNOT happen unless some brain-dead has
 *  fiddled with the handled object directly.....
 *
 *  Note:
 *  The handle base corrsponding to this object in for
 *  conveniance reasons instantiated in DD4hep/src/Segmentations.cpp.
 *
 *  \author  A. Zaborowska
 *  \version 1.0
 */
class FCCSWGridPhiTheta : public FCCSWGridPhiThetaHandle {
public:
  /// Defintiion of the basic handled object
  typedef FCCSWGridPhiThetaHandle::Object Object;

public:
  /// Default constructor
  FCCSWGridPhiTheta() = default;
  /// Copy constructor
  FCCSWGridPhiTheta(const FCCSWGridPhiTheta& e) = default;
  /// Copy Constructor from segmentation base object
  FCCSWGridPhiTheta(const Segmentation& e) : Handle<Object>(e) {}
  /// Copy constructor from handle
  FCCSWGridPhiTheta(const Handle<Object>& e) : Handle<Object>(e) {}
  /// Copy constructor from other polymorph/equivalent handle
  template <typename Q>
  FCCSWGridPhiTheta(const Handle<Q>& e) : Handle<Object>(e) {}
  /// Assignment operator
  FCCSWGridPhiTheta& operator=(const FCCSWGridPhiTheta& seg) = default;
  /// Equality operator
  bool operator==(const FCCSWGridPhiTheta& seg) const { return m_element == seg.m_element; }
  /// determine the position based on the cell ID
  inline Position position(const CellID& id) const { return Position(access()->implementation->position(id)); }

  /// determine the cell ID based on the position
  inline dd4hep::CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const {
    return access()->implementation->cellID(local, global, volID);
  }

  /// access the grid size in theta
  inline double gridSizeTheta() const { return access()->implementation->gridSizeTheta(); }

  /// access the grid size in Phi
  inline int phiBins() const { return access()->implementation->phiBins(); }

  /// access the coordinate offset in theta
  inline double offsetTheta() const { return access()->implementation->offsetTheta(); }

  /// access the coordinate offset in Phi
  inline double offsetPhi() const { return access()->implementation->offsetPhi(); }

  /// set the coordinate offset in theta
  inline void setOffsetTheta(double offset) const { access()->implementation->setOffsetTheta(offset); }

  /// set the coordinate offset in Phi
  inline void setOffsetPhi(double offset) const { access()->implementation->setOffsetPhi(offset); }

  /// set the grid size in theta
  inline void setGridSizeTheta(double cellSize) const { access()->implementation->setGridSizeTheta(cellSize); }

  /// set the grid size in Phi
  inline void setPhiBins(int cellSize) const { access()->implementation->setPhiBins(cellSize); }

  /// access the field name used for theta
  inline const std::string& fieldNameTheta() const { return access()->implementation->fieldNameTheta(); }

  /// access the field name used for Phi
  inline const std::string& fieldNamePhi() const { return access()->implementation->fieldNamePhi(); }

  /** \brief Returns a std::vector<double> of the cellDimensions of the given cell ID
      in natural order of dimensions (dPhi, dTheta)

      Returns a std::vector of the cellDimensions of the given cell ID
      \param cellID is ignored as all cells have the same dimension
      \return std::vector<double> size 2:
      -# size in phi
      -# size in theta
  */
  inline std::vector<double> cellDimensions(const CellID& /*id*/) const {
    return {access()->implementation->gridSizePhi(), access()->implementation->gridSizeTheta()};
  }
};

} /* End namespace dd4hep                */
#endif  // DD4HEP_DDCORE_GRIDPHITHETA_H
