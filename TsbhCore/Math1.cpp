
#include "Vec3.h"
#include "Angle.h"
#include "Math1.h"


//------------------------------------------------------------------------
static const double NED_ENU[3][3] =
{
	{ 0.0, 1.0,  0.0 },
	{ 1.0, 0.0,  0.0 },
	{ 0.0, 0.0, -1.0 }
};
/// Compares two vectors for equality within the specified tolerance
bool TMath::v3AreEqual(const SimVec3 &u, const SimVec3 &v, double const t)
{
  return areEqual(u[0], v[0], t) && areEqual(u[1], v[1], t) && areEqual(u[2], v[2], t);
}

/// Rotates given vector about X axis
void TMath::v3RotX(const SimVec3 &a, const double ang, SimVec3 &vp)
{
  if (!ang)
  {
    vp = a;
    return;
  }

  const double cosang = cos(ang);
  const double sinang = sin(ang);

  const double y = cosang*a[1] - sinang*a[2];
  const double z = sinang*a[1] + cosang*a[2];

  vp.set(a[0], y, z);
}

/// Rotates given vector about Y axis
void TMath::v3RotY(const SimVec3 &a, const double ang, SimVec3 &vp)
{
  if (!ang)
  {
    vp = a;
    return;
  }

  // Need to negate to make it a rotation from x to z
  const double cosang = cos(-ang);
  const double sinang = sin(-ang);

  const double x = cosang * a[0] - sinang * a[2];
  const double z = sinang * a[0] + cosang * a[2];

  vp.set(x, a[1], z);
}

/// Computes angle between two input vectors
double TMath::v3Angle(const SimVec3 &u, const SimVec3 &v)
{
  const double length = v3Length(u) * v3Length(v);
  if (length == 0.0)
    return 0.0;

  const double arg = v3Dot(u, v) / length;
  return inverseCosine(arg);
}

/// Matrix multiply; c = a * b
void TMath::d3MMmult(const double a[][3], const double b[][3], double c[][3])
{
  assert(a);
  assert(b);
  assert(c);

  if ((a == nullptr) || (b == nullptr) || (c == nullptr))
  {
    if (c != nullptr)
    {
      for (size_t ii = 0; ii < 3; ++ii)
      {
        for (size_t jj = 0; jj < 3; ++jj)
          c[ii][jj] = 0.0;
      }
    }

    return;
  }

  const double *aa = *a;
  const double *bb = *b;
  double *cc = *c;
  cc[0] = aa[0] * bb[0]  +  aa[1] * bb[3]  +  aa[2] * bb[6];
  cc[1] = aa[0] * bb[1]  +  aa[1] * bb[4]  +  aa[2] * bb[7];
  cc[2] = aa[0] * bb[2]  +  aa[1] * bb[5]  +  aa[2] * bb[8];
  cc[3] = aa[3] * bb[0]  +  aa[4] * bb[3]  +  aa[5] * bb[6];
  cc[4] = aa[3] * bb[1]  +  aa[4] * bb[4]  +  aa[5] * bb[7];
  cc[5] = aa[3] * bb[2]  +  aa[4] * bb[5]  +  aa[5] * bb[8];
  cc[6] = aa[6] * bb[0]  +  aa[7] * bb[3]  +  aa[8] * bb[6];
  cc[7] = aa[6] * bb[1]  +  aa[7] * bb[4]  +  aa[8] * bb[7];
  cc[8] = aa[6] * bb[2]  +  aa[7] * bb[5]  +  aa[8] * bb[8];
}

/// Matrix to vector multiply; v = a * u
void TMath::d3Mv3Mult(const double a[][3], const SimVec3 &u, SimVec3 &v)
{
  // Test for same input/output -- this function cannot handle case of u == v
  if (&u == &v)
  {
    assert(0);
    return;
  }

  assert(a);
  if (a == nullptr)
  {
    v.zero();
    return;
  }

  v.setV0(a[0][0] * u[0] + a[0][1] * u[1] + a[0][2] * u[2]);
  v.setV1(a[1][0] * u[0] + a[1][1] * u[1] + a[1][2] * u[2]);
  v.setV2(a[2][0] * u[0] + a[2][1] * u[1] + a[2][2] * u[2]);
}

/// Transposed matrix to vector multiply; v = transpose(a) * u
void TMath::d3MTv3Mult(const double a[][3], const SimVec3 &u, SimVec3 &v)
{
  // Test for same input/output -- this function cannot handle case of u == v
  if (&u == &v)
  {
    assert(0);
    return;
  }

  assert(a);
  if (a == nullptr)
  {
    v.zero();
    return;
  }

  v.setV0(a[0][0] * u[0] + a[1][0] * u[1] + a[2][0] * u[2]);
  v.setV1(a[0][1] * u[0] + a[1][1] * u[1] + a[2][1] * u[2]);
  v.setV2(a[0][2] * u[0] + a[1][2] * u[1] + a[2][2] * u[2]);
}

/// Transposed matrix multiply; c = a * transpose(b)
void TMath::d3MMTmult(const double a[][3], const double b[][3], double c[][3])
{
  assert(a);
  assert(b);
  assert(c);

  if ((a == nullptr) || (b == nullptr) || (c == nullptr))
  {
    if (c != nullptr)
    {
      for (size_t ii = 0; ii < 3; ++ii)
      {
        for (size_t jj = 0; jj < 3; ++jj)
          c[ii][jj] = 0.0;
      }
    }

    return;
  }

  const double *aa = *a;
  const double *bb = *b;
  double *cc = *c;
  cc[0] = aa[0] * bb[0]  +  aa[1] * bb[1]  +  aa[2] * bb[2];
  cc[1] = aa[0] * bb[3]  +  aa[1] * bb[4]  +  aa[2] * bb[5];
  cc[2] = aa[0] * bb[6]  +  aa[1] * bb[7]  +  aa[2] * bb[8];
  cc[3] = aa[3] * bb[0]  +  aa[4] * bb[1]  +  aa[5] * bb[2];
  cc[4] = aa[3] * bb[3]  +  aa[4] * bb[4]  +  aa[5] * bb[5];
  cc[5] = aa[3] * bb[6]  +  aa[4] * bb[7]  +  aa[5] * bb[8];
  cc[6] = aa[6] * bb[0]  +  aa[7] * bb[1]  +  aa[8] * bb[2];
  cc[7] = aa[6] * bb[3]  +  aa[7] * bb[4]  +  aa[8] * bb[5];
  cc[8] = aa[6] * bb[6]  +  aa[7] * bb[7]  +  aa[8] * bb[8];
}

/// Returns the normal of quaternion q, a comparison to zero occurs within the specified tolerance
void TMath::dQNorm(const double q[4], double n[4], double t)
{
  assert(q);
  assert(n);
  if (!q || !n)
    return;

  // prevent divide by zero
  if (q[0] != 0 || q[1] != 0 || q[2] != 0 || q[3] != 0)
  {
    double invMag = 1.0 / sqrt((q[0] * q[0]) + (q[1] * q[1]) + (q[2] * q[2]) + (q[3] * q[3]));
    n[0] = q[0] * invMag;
    n[1] = q[1] * invMag;
    n[2] = q[2] * invMag;
    n[3] = q[3] * invMag;

    n[0] = (fabs(n[0]) < t) ? 0 : n[0];
    n[1] = (fabs(n[1]) < t) ? 0 : n[1];
    n[2] = (fabs(n[2]) < t) ? 0 : n[2];
    n[3] = (fabs(n[3]) < t) ? 0 : n[3];
  }
  else
  {
    n[0] = n[1] = n[2] = n[3] = 0;
  }
}

/// Returns the multiplication of two quaternions, where result = q2 * q1 in an absolute frame
void TMath::dQMult(const double q2[4], const double q1[4], double result[4])
{
  assert(q1);
  assert(q2);
  assert(result);
  if (!q1 || !q2 || !result)
    return;

  // Perform multiplication of two four-element quaternion vectors (q0,q1,q2,q3) aka (w,x,y,z)
  // Quaternion multiplication is not commutative. Thus q1 * q2 is not the same as q2 * q1.
  // q2 * q1 denotes an absolute frame of reference
  // q1 * q2 denotes a relative frame of reference for combining rotations

  result[0] = -q2[1]*q1[1] - q2[2]*q1[2] - q2[3]*q1[3] + q2[0]*q1[0]; // w
  result[1] =  q2[1]*q1[0] + q2[2]*q1[3] - q2[3]*q1[2] + q2[0]*q1[1]; // x
  result[2] = -q2[1]*q1[3] + q2[2]*q1[0] + q2[3]*q1[1] + q2[0]*q1[2]; // y
  result[3] =  q2[1]*q1[2] - q2[2]*q1[1] + q2[3]*q1[0] + q2[0]*q1[3]; // z
}

/// Convert a direction cosine matrix to Euler angles
void TMath::d3DCMtoEuler(const double dcm[][3], SimVec3 &ea)
{
  // From Aircraft Control and Simulation 2nd Edition
  // B. Stevens & F. Lewis  2003
  // ISBN 0-471-37145-9
  // p. 29, Eqn 1.3-24

  // The Direction Cosine Matrix to Euler angles function converts a
  // 3x3 direction cosine matrix (DCM) into three Euler rotation angles.
  // The DCM matrix performs the coordinate transformation of a vector in
  // inertial axes into a vector in body axes. The order of the axis
  // rotations required to bring the body axis into coincidence with the
  // inertial axis is first, a rotation about the body x through the roll
  // angle phi, second, a rotation about the body y through the pitch angle
  // theta, and finally a rotation about the body z through the yaw angle psi.

  // prevent division by zero and inverse trig function arguments of
  // magnitude greater than unity:
  //   atan2 returns in the range -pi to pi
  //   asin returns in the range -pi/2 to pi/2

  assert(dcm);
  if (dcm == nullptr)
  {
    ea.zero();
    return;
  }

  if (areEqual(dcm[0][2], 1.0))
  {
    ea.setV0(0.0);
    ea.setV1(-M_PI_2);
    ea.setV2(atan2(-dcm[1][0], -dcm[2][0]));
  }
  else if (areEqual(dcm[0][2], -1.0))
  {
    ea.setV1(M_PI_2);
    ea.setV2(atan2(dcm[1][0], dcm[2][0]));
    ea.setV0(0.0);
  }
  else
  {
    // no gimbal lock
    // we want psi (yaw) between 0 to 360
    ea.setV0(angFix2PI(atan2(dcm[0][1], dcm[0][0])));
    ea.setV1(inverseSine(-dcm[0][2]));
    ea.setV2(atan2(dcm[1][2], dcm[2][2]));
  }
}

/// Convert Euler angles to a direction cosine matrix
void TMath::d3EulertoDCM(const SimVec3 &ea, double dcm[][3])
{
  // From Aircraft Control and Simulation 2nd Edition
  // B. Stevens & F. Lewis  2003
  // ISBN 0-471-37145-9
  // p. 26, Eqn 1.3-20

  // The Euler angles to Direction Cosine Matrix function converts three
  // Euler rotation angles into a 3x3 direction cosine matrix (DCM).
  // The DCM matrix performs the coordinate transformation of a yaw, pitch, roll
  // (YPR) vector in inertial axes into a vector in a NED body axes. The order
  // of the axis rotations required to bring the body axis into coincidence with
  // the inertial axis is first, a rotation about the body x through the roll
  // angle phi, second, a rotation about the body y through the pitch angle
  // theta, and finally a rotation about the body z through the yaw angle psi.

  assert(dcm);
  if (dcm == nullptr)
    return;

  // psi/yaw components
  const double spsi = sin(ea[0]);
  const double cpsi = cos(ea[0]);
  // theta/pitch components
  const double stheta = sin(ea[1]);
  const double ctheta = cos(ea[1]);
  // phi/roll components
  const double sphi = sin(ea[2]);
  const double cphi = cos(ea[2]);

  // complete transformation from a reference frame to the body frame
  // The sequence of rotations to describe the instantaneous attitude
  // (orientation) with respect to a reference frame is as follows:
  //
  // 1. Rotate about the z-axis, nose right (positive yaw)
  // 2. Rotate about the new y-axis, nose up (positive pitch)
  // 3. Rotate about the new x-axis, right wing down (positive roll)
  //
  // Coordinate transformation from YPR vector to NED frame

  dcm[0][0] = cpsi * ctheta;
  dcm[0][1] = spsi * ctheta;
  dcm[0][2] = -stheta;

  dcm[1][0] = cpsi * stheta * sphi - spsi * cphi;
  dcm[1][1] = spsi * stheta * sphi + cpsi * cphi;
  dcm[1][2] = ctheta * sphi;

  dcm[2][0] = cpsi * stheta * cphi + spsi * sphi;
  dcm[2][1] = spsi * stheta * cphi - cpsi * sphi;
  dcm[2][2] = ctheta * cphi;
}

/// Converts Euler angles to a quaternion vector
void TMath::d3EulertoQ(const SimVec3 &ea, double q[4])
{
  // From Aircraft Control and Simulation 2nd Edition
  // B. Stevens & F. Lewis  2003
  // ISBN 0-471-37145-9
  // p. 32, Eqn 1.3-33

  // The Euler angles to quaternions function converts the rotation
  // described by the three Euler angles (yaw, pitch, roll) into the
  // four-element quaternion vector (q0,q1,q2,q3) aka (w,x,y,z)
  // Results verified by Matlab: http://www.mathworks.com/matlabcentral/fileexchange/27653

  assert(q);
  if (q == nullptr)
    return;

  // psi/yaw components
  const double shpsi = sin(ea[0]*.5);
  const double chpsi = cos(ea[0]*.5);
  // theta/pitch components
  const double shtheta = sin(ea[1]*.5);
  const double chtheta = cos(ea[1]*.5);
  // phi/roll components
  const double shphi = sin(ea[2]*.5);
  const double chphi = cos(ea[2]*.5);

  q[0] = chphi * chtheta * chpsi + shphi * shtheta * shpsi; // w
  q[1] = shphi * chtheta * chpsi - chphi * shtheta * shpsi; // x
  q[2] = chphi * shtheta * chpsi + shphi * chtheta * shpsi; // y
  q[3] = chphi * chtheta * shpsi - shphi * shtheta * chpsi; // z
}

/// Converts a quaternion vector to Euler angles
void TMath::d3QtoEuler(const double q[4], SimVec3 &ea)
{
  // From Aircraft Control and Simulation 2nd Edition
  // B. Stevens & F. Lewis  2003
  // ISBN 0-471-37145-9
  // p. 29 and 31, Eqns 1.3-24 and 1.3-32
  // Euler angles (1.3-24) are recovered from the DCM elements in 1.3-32

  // The quaternions to Euler angles function converts the four-element
  // unit quaternion (q0,q1,q2,q3) aka (w,x,y,z) into the equivalent three
  // Euler angle rotations (yaw, pitch, roll).  The conversion is generated
  // by comparing elements in the direction cosine matrix (DCM), as functions
  // of the Euler rotation angles, with elements in the DCM, as functions of
  // a unit quaternion vector

  // The DCM matrix performs the coordinate transformation of a vector in
  // inertial axes into a vector in body axes. The order of the axis
  // rotations required to bring the body axis into coincidence with the
  // inertial axis is first, a rotation about the body x through the roll
  // angle phi, second, a rotation about the body y through the pitch angle
  // theta, and finally a rotation about the body z through the yaw angle psi.

  // Function expects a normalized quaternion in the form:  q0 + q1i + q2j + q3k

  assert(q);
  if (q == nullptr)
  {
    ea.zero();
    return;
  }

  const double sq0 = q[0] * q[0];
  const double sq1 = q[1] * q[1];
  const double sq2 = q[2] * q[2];
  const double sq3 = q[3] * q[3];

  const double dcm00 = sq0 + sq1 - sq2 - sq3;
  const double dcm01 = 2. * (q[1]*q[2] + q[0]*q[3]);
  const double dcm02 = 2. * (q[1]*q[3] - q[0]*q[2]);

  // check for singularities at +/- 90
  if (fabs(dcm00) > 1e-6 || fabs(dcm01) > 1e-6)
  {
    // no gimbal lock
    // we want psi (yaw) between 0 to 360
    ea.setV0(angFix2PI(atan2(dcm01, dcm00)));
    // theta/pitch
    ea.setV1(inverseSine(dcm02));
    // phi/roll
    const double dcm12 = 2. * (q[2]*q[3] + q[0]*q[1]);
    const double dcm22 = sq0 - sq1 - sq2 + sq3;
    ea.setV2(atan2(dcm12, dcm22));
  }
  else
  {
    // gimbal lock case
    // psi/yaw
    ea.setV0(0.);
    // theta/pitch
    ea.setV1(-inverseSine(dcm02));
    // phi/roll
    const double dcm10 = 2. * (q[1]*q[2] - q[0]*q[3]);
    const double dcm11 = sq0 - sq1 + sq2 - sq3;
    ea.setV2(-atan2(dcm10, dcm11));
  }
}

double TMath::toScientific(double value, int* exp)
{
  if (value == 0.0)
  {
    if (exp)
      *exp = 0;
    return 0.0;
  }

  // Reduce edge cases by working with absolute value
  const double absValue = fabs(value);
  // Determine exponent and mantissa of the absolute value
  const double numZeros = floor(log10(absValue));
  const double mantissa = absValue / pow(10.0, numZeros);
  // Assertion failure means error in logic for scientific notation calculations
  assert(mantissa >= 1.0 && mantissa < 10.0);
  // Return mantissa with correct sign, and exponent
  if (exp)
    *exp = static_cast<int>(numZeros);
  return (value < 0) ? -mantissa : mantissa;
}

int TMath::getPowerOfTenForSignificance(double num, unsigned int significance)
{
  // If number is 0, return 0 to avoid infinite case from log10(0)
  if (num == 0.0)
    return 0;

  const double precisionMax = pow(10.0, static_cast<double>(significance));
  // Determine the number of digits difference from the significance to the provided value's significance
  const double log10Diff = static_cast<double>(significance) - log10(fabs(num));

  // Return 0 for a log10 that is positive, but under/at the significance value.  For example,
  // if the expected range is [0.0,100.0] any incoming num in that range should return 0 for
  // 10^0, which will have the impact of rounding to the nearest 10.
  if (log10Diff >= 0.0 && log10Diff <= static_cast<double>(significance))
    return 0;

  return static_cast<int>(floor(log10Diff));
}

void TMath::roundRanges(double& minValue, double& maxValue)
{
  // Round ranges algorithm taken from legacy Plot-XY:
  // Rounds the axis numbers to smooth numbers; general idea is that
  // ([ceil | floor](val * 10 ^ X)) / 10 ^ X == rounded number
  const double range = fabs(maxValue - minValue);
  if (range == 0.0)
    return;

  // Calculate the desired 10^X first
  const double powerVal = static_cast<double>(getPowerOfTenForSignificance(range, 2u));
  const double powerTen = pow(10.0, powerVal);
  // Do the formula: floor|ceil(num * 10^X) / 10^X to get significance of 2
  if (minValue < maxValue)
  {
    minValue = (floor(minValue * powerTen)) / powerTen;
    maxValue = (ceil(maxValue * powerTen)) / powerTen;
  }
  else
  {
    minValue = (ceil(minValue * powerTen)) / powerTen;
    maxValue = (floor(maxValue * powerTen)) / powerTen;
  }
}

void TMath::enuRotMatrixToEcefEuler(const osg::Matrix& in, SimVec3& out)
{
	// direction cosine matrix in ENU frame
	double enu_dcm[3][3] = {
		{ in(0,0), in(0,1), in(0,2) },
		{ in(1,0), in(1,1), in(1,2) },
		{ in(2,0), in(2,1), in(2,2) }
	};

	

	// convert DCM to NED frame:
	double ned_dcm[3][3];
	d3MMmult(NED_ENU, enu_dcm, ned_dcm);

	// and into Euler angles.
	d3DCMtoEuler(ned_dcm, out);
}

osg::Quat TMath::eulerRadToQuat(double h, double p, double r)
{


	// NOTE: OSG coordinate systems are all RIGHT-HANDED.
	// Here we create a series of quaternions based on the simCore
	// semantics for HPR, as detailed below.

	// +H is a "right turn", a right-handed rotation about the -Z axis:
	const osg::Quat azim_q = (!osg::equivalent(h, 0.0)) ? osg::Quat(h, osg::Vec3d(0, 0, -1)) : osg::Quat();

	// +P is "nose up"; a right-handed rotation about the +X axis:
	const osg::Quat pitch_q = (!osg::equivalent(p, 0.0)) ? osg::Quat(p, osg::Vec3d(1, 0, 0)) : osg::Quat();

	// +R is "right wing down", a right-handed rotation about the +Y axis:
	const osg::Quat roll_q = (!osg::equivalent(r, 0.0)) ? osg::Quat(r, osg::Vec3d(0, 1, 0)) : osg::Quat();

	// combine them in the reverse of the desired rotation order:
	// azim-pitch-roll
	return roll_q * pitch_q * azim_q;


}

void TMath::ecefEulerToEnuRotMatrix(const SimVec3& in, osg::Matrix& out)
{
	// first convert the ECEF orientation to a 3x3 matrix:
	double ned_dcm[3][3];
	d3EulertoDCM(in, ned_dcm);
	double enu_dcm[3][3];
	d3MMmult(NED_ENU, ned_dcm, enu_dcm);

	// poke the values into the OSG matrix:
	out.set(
		enu_dcm[0][0], enu_dcm[0][1], enu_dcm[0][2], 0.0,
		enu_dcm[1][0], enu_dcm[1][1], enu_dcm[1][2], 0.0,
		enu_dcm[2][0], enu_dcm[2][1], enu_dcm[2][2], 0.0,
		0.0, 0.0, 0.0, 1.0);
}

double TMath::sodanoInverse(const double refLat, const double refLon, const double refAlt, const double lat, const double lon, double *azfwd, double *azbck)
{
	// Reference:
	// E. M. Sodano and T. A. Robinson,
	// "Direct and Inverse Solutions in Geodesics Technical Report 7"
	// U.S. Army Map Service, Washington, DC 1963 pp. 15-27.
	if (refLat == lat && refLon == lon)
	{
		if (azfwd) *azfwd = 0;
		if (azbck) *azbck = 0;
		return 0.0;
	}

	const double reqtr = 6378137.0 + refAlt;
	const double rpolr = reqtr * (1.0 - 1.0 / 298.257223563);
	const double flat = 1. - (rpolr / reqtr);
	const double deltaLon = lon - refLon;
	const double beta1 = atan2((rpolr*sin(refLat)), (reqtr*cos(refLat)));
	const double beta2 = atan2((rpolr*sin(lat)), (reqtr*cos(lat)));
	const double sbet1 = sin(beta1);
	const double sbet2 = sin(beta2);
	const double cbet1 = cos(beta1);
	const double cbet2 = cos(beta2);
	const double sl = sin(deltaLon);
	const double sl2 = sin(0.5*deltaLon);

	const double a = sbet1*sbet2;
	const double b = cbet1*cbet2;
	const double cdel = a + b*cos(deltaLon);
	const double n = (reqtr - rpolr) / (reqtr + rpolr);
	const double b2mb1 = (lat - refLat) + 2.*(a*(n + n*n + n*n*n) - b*(n - n*n + n*n*n)) * sin(lat - refLat);

	const double d = sin(b2mb1) + 2.*cbet2*sbet1*sl2*sl2;
	const double sdel = sqrt(sl*sl*cbet2*cbet2 + d*d);
	const double delta = fabs(atan2(sdel, cdel));

	const double c = b*sl / sdel;
	const double m = 1. - c*c;
	const double f2 = flat*flat;
	const double d2 = delta*delta;

	if (azfwd || azbck)
	{
		// Forward and back azimuths
		const double lamda = deltaLon + c*((flat + f2)*delta - 0.5*a*f2*(sdel + 2.*d2 / sdel) +
			0.25*m*f2*(sdel*cdel - 5.*delta + 4.*d2 / tan(delta)));

		const double slam = sin(lamda);
		const double slam2 = sin(0.5*lamda);

		if (azfwd) *azfwd = atan2((cbet2*slam), (sin(b2mb1) + 2.*cbet2*sbet1*slam2*slam2));
		if (azbck) *azbck = atan2((-cbet1*slam), (2.*cbet1*sbet2*slam2*slam2 - sin(b2mb1)));
	}

	// Geodesic length
	return rpolr*((1. + flat + f2)*delta + a*((flat + f2)*sdel - f2*d2 / (2.*sdel))
		- 0.5*m*((flat + f2)*(delta + sdel*cdel) - f2*d2 / tan(delta))
		- 0.5*a*a*f2*sdel*cdel + (f2*m*m / 16.)
		*(delta + sdel*cdel - 2.*sdel*cdel*cdel*cdel - 8.*d2 / tan(delta))
		+ 0.5*a*m*f2*(sdel*cdel*cdel + d2 / sdel));
}

void TMath::sodanoDirect(const double refLat, const double refLon, const double refAlt, const double dist, const double azfwd, double *latOut, double *lonOut, double *azbck)
{
	// Reference:
	// E. M. Sodano and T. A. Robinson,
	// "Direct and Inverse Solutions in Geodesics Technical Report 7"
	// U.S. Army Map Service, Washington, DC 1963 pp. 15-27.
	assert(latOut || lonOut || azbck);
	if (!latOut && !lonOut && !azbck)
	{
		// SIM_ERROR << "sodanoDirect, invalid output params: " << __LINE__ << std::endl;
		return;
	}

	const double reqtr = 6378137.0 + refAlt;
	const double rpolr = reqtr * (1.0 - 1.0 / 298.257223563);
	const double flat = 1. - (rpolr / reqtr);
	const double ecc2 = (reqtr*reqtr - rpolr*rpolr) / (rpolr*rpolr);
	const double theta = dist / rpolr;
	const double beta1 = atan2(rpolr*sin(refLat), reqtr*cos(refLat));

	const double sbeta1 = sin(beta1);
	const double cbeta1 = cos(beta1);
	const double stheta = sin(theta);
	const double ctheta = cos(theta);
	const double saz = sin(azfwd);
	const double caz = cos(azfwd);

	const double g = cbeta1*caz;
	const double h = cbeta1*saz;

	const double m = (1. + 0.5*ecc2*sbeta1*sbeta1)*(1. - h*h)*0.5;
	const double n = (1. + 0.5*ecc2*sbeta1*sbeta1)*(ctheta*sbeta1*sbeta1 + g*sbeta1*stheta)*0.5;
	const double length = h*(-flat*theta + 3.*flat*flat*n*stheta + 3.*flat*flat*m*(theta - stheta*ctheta)*0.5);
	const double capm = m*ecc2;
	const double capn = n*ecc2;
	const double delta = theta - capn*stheta + 0.5*capm*(stheta*ctheta - theta) + (5. / 2.)*capn*capn*stheta*ctheta +
		(capm*capm / 16.)*(11.*theta - 13.*stheta*ctheta - 8.*theta*ctheta*ctheta + 10.*stheta*ctheta*ctheta*ctheta)
		+ 0.5*capm*capn*(3.*stheta + 2.*theta*ctheta - 5.*stheta*ctheta*ctheta);

	const double sdel = sin(delta);
	const double cdel = cos(delta);
	const double f = g*cdel - sbeta1*sdel;
	const double sbeta2 = sbeta1*cdel + g*sdel;
	const double cbeta2 = sqrt(h*h + f*f);
	const double lamda = atan2((sdel*saz), (cbeta1*cdel - sbeta1*sdel*caz));

	// Set second latitude and longitude point
	if (latOut) *latOut = atan2(reqtr*sbeta2, rpolr*cbeta2);
	if (lonOut) *lonOut = refLon + lamda + length;

	// Back azimuth
	if (azbck) *azbck = atan2(-h, (sbeta1*sdel - g*cdel));
}