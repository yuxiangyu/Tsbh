
#include <cassert>
#include "Format1.h"
#include "CoordinateSystem.h"



// Coordinate system string constants, matching Rule Evaluation names for SIMDIS
static const std::string COORD_SYS_NED_STR = "Topo_NED";
static const std::string COORD_SYS_NWU_STR = "Topo_NWU";
static const std::string COORD_SYS_ENU_STR = "Topo_ENU";
static const std::string COORD_SYS_LLA_STR = "LLA_DD";
static const std::string COORD_SYS_ECEF_STR = "ECEF_WGS84";
static const std::string COORD_SYS_XEAST_STR = "TangentPlane_XEast";
static const std::string COORD_SYS_GTP_STR = "TangentPlane_Generic";
static const std::string COORD_SYS_ECI_STR = "ECI_WGS84";

// We will read these strings as "LLA", but do not write them
static const std::string COORD_SYS_LLA_DMD_STR = "LLA_DMD";
static const std::string COORD_SYS_LLA_DMS_STR = "LLA_DMS";

std::string coordinateSystemToString(CoordinateSystem coordSystem)
{
  switch (coordSystem)
  {
  case COORD_SYS_NED:   return COORD_SYS_NED_STR;
  case COORD_SYS_NWU:   return COORD_SYS_NWU_STR;
  case COORD_SYS_ENU:   return COORD_SYS_ENU_STR;
  case COORD_SYS_LLA:   return COORD_SYS_LLA_STR;
  case COORD_SYS_ECEF:  return COORD_SYS_ECEF_STR;
  case COORD_SYS_XEAST: return COORD_SYS_XEAST_STR;
  case COORD_SYS_GTP:   return COORD_SYS_GTP_STR;
  case COORD_SYS_ECI:   return COORD_SYS_ECI_STR;
  case COORD_SYS_NONE:
  case COORD_SYS_MAX:
    assert(0);  // Not supported
    break;
  }
  // Default to ENU (from legacy code)
  return COORD_SYS_ENU_STR;
}

int coordinateSystemFromString(const std::string& str, CoordinateSystem& outSystem)
{
  if (TFormat::caseCompare(str, COORD_SYS_NED_STR) == 0)
    outSystem = COORD_SYS_NED;
  else if (TFormat::caseCompare(str, COORD_SYS_NWU_STR) == 0)
    outSystem = COORD_SYS_NWU;
  else if (TFormat::caseCompare(str, COORD_SYS_LLA_DMS_STR) == 0)
    outSystem = COORD_SYS_LLA;
  else if (TFormat::caseCompare(str, COORD_SYS_LLA_DMD_STR) == 0)
    outSystem = COORD_SYS_LLA;
  else if (TFormat::caseCompare(str, COORD_SYS_LLA_STR) == 0)
    outSystem = COORD_SYS_LLA;
  else if (TFormat::caseCompare(str, COORD_SYS_ECEF_STR) == 0)
    outSystem = COORD_SYS_ECEF;
  else if (TFormat::caseCompare(str, COORD_SYS_ECI_STR) == 0)
    outSystem = COORD_SYS_ECI;
  else if (TFormat::caseCompare(str, COORD_SYS_ENU_STR) == 0)
    outSystem = COORD_SYS_ENU;
  else if (TFormat::caseCompare(str, COORD_SYS_XEAST_STR) == 0)
    outSystem = COORD_SYS_XEAST;
  else if (TFormat::caseCompare(str, COORD_SYS_GTP_STR) == 0)
    outSystem = COORD_SYS_GTP;
  else
  {
    outSystem = COORD_SYS_LLA;
    return 1;
  }

  return 0;
}


