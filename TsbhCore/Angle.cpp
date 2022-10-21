
#include <complex>
#include <cassert>
#include "Math1.h"
#include "Angle.h"

//------------------------------------------------------------------------

/// Returns the inverse cosine with allowance for accumulated error
double inverseCosine(double in)
{
  // Assert if off by more than an accumulated error
  assert((in > -1.0001) && (in < 1.0001));

  // Guard against accumulated math error that results in an invalid argument value
  if (in <= -1.0)
    return M_PI;

  // Guard against accumulated math error that results in an invalid argument value
  if (in >= 1.0)
    return 0.0;

  return acos(in);
}

/// Returns the inverse sine with allowance for accumulated error
double inverseSine(double in)
{
  // Assert if off by more than an accumulated error
  assert((in > -1.0001) && (in < 1.0001));

  // Guard against accumulated math error that results in an invalid argument value
  if (in <= -1.0)
    return -M_PI_2;

  // Guard against accumulated math error that results in an invalid argument value
  if (in >= 1.0)
    return M_PI_2;

  return asin(in);
}

/// Compares two angle vectors for equality within the specified tolerance
bool v3AreAnglesEqual(const SimVec3 &u, const SimVec3 &v, double const t)
{
  return areAnglesEqual(u[0], v[0], t) && areAnglesEqual(u[1], v[1], t) && areAnglesEqual(u[2], v[2], t);
}

/// Checks the equality of two angles based on a tolerance
bool areAnglesEqual(double angle1, double angle2, double t)
{
  if (TMath::areEqual(angle1, angle2, t))
    return true;

  // Make sure the values are in the same range
  angle1 = angFixPI(angle1);
  angle2 = angFixPI(angle2);

  if (TMath::areEqual(angle1, angle2, t))
    return true;

  // Test for 180 versus -180 which are the same
  if (TMath::areEqual(fabs(angle1), M_PI, t) && TMath::areEqual(fabs(angle2), M_PI, t))
    return true;

  return false;
}

/// Rotates one angle by another
SimVec3 rotateEulerAngle(const SimVec3& startAngle, const SimVec3& rotateBy)
{
  // Create quaternions from the rotations
  double qStartAngle[4];
  TMath::d3EulertoQ(startAngle, qStartAngle);
  double qRotateBy[4];
  TMath::d3EulertoQ(rotateBy, qRotateBy);

  // Multiply the rotations, and convert back out to Euler
  double qFinal[4];
  TMath::dQMult(qStartAngle, qRotateBy, qFinal);
  SimVec3 rv;
  TMath::d3QtoEuler(qFinal, rv);
  return rv;
}

double angFix(double radianAngle, AngleExtents extents)
{
  switch (extents)
  {
  case ANGLEEXTENTS_TWOPI:
    return angFix2PI(radianAngle);
  case ANGLEEXTENTS_PI:
    return angFixPI(radianAngle);
  case ANGLEEXTENTS_PI_2:
    return angFixPI2(radianAngle);
  case ANGLEEXTENTS_ALL:
    return radianAngle;
  }
  // Invalid enumeration, developer error
  assert(0);
  return radianAngle;
}

double angFixDegrees(double degreeAngle, AngleExtents extents)
{
  switch (extents)
  {
  case ANGLEEXTENTS_TWOPI:
    return angFix360(degreeAngle);
  case ANGLEEXTENTS_PI:
    return angFix180(degreeAngle);
  case ANGLEEXTENTS_PI_2:
    return angFix90(degreeAngle);
  case ANGLEEXTENTS_ALL:
    return degreeAngle;
  }
  // Invalid enumeration, developer error
  assert(0);
  return degreeAngle;
}

double angleDifference(double fromRad, double toRad)
{
  // Implementation drew from https://stackoverflow.com/questions/1878907

  // Fix toRad and fromRad from [-PI,PI] inclusive before subtracting
  const double subtracted = angFixPI(toRad) - angFixPI(fromRad);
  // Note that we can't rely solely on angFixPI here due to inclusiveness of -PI
  const double fixed = angFixPI(subtracted);
  return (fixed <= -M_PI) ? (fixed + M_TWOPI) : fixed;
}

double angleDifferenceDeg(double fromDeg, double toDeg)
{
  return RAD2DEG * angleDifference(DEG2RAD * fromDeg, DEG2RAD * toDeg);
}
