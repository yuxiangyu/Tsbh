#ifndef CALC_MATH_H
#define CALC_MATH_H

#include <complex>
#include <cmath>
#include <cassert>
#include <limits>

#include "rtsscore_global.h"
#include "MathConstants.h"
#include "Vec3.h"
#include <osg/Matrix>
#include <osg/Quat>

// Define isnan, finite to match Linux


class RTSSCORE_EXPORT TMath {

public:

  template<typename T>
  static T sdkMax(const T &a, const T& b)
  {
    return (a > b) ? a : b;
  };

  /**
  * Compute the minimum of the two incoming values
  * @param[in ] a First value
  * @param[in ] b Second value
  * @return minimum of the two values
  */
  template<typename T>
  static T sdkMin(const T &a, const T& b)
  {
    return (a < b) ? a : b;
  };

  /**
  * Computes the nearest integer or even of the incoming value
  * @param[in ] x
  * @return nearest integer, ties (.5) are rounded to the nearest even integer
  */
  static double rint(double x)
  {
#ifdef WIN32
    // round to nearest or even value
    // in case of ties, (.5) values are rounded to the nearest even integer
    double a = floor(x);
    if (x - a < .5) return a;
    if (x - a > .5) return a + 1.0;
    return (fmod(a, 2) == 0.0) ? a : a + 1.0;
#else
    return ::rint(x);
#endif
  };

  /**
  * Rounds the incoming value to the nearest integer
  * @param[in ] x
  * @return nearest integer
  */
  static double round(double x)
  {
    // round to nearest integer, provides consistent behavior across all OS
    return (x > 0.0) ? floor(x + 0.5) : ceil(x - 0.5);
  };

  //--------------------------------------------------------------------------
  //---general functions

  /// Determines if an integer is odd
  /**
  * Determines if an integer is odd
  * @param[in ] n an integer to be tested for oddness
  * @return true if n is odd; false otherwise
  */
  static bool odd(int n)
  {
    return ((n&1) != 0 ? true : false);
  };

  /**
  * Compute the square of incoming value
  * @param[in ] in
  * @return square of the input
  */
  template<typename T>
  static T square(const T &in)
  {
    return in * in;
  };

  /**
  * Determines the sign of incoming value
  * @param[in ] in
  * @return sign, 0: no sign, 1: positive, -1: negative
  */
  template<typename T>
  static T sign(const T &in)
  {
    return in >= 0 ? (in == 0 ? 0 : 1) : -1;
  };

  /**
  * Checks the equality of two values based on a tolerance
  * @param[in ] a First value to compare
  * @param[in ] b Second value to compare
  * @param[in ] t Comparison tolerance
  * @return the equality of two values based on a tolerance
  */
  static bool areEqual(double a, double b, double t=1.0e-6)
  {
    return fabs(a - b) < t;
  };

  /**
   * Gives a reasonable step size for incrementing through the given range.
   * Step size is a power of 10, no smaller than 1e(-precision), and spans the full range in no more than 1000 increments.
   * @param[in ] range Size of the range of values.  Must be >0
   * @param[in ] precision Precision of the range.  Must be >=0
   * @return A reasonable step size for incrementing through the range or 0 if input is invalid
   */
  static double guessStepSize(double range, int precision)
  {
    if (range <= 0 || precision < 0)
    {
      // Given invalid input
      assert(0);
      return 0;
    }
    double rangeStep = pow(10.0, ceil(log10(range))) / 1000;
    double precStep = 1/pow(10.0, precision);
    return sdkMax(rangeStep, precStep);
  };

  //--------------------------------------------------------------------------
  //---Vec3 related functions

  /**
   * Returns true if all 3 elements are finite
   * @param[in ] u vector
   * @return true if all 3 elements are finite
   */
  static bool isFinite(const SimVec3 &u)
  {
   // return finite(u[0]) && finite(u[1]) && finite(u[2]);
	  if(u[0]<10e29&&u[1]<10e29&&u[2]<10e29)
	  return true;
	  return false;
  };

  /**
   * Breaks up value into a mantissa (or significand) and exponent value, for base 10
   * values.  Returns mantissa such that mantissa * pow(10.0, exp) == value.
   * @param value Argument to represent in scientific notation.
   * @param exp If not nullptr, will receive the exponent portion of the value.
   * @return Mantissa value.  Will be 0.0 for input of 0.0.  Otherwise, is a value with
   *   an absolute value between [+1.0, +10.0) with a sign set appropriately.  Negative
   *   input values result in a negative return value.
   */
  static double toScientific(double value, int* exp);

  /**
  * Find the distance from u to v
  * @param[in ] u first vector
  * @param[in ] v second vector
  * @return distance
  */
  static double v3Distance(const SimVec3 &u, const SimVec3 &v)
  {
    // use sqrt of sum of squares of deltas
    return sqrt(square(u[0] - v[0]) + square(u[1] - v[1]) + square(u[2] - v[2]));
  };

  /**
  * Magnitude or length of a vector
  * @param[in ] u vector to consider
  * @return length (Euclidean norm or magnitude) of the vector
  */
  static double v3Length(const SimVec3 &u)
  {
    return sqrt(square(u[0]) + square(u[1]) + square(u[2]));
  };

  /**
  * Scale a vector; v  =  s * u
  * @param[in ] s scale factor (scalar)
  * @param[in ] u starting vector
  * @param[out] v output
  */
  static void v3Scale(double s, const SimVec3 &u, SimVec3 &v)
  {
    v.set(s * u[0], s * u[1], s * u[2]);
  };

  /**
  * Turns u into a unit vector, and returns original | u |
  * Converts 3 element double vector u into a unit vector
  * @param[in,out] u vector to be set to unit length
  * @return | u | before transformation applied
  */
  static double v3Unit(SimVec3 &u)
  {
    double t = v3Length(u);
    if (t > 0.0)
    {
      v3Scale(1.0/t, u, u);
    }
    return t;
  };

  /**
  * Returns the normal of vector u
  * @param[ in] u Vector to determine normal of (3 element double vector)
  * @param[out] v output vector to contain normal of u
  * @param[in ] t Comparison tolerance for a zero value
  */
  static void v3Norm(const SimVec3 &u, SimVec3 &v, double t=1.0e-9)
  {
    double len = v3Length(u);
    // prevent divide by zero
    if (len > 0.0)
    {
      v3Scale(1.0/len, u, v);
      // if very small values are detected, set to zero
      if (fabs(v.x()) < t) v.setX(0);
      if (fabs(v.y()) < t) v.setY(0);
      if (fabs(v.z()) < t) v.setZ(0);
    }
    else
    {
      v.zero();
    }
  };

  /**
  * Add two vectors; w = u + v
  * @param[in ] u first vector
  * @param[in ] v second vector
  * @param[out] w output
  */
  static void v3Add(const SimVec3 &u, const SimVec3 &v, SimVec3 &w)
  {
    w.set(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
  };

  /**
  * Subtract two vectors; w = u - v
  * @param[in ] u left side of subtraction
  * @param[in ] v subtracted from u
  * @param[out] w output
  */
  static void v3Subtract(const SimVec3 &u, const SimVec3 &v, SimVec3 &w)
  {
    w.set(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
  };

  /**
  * Dot product of two vectors
  * @param[in ] u first vector
  * @param[in ] v second vector
  */
  static double v3Dot(const SimVec3 &u, const SimVec3 &v)
  {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
  };

  /**
  * Cross product of two vectors; w = u cross v
  * @param[in ] u first vector
  * @param[in ] v second vector
  * @param[out] w output
  */
  static void v3Cross(const SimVec3 &u, const SimVec3 &v, SimVec3& w)
  {
    w.set(u[1]*v[2] - u[2]*v[1],
          u[2]*v[0] - u[0]*v[2],
          u[0]*v[1] - u[1]*v[0]);
  };

  /**
  * Negate a vector
  * @param[in ] u input vector
  * @param[out] w output
  */
  static void v3Negate(const SimVec3 &u, SimVec3& w)
  {
    w.set(-u[0], -u[1], -u[2]);
  };

  /**
  * Convert a spherical coordinate to a rectangular coordinate
  * @param[in ] rng range
  * @param[in ] az azimuth (rad)
  * @param[in ] el elevation (rad)
  * @param[out] v output vector in units of range
  */
  static void v3SphtoRec(double rng, double az, double el, SimVec3 &v)
  {
    // NOTE: elevation measured off horizon (XY plane) instead of Z axis
    v.set(rng * sin(az) * cos(el),  // X (v[0])
          rng * cos(az) * cos(el),  // Y (v[1])
          rng * sin(el));           // Z (v[2])
  };

  /**
  * Convert a spherical coordinate to a rectangular coordinate
  * @param[in ] rae input vector of range, azimuth (rad) an elevation (rad)
  * @param[out] v output vector in units of range component
  */
  static void v3SphtoRec(const SimVec3 &rae, SimVec3 &v)
  {
    v3SphtoRec(rae.range(), rae.raeAz(), rae.raeEl(), v);
  };

  /**
  * Compares two vectors for equality within the specified tolerance
  * @param[in ] u input vector to compare
  * @param[in ] v input vector to compare
  * @param[in ] t comparison tolerance
  * @return true if u is equal to v (within tolerance 't')
  */
  static bool v3AreEqual(const SimVec3 &u, const SimVec3 &v, double const t=1.0e-6);

  /**
  * Rotates given vector about X axis
  * @param[in ] a input vector
  * @param[in ] ang rotation angle about X axis (rad)
  * @param[out] vp output vector
  */
  static void v3RotX(const SimVec3 &a, double ang, SimVec3 &vp);

  /**
  * Rotates given vector about Y axis
  * @param[in ] a input vector
  * @param[in ] ang rotation angle about Y axis (rad)
  * @param[out] vp output vector
  */
  static void v3RotY(const SimVec3 &a, double ang, SimVec3 &vp);

  /**
  * Computes angle between two input vectors
  * @param[in ] u input vector 1
  * @param[in ] v input vector 2
  * @return angle between u and v
  */
  static double v3Angle(const SimVec3 &u, const SimVec3 &v);

  //--------------------------------------------------------------------------
  //---Matrix (double[3][3]) related functions
  //
  // In the function descriptions and prototypes below, a, b, and c are
  // 3-by-3 matrices; u and v are 3-vectors (Vec3); and q is a four-element
  // quaternion.  For efficiency, the matrices as well as the vectors and
  // quaternions are treated as one-dimensional within the code, by use of
  // an intermediary pointer (aa, bb, or cc).  The relationship of
  // internal storage to mathematical concept is as shown:
  //
  //        [ a[0]  a[1]  a[2] ]           [ u[0] ]         [ q[0] ]
  //  a  =  [ a[3]  a[4]  a[5] ],    u  =  [ u[1] ],    q = [ q[1] ].
  //        [ a[6]  a[7]  a[8] ]           [ u[2] ]         [ q[2] ]
  //                                                        [ q[3] ]

  /**
  * Matrix multiply; c[3][3] = a[3][3] * b[3][3]
  * @param[in ] a first matrix
  * @param[in ] b second matrix
  * @param[out] c output matrix
  * @pre All matrices valid
  */
  static void d3MMmult(const double a[][3], const double b[][3], double c[][3]);

  /**
  * Matrix to vector multiply; v[3] =  a[3][3] * u[3]
  * @param[in ] a matrix
  * @param[in ] u vector
  * @param[out] v output vector
  * @pre matrix valid, u and v cannot be the same vector
  */
  static void d3Mv3Mult(const double a[][3], const SimVec3 &u, SimVec3 &v);

  /**
  * Transposed matrix to vector multiply; v[3] = transpose(a[3][3]) * u[3]
  * @param[in ] a matrix
  * @param[in ] u vector
  * @param[out] v output vector
  * @pre matrix valid, u and v cannot be the same vector
  */
  static void d3MTv3Mult(const double a[][3], const SimVec3 &u, SimVec3 &v);

  /**
  * Transposed matrix multiply; c[3][3] = a[3][3] * transpose(b[3][3])
  * @param[in ] a first matrix
  * @param[in ] b second matrix
  * @param[out] c output matrix
  * @pre All matrices valid
  */
  static void d3MMTmult(const double a[][3], const double b[][3], double c[][3]);

  //--------------------------------------------------------------------------
  //---Quaternion related functions
  //
  // Functions expect a quaternion in the form:  q0 + q1i + q2j + q3k  (w,x,y,z)

  /**
  * Returns the normal of quaternion q
  * @param[in ] q 4 element double vector quaternion to normalize
  * @param[out] n 4 element double vector quaternion that contains normalized q
  * @param[in ] t Comparison tolerance for a zero value
  * @pre All vectors valid
  */
  static void dQNorm(const double q[4], double n[4], double t=1.0e-9);

  /**
  * Returns the multiplication of two quaternions, where result = q1 * q2
  * @param[in ] q1 4 element double vector quaternion
  * @param[in ] q2 4 element double vector quaternion
  * @param[out] result 4 element double vector quaternion containing q1 * q2
  * @pre All vectors valid
  */
  static void dQMult(const double q1[4], const double q2[4], double result[4]);

  //--------------------------------------------------------------------------
  //---Euler angle conversion functions

  /**
  * Convert a direction cosine matrix to Euler angles using a NED frame
  * @param[in ] dcm Direction cosine matrix
  * @param[out] ea Euler angles
  * @pre DCM valid
  */
  static void d3DCMtoEuler(const double dcm[][3], SimVec3 &ea);

  /**
  * Convert Euler angles to a direction cosine matrix using a NED frame
  * @param[in ] ea Euler angles
  * @param[out] dcm Direction cosine matrix (DCM)
  * @pre DCM valid
  */
  static void d3EulertoDCM(const SimVec3 &ea, double dcm[][3]);

  /**
  * Converts Euler angles to a quaternion vector using a NED frame
  * @param[in ] ea 3 element double vector of Euler angles (psi/yaw, theta/pitch, phi/roll)
  * @param[out] q Result of conversion of Euler angles ea to a 4 element double vector quaternion
  * @pre quaternion valid
  */
  static void d3EulertoQ(const SimVec3 &ea, double q[4]);

  /**
  * Converts a quaternion vector to Euler angles using a NED frame
  * @param[in ] q 4 element double vector quaternion
  * @param[out] ea Result of conversion of quaternion vector to a 3 element double vector of Euler angles (psi/yaw, theta/pitch, phi/roll)
  * @pre quaternion valid
  */
  static void d3QtoEuler(const double q[4], SimVec3 &ea);

  /**
   * Given a number and number of digits of significance, returns a number X, such that [floor|ceil](num * 10^X) / 10^X
   * will have "precision" significant digits.  This value is useful for rounding ranges to round numbers, and is used
   * by simCore::roundRanges() to generate bounds relevant and near to the input values.
   * @param num Number to do the function on
   * @param significance Number of digits of significance desired, post-formula (1200 = 2 digits, 500 = 1 digit, 0.0304 = 3 digits)
   * @return X, so that [floor|ceil](num * 10^X) / 10^X will have the desired significance
   */
  static int getPowerOfTenForSignificance(double num, unsigned int significance);

  /**
   * Performs a "round ranges" on the given values.  Given an input range value, generates
   * a pair of rounded ranges that encompasses the values.  For example, providing ranges
   * [0.5, 19.7] might round up to [0.0, 20.0].
   * @param[inout] minValue Minimum value for the new range
   * @param[inout] maxValue Maximum value for the new range
   */
  static void roundRanges(double& minValue, double& maxValue);

  static void enuRotMatrixToEcefEuler(const osg::Matrix& in, SimVec3& out);

  static osg::Quat eulerRadToQuat(double h, double p, double r);
  

  static void ecefEulerToEnuRotMatrix(const SimVec3& in, osg::Matrix& out);
 

  static double sodanoInverse(const double refLat, const double refLon, const double refAlt, const double lat, const double lon, double *azfwd, double *azbck=NULL);
  

  static void sodanoDirect(const double refLat, const double refLon, const double refAlt, const double dist, const double azfwd, double *latOut, double *lonOut, double *azbck=NULL);
  };
#endif /* SIMCORE_CALC_MATH_H */
