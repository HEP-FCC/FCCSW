#include "InterpolatedBFieldSvc.h"
#include "ACTS/MagneticField/concept/AnyFieldLookup.hpp"
#include "ACTS/Utilities/BFieldMapUtils.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ACTS/Utilities/detail/Axis.hpp"
#include "ACTS/Utilities/detail/Grid.hpp"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include <fstream>
#include <utility>

DECLARE_SERVICE_FACTORY(InterpolatedBFieldSvc)

InterpolatedBFieldSvc::InterpolatedBFieldSvc(const std::string& name, ISvcLocator* svc)
    : base_class(name, svc), m_actsBField(nullptr) {}

StatusCode InterpolatedBFieldSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  // The file type
  // Detector file type
  FileType fileType = FileType::root;
  if (m_fieldMapFile.value().find(".root") != std::string::npos) {
    debug() << " root format for magnetic field map detected" << endmsg;
  } else if (m_fieldMapFile.value().find(".txt") != std::string::npos ||
             m_fieldMapFile.value().find(".csv") != std::string::npos) {
    debug() << "txt format for magnetic field map detected" << std::endl;
    fileType = FileType::text;
  } else {
    error() << "magnetic field format could not be detected  use '.root', '.txt', or '.csv'." << endmsg;
    return StatusCode::FAILURE;
  }

  // Declare the mapper
  Acts::concept::AnyFieldLookup<> mapper;
  double lengthUnit = m_lengthScalor * Acts::units::_mm;
  double bFieldUnit = m_bFieldScalor * Acts::units::_T;
  // set the mapper
  if (fileType == FileType::root) {
    // - root
    if (m_cylinderCoordinates) {
      // cylinderCoordinates
      mapper = fieldMapperRZ_root(
          [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
            return (binsRZ.at(1) * nBinsRZ.at(0) + binsRZ.at(0));
          },
          m_fieldMapFile,
          m_treeName,
          lengthUnit,
          bFieldUnit,
          m_firstOctant);
    } else {
      // cartesian coordinates
      mapper = fieldMapperXYZ_root(
          [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
            return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2)) + binsXYZ.at(1) * nBinsXYZ.at(2) + binsXYZ.at(2));
          },
          m_fieldMapFile,
          m_treeName,
          lengthUnit,
          bFieldUnit,
          m_firstOctant);
    }
  } else {
    // - txt/csv
    if (m_cylinderCoordinates) {
      // cylinderCoordinates
      // cylinderCoordinates
      mapper = fieldMapperRZ_txt(
          [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
            return (binsRZ.at(1) * nBinsRZ.at(0) + binsRZ.at(0));
          },
          m_fieldMapFile,
          lengthUnit,
          bFieldUnit,
          m_nPoints,
          m_firstOctant);

    } else {
      // cartesian coordinates
      mapper = fieldMapperXYZ_txt(
          [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
            return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2)) + binsXYZ.at(1) * nBinsXYZ.at(2) + binsXYZ.at(2));
          },
          m_fieldMapFile, lengthUnit, bFieldUnit, m_nPoints, m_firstOctant);
    }
  }
  // configure the ACTS InterpolatedBField
  auto bFieldConfig = Acts::InterpolatedBFieldMap::Config();
  bFieldConfig.scale = 1;
  bFieldConfig.mapper = std::move(mapper);

  // set the interpolated magnetic field of the ACTS magnetic field service

  m_actsBField = std::make_unique<Acts::InterpolatedBFieldMap>(bFieldConfig);

  if (!m_actsBField) {
    error() << "Unable to initialize InterpolatedBFieldSvc!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

Acts::Vector3D InterpolatedBFieldSvc::getField(const Acts::Vector3D& position) const {
  return m_actsBField->getField(position);
}

Acts::concept::AnyFieldCell<> InterpolatedBFieldSvc::getFieldCell(const Acts::Vector3D& position) const {
  return m_actsBField->getFieldCell(position);
}

Acts::Vector3D InterpolatedBFieldSvc::getFieldGradient(const Acts::Vector3D& pos,
                                                       Acts::ActsMatrixD<3, 3>& deriv) const {
  return m_actsBField->getFieldGradient(pos, deriv);
}

StatusCode InterpolatedBFieldSvc::finalize() { return StatusCode::SUCCESS; }

Acts::InterpolatedBFieldMap::FieldMapper<2, 2> InterpolatedBFieldSvc::fieldMapperRZ_txt(
    std::function<size_t(std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ)> localToGlobalBin,
    std::string fieldMapFile,
    double lengthUnit,
    double BFieldUnit,
    size_t nPoints,
    bool firstQuadrant) {
  /// [1] Read in field map file
  // Grid position points in r and z
  std::vector<double> rPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector2D> bField;
  // reserve estimated size
  rPos.reserve(nPoints);
  zPos.reserve(nPoints);
  bField.reserve(nPoints);
  // [1] Read in file and fill values
  debug() << "Opening file: " << fieldMapFile << endmsg;
  std::ifstream map_file(fieldMapFile.c_str(), std::ios::in);
  std::string line;
  double r = 0., z = 0.;
  double br = 0., bz = 0.;
  while (std::getline(map_file, line)) {
    if (line.empty() || line[0] == '%' || line[0] == '#' || line.find_first_not_of(' ') == std::string::npos) continue;

    std::istringstream tmp(line);
    tmp >> r >> z >> br >> bz;
    rPos.push_back(r);
    zPos.push_back(z);
    bField.push_back(Acts::Vector2D(br, bz));
  }
  map_file.close();
  /// [2] use helper function in core
  return fieldMapperRZ(localToGlobalBin, rPos, zPos, bField, lengthUnit, BFieldUnit, firstQuadrant);
}

Acts::InterpolatedBFieldMap::FieldMapper<3, 3> InterpolatedBFieldSvc::fieldMapperXYZ_txt(
    std::function<size_t(std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ)> localToGlobalBin,
    std::string fieldMapFile,
    double lengthUnit,
    double BFieldUnit,
    size_t nPoints,
    bool firstOctant) {
  /// [1] Read in field map file
  // Grid position points in x, y and z
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector3D> bField;
  // reserve estimated size
  xPos.reserve(nPoints);
  yPos.reserve(nPoints);
  zPos.reserve(nPoints);
  bField.reserve(nPoints);
  // [1] Read in file and fill values
  debug() << "Opening file: " << fieldMapFile << endmsg;
  std::ifstream map_file(fieldMapFile.c_str(), std::ios::in);
  std::string line;
  double x = 0., y = 0., z = 0.;
  double bx = 0., by = 0., bz = 0.;
  while (std::getline(map_file, line)) {
    if (line.empty() || line[0] == '%' || line[0] == '#' || line.find_first_not_of(' ') == std::string::npos) continue;

    std::istringstream tmp(line);
    tmp >> x >> y >> z >> bx >> by >> bz;
    xPos.push_back(x);
    yPos.push_back(y);
    zPos.push_back(z);
    bField.push_back(Acts::Vector3D(bx, by, bz));
  }
  map_file.close();

  return fieldMapperXYZ(localToGlobalBin, xPos, yPos, zPos, bField, lengthUnit, BFieldUnit, firstOctant);
}

bool InterpolatedBFieldSvc::isInside(const Acts::Vector3D& position) const { return m_actsBField->isInside(position); }

Acts::InterpolatedBFieldMap::FieldMapper<2, 2> InterpolatedBFieldSvc::fieldMapperRZ_root(
    std::function<size_t(std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ)> localToGlobalBin,
    std::string fieldMapFile,
    std::string treeName,
    double lengthUnit,
    double BFieldUnit,
    bool firstQuadrant) {
  /// [1] Read in field map file
  // Grid position points in r and z
  std::vector<double> rPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector2D> bField;
  // [1] Read in file and fill values
  debug() << "Opening file: " << fieldMapFile << endmsg;
  std::unique_ptr<TFile> inputFile(TFile::Open(fieldMapFile.c_str()));
  TTree* tree = (TTree*)inputFile->Get(treeName.c_str());
  Int_t entries = tree->GetEntries();

  double r, z;
  double Br, Bz;

  tree->SetBranchAddress("r", &r);
  tree->SetBranchAddress("z", &z);

  tree->SetBranchAddress("Br", &Br);
  tree->SetBranchAddress("Bz", &Bz);

  // reserve size
  rPos.reserve(entries);
  zPos.reserve(entries);
  bField.reserve(entries);

  for (int i = 0; i < entries; i++) {
    tree->GetEvent(i);
    rPos.push_back(r);
    zPos.push_back(z);
    bField.push_back(Acts::Vector2D(Br, Bz));
  }
  inputFile->Close();
  /// [2] use helper function in core
  return fieldMapperRZ(localToGlobalBin, rPos, zPos, bField, lengthUnit, BFieldUnit, firstQuadrant);
}

Acts::InterpolatedBFieldMap::FieldMapper<3, 3> InterpolatedBFieldSvc::fieldMapperXYZ_root(
    std::function<size_t(std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ)> localToGlobalBin,
    std::string fieldMapFile,
    std::string treeName,
    double lengthUnit,
    double BFieldUnit,
    bool firstOctant) {
  /// [1] Read in field map file
  // Grid position points in x, y and z
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  // components of magnetic field on grid points
  std::vector<Acts::Vector3D> bField;
  // [1] Read in file and fill values
  debug() << "Opening file: " << fieldMapFile << endmsg;
  std::unique_ptr<TFile> inputFile(TFile::Open(fieldMapFile.c_str()));
  TTree* tree = (TTree*)inputFile->Get(treeName.c_str());
  Int_t entries = tree->GetEntries();

  double x, y, z;
  double Bx, By, Bz;

  tree->SetBranchAddress("x", &x);
  tree->SetBranchAddress("y", &y);
  tree->SetBranchAddress("z", &z);

  tree->SetBranchAddress("Bx", &Bx);
  tree->SetBranchAddress("By", &By);
  tree->SetBranchAddress("Bz", &Bz);

  // reserve size
  xPos.reserve(entries);
  yPos.reserve(entries);
  zPos.reserve(entries);
  bField.reserve(entries);

  for (int i = 0; i < entries; i++) {
    tree->GetEvent(i);
    xPos.push_back(x);
    yPos.push_back(y);
    zPos.push_back(z);
    bField.push_back(Acts::Vector3D(Bx, By, Bz));
  }
  inputFile->Close();

  return fieldMapperXYZ(localToGlobalBin, xPos, yPos, zPos, bField, lengthUnit, BFieldUnit, firstOctant);
}

Acts::InterpolatedBFieldMap::FieldMapper<2, 2> InterpolatedBFieldSvc::fieldMapperRZ(
    std::function<size_t(std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ)> localToGlobalBin,
    std::vector<double>
        rPos,
    std::vector<double>
        zPos,
    std::vector<Acts::Vector2D>
        bField,
    double lengthUnit,
    double BFieldUnit,
    bool firstQuadrant) {
  // [1] Create Grid
  // sort the values
  std::sort(rPos.begin(), rPos.end());
  std::sort(zPos.begin(), zPos.end());
  // Get unique values
  rPos.erase(std::unique(rPos.begin(), rPos.end()), rPos.end());
  zPos.erase(std::unique(zPos.begin(), zPos.end()), zPos.end());
  rPos.shrink_to_fit();
  zPos.shrink_to_fit();
  // get the number of bins
  size_t nBinsR = rPos.size();
  size_t nBinsZ = zPos.size();

  // get the minimum and maximum
  auto minMaxR = std::minmax_element(rPos.begin(), rPos.end());
  auto minMaxZ = std::minmax_element(zPos.begin(), zPos.end());
  double rMin = *minMaxR.first;
  double zMin = *minMaxZ.first;
  double rMax = *minMaxR.second;
  double zMax = *minMaxZ.second;
  // calculate maxima (add one last bin, because bin value always corresponds to
  // left boundary)
  double stepZ = std::fabs(zMax - zMin) / (nBinsZ - 1);
  double stepR = std::fabs(rMax - rMin) / (nBinsR - 1);
  rMax += stepR;
  zMax += stepZ;
  if (firstQuadrant) {
    zMin = -(*minMaxZ.second);
    nBinsZ = 2. * nBinsZ - 1;
  }

  // Create the axis for the grid
  Acts::detail::EquidistantAxis rAxis(rMin * lengthUnit, rMax * lengthUnit, nBinsR);
  Acts::detail::EquidistantAxis zAxis(zMin * lengthUnit, zMax * lengthUnit, nBinsZ);

  // Create the grid
  typedef Acts::detail::Grid<Acts::Vector2D, Acts::detail::EquidistantAxis, Acts::detail::EquidistantAxis> Grid_t;
  Grid_t grid(std::make_tuple(std::move(rAxis), std::move(zAxis)));
  // [2] Set the bField values

  for (size_t i = 1; i <= nBinsR; ++i) {
    for (size_t j = 1; j <= nBinsZ; ++j) {
      std::array<size_t, 2> nIndices = {{rPos.size(), zPos.size()}};
      Grid_t::index_t indices = {{i, j}};
      if (firstQuadrant) {
        // std::vectors begin with 0 and we do not want the user needing to
        // take underflow or overflow bins in account this is why we need to
        // subtract by one
        size_t n = std::abs(int(j) - int(zPos.size()));
        Grid_t::index_t indicesFirstQuadrant = {{i - 1, n}};

        grid.at(indices) = bField.at(localToGlobalBin(indicesFirstQuadrant, nIndices)) * BFieldUnit;
      } else {
        // std::vectors begin with 0 and we do not want the user needing to
        // take underflow or overflow bins in account this is why we need to
        // subtract by one
        grid.at(indices) = bField.at(localToGlobalBin({{i - 1, j - 1}}, nIndices)) * BFieldUnit;
      }
    }
  }
  // [3] Create the transformation for the position
  // map (x,y,z) -> (r,z)
  auto transformPos = [](const Acts::Vector3D& pos) { return Acts::Vector2D(pos.perp(), pos.z()); };

  // [4] Create the transformation for the bfield
  // map (Br,Bz) -> (Bx,By,Bz)
  auto transformBField = [](const Acts::Vector2D& field, const Acts::Vector3D& pos) {
    return Acts::Vector3D(field.x() * cos(pos.phi()), field.x() * sin(pos.phi()), field.y());
  };

  // [5] Create the mapper & BField Service
  // create field mapping
  return Acts::InterpolatedBFieldMap::FieldMapper<2, 2>(transformPos, transformBField, std::move(grid));
}

Acts::InterpolatedBFieldMap::FieldMapper<3, 3> InterpolatedBFieldSvc::fieldMapperXYZ(
    std::function<size_t(std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ)> localToGlobalBin,
    std::vector<double>
        xPos,
    std::vector<double>
        yPos,
    std::vector<double>
        zPos,
    std::vector<Acts::Vector3D>
        bField,
    double lengthUnit,
    double BFieldUnit,
    bool firstOctant) {
  // [1] Create Grid
  // Sort the values
  std::sort(xPos.begin(), xPos.end());
  std::sort(yPos.begin(), yPos.end());
  std::sort(zPos.begin(), zPos.end());
  // Get unique values
  xPos.erase(std::unique(xPos.begin(), xPos.end()), xPos.end());
  yPos.erase(std::unique(yPos.begin(), yPos.end()), yPos.end());
  zPos.erase(std::unique(zPos.begin(), zPos.end()), zPos.end());
  xPos.shrink_to_fit();
  yPos.shrink_to_fit();
  zPos.shrink_to_fit();
  // get the number of bins
  size_t nBinsX = xPos.size();
  size_t nBinsY = yPos.size();
  size_t nBinsZ = zPos.size();

  // get the minimum and maximum
  auto minMaxX = std::minmax_element(xPos.begin(), xPos.end());
  auto minMaxY = std::minmax_element(yPos.begin(), yPos.end());
  auto minMaxZ = std::minmax_element(zPos.begin(), zPos.end());
  // Create the axis for the grid
  // get minima
  double xMin = *minMaxX.first;
  double yMin = *minMaxY.first;
  double zMin = *minMaxZ.first;
  // get maxima
  double xMax = *minMaxX.second;
  double yMax = *minMaxY.second;
  double zMax = *minMaxZ.second;
  // calculate maxima (add one last bin, because bin value always corresponds to
  // left boundary)
  double stepZ = std::fabs(zMax - zMin) / (nBinsZ - 1);
  double stepY = std::fabs(yMax - yMin) / (nBinsY - 1);
  double stepX = std::fabs(xMax - xMin) / (nBinsX - 1);
  xMax += stepX;
  yMax += stepY;
  zMax += stepZ;

  // If only the first octant is given
  if (firstOctant) {
    xMin = -*minMaxX.second;
    yMin = -*minMaxY.second;
    zMin = -*minMaxZ.second;
    nBinsX = 2 * nBinsX - 1;
    nBinsY = 2 * nBinsY - 1;
    nBinsZ = 2 * nBinsZ - 1;
  }
  Acts::detail::EquidistantAxis xAxis(xMin * lengthUnit, xMax * lengthUnit, nBinsX);
  Acts::detail::EquidistantAxis yAxis(yMin * lengthUnit, yMax * lengthUnit, nBinsY);
  Acts::detail::EquidistantAxis zAxis(zMin * lengthUnit, zMax * lengthUnit, nBinsZ);
  // Create the grid
  typedef Acts::detail::Grid<Acts::Vector3D, Acts::detail::EquidistantAxis, Acts::detail::EquidistantAxis,
                             Acts::detail::EquidistantAxis>
      Grid_t;
  Grid_t grid(std::make_tuple(std::move(xAxis), std::move(yAxis), std::move(zAxis)));

  // [2] Set the bField values
  for (size_t i = 1; i <= nBinsX; ++i) {
    for (size_t j = 1; j <= nBinsY; ++j) {
      for (size_t k = 1; k <= nBinsZ; ++k) {
        Grid_t::index_t indices = {{i, j, k}};
        std::array<size_t, 3> nIndices = {{xPos.size(), yPos.size(), zPos.size()}};
        if (firstOctant) {
          // std::vectors begin with 0 and we do not want the user needing to
          // take underflow or overflow bins in account this is why we need to
          // subtract by one
          size_t m = std::abs(int(i) - (int(xPos.size())));
          size_t n = std::abs(int(j) - (int(yPos.size())));
          size_t l = std::abs(int(k) - (int(zPos.size())));
          Grid_t::index_t indicesFirstOctant = {{m, n, l}};

          grid.at(indices) = bField.at(localToGlobalBin(indicesFirstOctant, nIndices)) * BFieldUnit;

        } else {
          // std::vectors begin with 0 and we do not want the user needing to
          // take underflow or overflow bins in account this is why we need to
          // subtract by one
          grid.at(indices) = bField.at(localToGlobalBin({{i - 1, j - 1, k - 1}}, nIndices)) * BFieldUnit;
        }
      }
    }
  }
  // [3] Create the transformation for the position
  // map (x,y,z) -> (r,z)
  auto transformPos = [](const Acts::Vector3D& pos) { return pos; };
  // [4] Create the transformation for the bfield
  // map (Bx,By,Bz) -> (Bx,By,Bz)
  auto transformBField = [](const Acts::Vector3D& field, const Acts::Vector3D& /*pos*/) { return field; };

  // [5] Create the mapper & BField Service
  // create field mapping
  return Acts::InterpolatedBFieldMap::FieldMapper<3, 3>(transformPos, transformBField, std::move(grid));
}
