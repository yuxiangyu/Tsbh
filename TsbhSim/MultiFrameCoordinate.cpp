
#include <cassert>
#include "MultiFrameCoordinate.h"
#include "CoordinateConverter.h"



MultiFrameCoordinate::MultiFrameCoordinate()
  : llaValid_(false),
    ecefValid_(false)
{
}

MultiFrameCoordinate::MultiFrameCoordinate(const Coordinate& coordinate)
  : llaValid_(false),
    ecefValid_(false)
{
  setCoordinate(coordinate);
}

MultiFrameCoordinate::~MultiFrameCoordinate()
{
}

int MultiFrameCoordinate::setCoordinate(const Coordinate& coordinate)
{
  if (coordinate.coordinateSystem() == COORD_SYS_LLA)
  {
    llaValid_ = true;
    ecefValid_ = false;
    llaCoordinate_ = coordinate;
    ecefCoordinate_.clear();
    return 0;
  }
  else if (coordinate.coordinateSystem() == COORD_SYS_ECEF)
  {
    llaValid_ = false;
    ecefValid_ = true;
    llaCoordinate_.clear();
    ecefCoordinate_ = coordinate;
    return 0;
  }

  // Error, not a valid system
  clear();
  return 1;
}

int MultiFrameCoordinate::setCoordinate(const Coordinate& coordinate, const CoordinateConverter& converter)
{
  // Check for case where coord system passed in is unnecessary
  if (coordinate.coordinateSystem() == COORD_SYS_ECEF || coordinate.coordinateSystem() ==COORD_SYS_LLA)
    return setCoordinate(coordinate);

  // Check for invalid coordinate and invalid CC
  if (coordinate.coordinateSystem() == COORD_SYS_NONE || !converter.hasReferenceOrigin())
  {
    clear();
    // Error, invalid coordinate or converter
    return 1;
  }

  // Convert the coordinate and pass in using setCoordinate
  Coordinate llaValues;
  converter.convert(coordinate, llaValues, COORD_SYS_LLA);
  return setCoordinate(llaValues);
}

void MultiFrameCoordinate::clear()
{
  llaValid_ = false;
  ecefValid_ = false;
  llaCoordinate_.clear();
  ecefCoordinate_.clear();
}

bool MultiFrameCoordinate::isValid() const
{
  return llaValid_ || ecefValid_;
}

const Coordinate& MultiFrameCoordinate::llaCoordinate() const
{
  // Need to convert from ECEF into LLA, if ECEF is valid but LLA is not
  if (ecefValid_ && !llaValid_)
  {
    CoordinateConverter::convertEcefToGeodetic(ecefCoordinate_, llaCoordinate_);
    llaValid_ = true;
  }
  // Return the LLA value, either valid or not
  return llaCoordinate_;
}

const Coordinate& MultiFrameCoordinate::ecefCoordinate() const
{
  // Need to convert from LLA into ECEF, if LLA is valid but ECEF is not
  if (llaValid_ && !ecefValid_)
  {
    CoordinateConverter::convertGeodeticToEcef(llaCoordinate_, ecefCoordinate_);
    ecefValid_ = true;
  }
  // Return the ECEF value, either valid or not
  return ecefCoordinate_;
}


