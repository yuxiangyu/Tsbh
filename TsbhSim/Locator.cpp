

#include "Angle.h"
#include "CoordinateConverter.h"
//#include "simVis/Utils.h"
#include "Locator.h"
#include "Math1.h"
#undef LC
#define LC "[Locator] "

Locator::Locator()
  : componentsToInherit_(COMP_ALL),
  rotOrder_(HPR),
  isEmpty_(true),
  ecefCoordIsSet_(false),
  hasRotation_(false),
  offsetsAreSet_(false),
  timestamp_(std::numeric_limits<double>::max()),
  eciRefTime_(std::numeric_limits<double>::max()),
  eciRotationTime_(0.)
{
  ecefCoord_.setCoordinateSystem(COORD_SYS_ECEF);
}

Locator::Locator(Locator* parentLoc, unsigned int inheritMask)
  : rotOrder_(HPR),
  isEmpty_(false),
  ecefCoordIsSet_(false),
  hasRotation_(false),
  offsetsAreSet_(false),
  timestamp_(std::numeric_limits<double>::max()),
  eciRefTime_(std::numeric_limits<double>::max()),
  eciRotationTime_(0.)
{
  setParentLocator(parentLoc, inheritMask);
  ecefCoord_.setCoordinateSystem(COORD_SYS_ECEF);
}

bool Locator::isEmpty() const
{
  // locator is empty when it and all parents are empty
  return (isEmpty_ && (!parentLoc_.valid() || parentLoc_->isEmpty()));
}

bool Locator::isEci() const
{
  // locator is Eci when it or any parent has ECI rotation
  return (hasRotation_ || (parentLoc_.valid() && parentLoc_->isEci()));
}

void Locator::setParentLocator(Locator* newParent, unsigned int inheritMask, bool notify)
{
  if (newParent == this)
  {
    osg::notify(osg::WARN)
      << "Locator: illegal state, Locator cannot self-parent"
      << std::endl;
    return;
  }

  if (newParent == nullptr)
  {
    // remove me from my old parent's child list:
    if (parentLoc_.valid())
      parentLoc_->children_.erase(this);
  }

  parentLoc_ = newParent;
  componentsToInherit_ = inheritMask;

  if (newParent)
    newParent->children_.insert(this);

  if (notify)
    notifyListeners_();
}

void Locator::setComponentsToInherit(unsigned int value, bool notify)
{
  componentsToInherit_ = value;

  if (notify)
    notifyListeners_();
}

//void Locator::setCoordinate(const Coordinate& coord, bool notify)
//{
//  if (coord.coordinateSystem() != simCore::COORD_SYS_ECEF)
//  {
//    simCore::CoordinateConverter conv;
//    conv.convert(coord, ecefCoord_, simCore::COORD_SYS_ECEF);
//  }
//  else
//  {
//    ecefCoord_ = coord;
//  }
//  if (coord.elapsedEciTime() != 0)
//    timestamp_ = coord.elapsedEciTime() + getEciRefTime();
//
//  isEmpty_ = false;
//
//  ecefCoordIsSet_ = true;
//
//  if (notify)
//    notifyListeners_();
//}

void Locator::setCoordinate(const Coordinate& coord, double timestamp, double eciRefTime, bool notify)
{
  timestamp_ = timestamp;
  // Make sure we aren't overwriting a potentially good reference time with the default value
  if (eciRefTime != std::numeric_limits<double>::max())
    setEciRefTime(eciRefTime);

  if (coord.coordinateSystem() != COORD_SYS_ECEF)
  {
    Coordinate temp = coord;
    // Ignore whatever is in the coordinate's ECI time and instead use the internal reference time and timestamp
    temp.setElapsedEciTime(getElapsedEciTime());
    CoordinateConverter conv;
    conv.convert(temp, ecefCoord_, COORD_SYS_ECEF);
  }
  else
  {
    ecefCoord_ = coord;
  }

  isEmpty_ = false;

  ecefCoordIsSet_ = true;

  if (notify)
    notifyListeners_();
}

void Locator::setEciRotationTime(double rotationTime, double timestamp, bool notify)
{
  timestamp_ = timestamp;
  hasRotation_ = true;
  eciRotationTime_ = rotationTime;
  isEmpty_ = false;
  if (notify)
    notifyListeners_();
}

void Locator::setLocalOffsets(const SimVec3& pos, const SimVec3& ori, double timestamp, bool notify)
{
  offsetPos_ = pos;
  offsetOri_ = ori;
  offsetsAreSet_ =
      pos.x()   != 0.0 || pos.y()     != 0.0 || pos.z()    != 0.0 ||
      ori.yaw() != 0.0 || ori.pitch() != 0.0 || ori.roll() != 0.0;

  if (timestamp != std::numeric_limits<double>::max())
  {
    timestamp_ = timestamp;
    isEmpty_ = false; // mark this locator as non-empty, since it has a timestamp
  }
  else if (offsetsAreSet_)
    isEmpty_ = false;

  if (notify)
    notifyListeners_();
}

bool Locator::getCoordinate(Coordinate* out_coord, const CoordinateSystem& coordsys) const
{
  if (!out_coord || isEmpty())
    return false;

  Coordinate temp = ecefCoord_;

  if (!ecefCoordIsSet_ && parentLoc_.valid() && componentsToInherit_ != 0)
  {
    Coordinate parent;
    if (parentLoc_->getCoordinate(&parent))
    {
      if ((componentsToInherit_ & COMP_POSITION) != COMP_NONE)
      {
        temp.setPosition(parent.position());
      }
      if ((componentsToInherit_ & COMP_ORIENTATION) != COMP_NONE)
      {
        temp.setOrientation(parent.orientation());
      }
    }
  }
  temp.setElapsedEciTime(getElapsedEciTime());

  if (coordsys != ecefCoord_.coordinateSystem())
  {
    CoordinateConverter conv;
    conv.convert(temp, *out_coord, coordsys);
  }
  else
  {
    *out_coord = temp;
  }

  return true;
}

bool Locator::getLocalOffsets(SimVec3& pos, SimVec3& ori) const
{
  if (!offsetsAreSet_ || isEmpty())
  {
    pos.set(0, 0, 0);
    ori.set(0, 0, 0);
    return false;
  }

  pos = offsetPos_;
  ori = offsetOri_;
  return true;
}

void Locator::endUpdate()
{
  notifyListeners_();
}

void Locator::setTime(double stamp, bool notify)
{
  timestamp_ = stamp;
  isEmpty_ = false;
}

bool Locator::setEciRefTime(double eciRefTime)
{
  eciRefTime_ = eciRefTime;
  return true;
}

double Locator::getTime() const
{
  // if no valid timestamps, return 0.
  double mostRecentTime = -std::numeric_limits<double>::max();
  for (const Locator* loc = this; loc != nullptr; loc = loc->getParentLocator())
  {
    const double locatorTime = loc->timestamp_;
    if (locatorTime != std::numeric_limits<double>::max() && locatorTime > mostRecentTime)
      mostRecentTime = locatorTime;
  }
  return (mostRecentTime != -std::numeric_limits<double>::max()) ? mostRecentTime : 0.;
}

double Locator::getEciRefTime() const
{
  // traverse up through parents to find first set ECI reference time
  for (const Locator* loc = this; loc != nullptr; loc = loc->getParentLocator())
  {
    if (loc->eciRefTime_ != std::numeric_limits<double>::max())
      return loc->eciRefTime_;
  }
  return 0.;
}

double Locator::getElapsedEciTime() const
{
  // find the first locator that has a set value for eci reference time
  for (const Locator* loc = this; loc != nullptr; loc = loc->getParentLocator())
  {
    if (loc->eciRefTime_ != std::numeric_limits<double>::max())
    {
      // use loc's eci reference time, with the most recent time of loc or loc's parents
      return (loc->getTime() - loc->eciRefTime_);
    }
  }
  return getTime();
}

//bool Locator::inherits_(unsigned int mask) const
//{
//  return (componentsToInherit_ & mask) != COMP_NONE;
//}

bool Locator::getLocatorPosition(SimVec3* out_position, const CoordinateSystem& coordsys) const
{
  if (!out_position)
    return false;

  osg::Matrix m;
  if (!getLocatorMatrix(m))
    return false;
  const osg::Vec3d& ecefPos = m.getTrans();

  if (coordsys == COORD_SYS_ECEF)
  {
    out_position->set(ecefPos.x(), ecefPos.y(), ecefPos.z());
    return true;
  }
  if (coordsys == COORD_SYS_LLA)
  {
    CoordinateConverter::convertEcefToGeodeticPos(SimVec3(ecefPos.x(), ecefPos.y(), ecefPos.z()), *out_position);
    return true;
  }
  if (coordsys == COORD_SYS_ECI)
  {
    const Coordinate in(COORD_SYS_ECEF, SimVec3(ecefPos.x(), ecefPos.y(), ecefPos.z()), getElapsedEciTime());
    Coordinate out;
    CoordinateConverter::convertEcefToEci(in, out);
    *out_position = out.position();
    return true;
  }
  // unsupported coordsys
  return false;
}

bool Locator::getLocatorPositionOrientation(SimVec3* out_position, SimVec3* out_orientation, const CoordinateSystem& coordsys) const
{
  if ((!out_position) || (!out_orientation))
    return false;

  osg::Matrix m;
  if (!getLocatorMatrix(m))
    return false;

  const osg::Vec3d& v = m.getTrans();
  out_position->set(v.x(), v.y(), v.z());
  TMath::enuRotMatrixToEcefEuler(m, *out_orientation);

  if (coordsys == COORD_SYS_ECEF)
    return true;
  if (coordsys == COORD_SYS_LLA)
  {
    const Coordinate in(COORD_SYS_ECEF, *out_position, *out_orientation);
   Coordinate out;
    CoordinateConverter::convertEcefToGeodetic(in, out);
    *out_position = out.position();
    *out_orientation = out.orientation();
    return true;
  }
  if (coordsys == COORD_SYS_ECI)
  {
    const Coordinate in(COORD_SYS_ECEF, *out_position, *out_orientation, getElapsedEciTime());
    Coordinate out;
    CoordinateConverter::convertEcefToEci(in, out);
    *out_position = out.position();
    *out_orientation = out.orientation();
    return true;
  }
  // unsupported coordsys
  return false;
}

void Locator::applyOffsets_(osg::Matrixd& output, unsigned int comps) const
{
  // Start by collecting any offsets in this Locator's parent, recursively
  if (parentLoc_.valid())
  {
    parentLoc_->applyOffsets_(output, (comps & componentsToInherit_));
  }

  // now apply this locator's own offsets
  applyLocalOffsets_(output, comps);
}

void Locator::applyLocalOffsets_(osg::Matrixd& output, unsigned int comps) const
{
  if (!offsetsAreSet_)
    return;

  const bool haveOriOffset = ((comps & COMP_ORIENTATION) != COMP_NONE) && (offsetOri_.yaw() != 0 || offsetOri_.pitch() != 0 || offsetOri_.roll() != 0);
  const bool havePosOffset = ((comps & COMP_POSITION) != COMP_NONE) && (offsetPos_.x() != 0 || offsetPos_.y() != 0 || offsetPos_.z() != 0);

  if (havePosOffset)
  {
    output.preMultTranslate(osg::Vec3d(offsetPos_.x(), offsetPos_.y(), offsetPos_.z()));
  }

  if (haveOriOffset)
  {
    if ((comps & COMP_ORIENTATION) == COMP_ORIENTATION)
    {
      const osg::Quat& oq = TMath::eulerRadToQuat(offsetOri_.yaw(), offsetOri_.pitch(), offsetOri_.roll());
      output.preMultRotate(oq);
    }
    else
    {
      // partial (not all components requested)
      SimVec3 partialOri(
        (comps & COMP_HEADING) != COMP_NONE ? offsetOri_.yaw() : 0.0,
        (comps & COMP_PITCH) != COMP_NONE ? offsetOri_.pitch() : 0.0,
        (comps & COMP_ROLL) != COMP_NONE ? offsetOri_.roll() : 0.0);

      const osg::Quat& oq = TMath::eulerRadToQuat(partialOri.yaw(), partialOri.pitch(), partialOri.roll());
      output.preMultRotate(oq);
    }
  }
}

bool Locator::getLocatorMatrix(osg::Matrixd& output, unsigned int comps) const
{
  osg::Vec3d pos;
  const bool posFound = getPosition_(pos, comps);

  if (getOrientation_(output, comps))
  {
    if (posFound)
      output.postMultTranslate(pos);

    osg::Matrixd rotation;
    if (getRotation_(rotation))
      output.postMult(rotation);
  }
  else if (getRotation_(output))
  {
    if (posFound)
      output.preMultTranslate(pos);
  }
  else if (posFound)
  {
    computeLocalToWorldTransformFromXYZ_(pos, output);
  }
  applyOffsets_(output, comps);
  return true;
}

osg::Matrixd Locator::getLocatorMatrix(unsigned int comps) const
{
  osg::Matrixd output;
  getLocatorMatrix(output, comps);
  return output;
}

double Locator::getEciRotationTime() const
{
  if (isEmpty())
    return 0.;

  // sum all rotations of this and all parents
  double rotationSum = 0.;
  for (const Locator* loc = this; loc != nullptr; loc = loc->getParentLocator())
  {
    if (loc->hasRotation_)
      rotationSum += loc->eciRotationTime_;
  }
  return rotationSum;
}

bool Locator::getRotation_(osg::Matrixd& rotationMatrix) const
{
  // get the sum all rotations of this and all parents
  const double rotationSum = getEciRotationTime();
  if (rotationSum == 0.)
    return false;
  const double eciRotation = angFix2PI(EARTH_ROTATION_RATE * rotationSum);
  rotationMatrix *= osg::Matrixd::rotate(-eciRotation, osg::Z_AXIS);
  return true;
}

bool Locator::getPosition_(osg::Vec3d& pos, unsigned int comps) const
{
  if (isEmpty())
    return false;

  comps &= componentsToInherit_;
  if ((comps & COMP_POSITION) == COMP_NONE)
    return false;

  if (ecefCoordIsSet_)
  {
    pos = osg::Vec3d(ecefCoord_.x(), ecefCoord_.y(), ecefCoord_.z());
    return true;
  }

  if (getParentLocator())
    return getParentLocator()->getPosition_(pos, comps);

  return false;
}

bool Locator::getOrientation_(osg::Matrixd& ori, unsigned int comps) const
{
  if ((comps & COMP_ORIENTATION) == COMP_NONE || isEmpty())
    return false;

  if (!ecefCoordIsSet_)
  {
    if (getParentLocator())
      return getParentLocator()->getOrientation_(ori, comps & getComponentsToInherit());
  }
  else if (ecefCoord_.hasOrientation())
  {
    // find the base orientation, then apply offsets
    if ((comps & COMP_ORIENTATION) == COMP_ORIENTATION)
    {
      // easy, use all orientation components
		TMath::ecefEulerToEnuRotMatrix(ecefCoord_.orientation(), ori);
      return true;
    }
    else
    {
      // above, this condition is reason for early exit (if no orientation information is requested in comps)
      // if assert fails, check for changes to the early exit test
      assert((comps & COMP_ORIENTATION) != 0);
      // painful: need to convert to body-local, remove unwanted components,
      // and convert back to ECEF.
      CoordinateConverter conv;
     Coordinate lla;
      conv.convert(ecefCoord_, lla, COORD_SYS_LLA);
      assert(lla.hasOrientation());
      lla.setOrientation(
        (comps & COMP_HEADING) != 0 ? lla.yaw() : 0.0,
        (comps & COMP_PITCH) != 0 ? lla.pitch() : 0.0,
        (comps & COMP_ROLL) != 0 ? lla.roll() : 0.0);

      Coordinate ecef;
      conv.convert(lla, ecef, COORD_SYS_ECEF);
	  TMath::ecefEulerToEnuRotMatrix(ecef.orientation(), ori);
      return true;
    }
  }
  return false;
}

void Locator::computeLocalToWorldTransformFromXYZ_(const osg::Vec3d& ecefPos, osg::Matrixd& local2world) const
{
  local2world.makeTranslate(ecefPos);

  SimVec3 llaPos;
  CoordinateConverter::convertEcefToGeodeticPos(SimVec3(ecefPos.x(), ecefPos.y(), ecefPos.z()), llaPos);

  double rotationMatrixENU_[3][3];
  CoordinateConverter::setLocalToEarthMatrix(llaPos.lat(), llaPos.lon(), LOCAL_LEVEL_FRAME_ENU, rotationMatrixENU_);

  // set matrix
  local2world(0,0) = rotationMatrixENU_[0][0];
  local2world(0,1) = rotationMatrixENU_[0][1];
  local2world(0,2) = rotationMatrixENU_[0][2];

  local2world(1,0) = rotationMatrixENU_[1][0];
  local2world(1,1) = rotationMatrixENU_[1][1];
  local2world(1,2) = rotationMatrixENU_[1][2];

  local2world(2,0) = rotationMatrixENU_[2][0];
  local2world(2,1) = rotationMatrixENU_[2][1];
  local2world(2,2) = rotationMatrixENU_[2][2];
}

void Locator::addCallback(LocatorCallback* callback)
{
  callbacks_.push_back(callback);
}

void Locator::removeCallback(LocatorCallback* callback)
{
  for (std::vector< osg::ref_ptr<LocatorCallback> >::iterator i = callbacks_.begin(); i != callbacks_.end(); ++i)
  {
    if (i->get() == callback)
    {
      callbacks_.erase(i);
      break;
    }
  }
}

void Locator::notifyListeners_()
{
  dirty();

  for (std::vector< osg::ref_ptr<LocatorCallback> >::iterator i = callbacks_.begin(); i != callbacks_.end();)
  {
    LocatorCallback* cb = i->get();
    if (cb)
    {
      (*cb)(this);
      ++i;
    }
    else
    {
      i = callbacks_.erase(i);
    }
  }

  for (std::set< osg::observer_ptr<Locator> >::iterator i = children_.begin(); i != children_.end();)
  {
    Locator* child = i->get();
    if (child)
    {
      child->notifyListeners_();
      ++i;
    }
    else
    {
      // erase(i++) will not work for vector, but does work for std::set
      children_.erase(i++);
    }
  }
}

//---------------------------------------------------------------------------

CachingLocator::CachingLocator()
  : Locator()
{}

CachingLocator::CachingLocator(Locator* parentLoc, unsigned int inheritMask)
  : Locator(parentLoc, inheritMask)
{}

bool CachingLocator::getLocatorPosition(SimVec3* out_position, const CoordinateSystem& coordsys) const
{
  if (!out_position)
    return false;

  if (coordsys != COORD_SYS_LLA)
    return Locator::getLocatorPosition(out_position, coordsys);

  // use the cached lla position if it is valid
  assert(coordsys == COORD_SYS_LLA);
  if (inSyncWith(llaPositionCacheRevision_))
  {
    *out_position = llaPositionCache_;
    return true;
  }
  if (Locator::getLocatorPosition(out_position, coordsys))
  {
    llaPositionCache_ = *out_position;
    sync(llaPositionCacheRevision_);
    return true;
  }
  return false;
}

bool CachingLocator::getLocatorPositionOrientation(SimVec3* out_position, SimVec3* out_orientation, const CoordinateSystem& coordsys) const
{
  if ((!out_position) || (!out_orientation))
    return false;

  if (coordsys != COORD_SYS_LLA)
    return Locator::getLocatorPositionOrientation(out_position, out_orientation, coordsys);

  // use the cached lla position & orientation if they are valid
  assert(coordsys == COORD_SYS_LLA);
  if (inSyncWith(llaPositionCacheRevision_) && inSyncWith(llaOrientationCacheRevision_))
  {
    *out_position = llaPositionCache_;
    *out_orientation = llaOrientationCache_;
    return true;
  }
  if (Locator::getLocatorPositionOrientation(out_position, out_orientation, coordsys))
  {
    llaPositionCache_ = *out_position;
    llaOrientationCache_ = *out_orientation;
    sync(llaPositionCacheRevision_);
    sync(llaOrientationCacheRevision_);
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------

ResolvedPositionOrientationLocator::ResolvedPositionOrientationLocator()
  : Locator()
{}

ResolvedPositionOrientationLocator::ResolvedPositionOrientationLocator(Locator* parentLoc, unsigned int inheritMask)
  : Locator(parentLoc, inheritMask)
{}

// ignores comps arg, since children's comps do not affect the resolved position (but do affect subsequent offsets)
bool ResolvedPositionOrientationLocator::getPosition_(osg::Vec3d& pos, unsigned int comps) const
{
  osg::Matrixd mat;
  // resolved position is not modified by children's inheritance orientation components
  if (getParentLocator() && getParentLocator()->getLocatorMatrix(mat, getComponentsToInherit()))
  {
    // strip out orientation and scale; does not strip out rotation.
    pos = mat.getTrans();
    return true;
  }
  return false;
}
bool ResolvedPositionOrientationLocator::getRotation_(osg::Matrixd& rotationMatrix) const
{
  // rotation already included by getPosition_
  return false;
}

// only apply local offsets
// do not apply parent offsets, since they have already been processed to produce the resolved position
void ResolvedPositionOrientationLocator::applyOffsets_(osg::Matrixd& output, unsigned int comps) const
{
  // apply only this locator's own offsets
  applyLocalOffsets_(output, comps);
}

//---------------------------------------------------------------------------

ResolvedPositionLocator::ResolvedPositionLocator()
  : ResolvedPositionOrientationLocator()
{}

ResolvedPositionLocator::ResolvedPositionLocator(Locator* parentLoc, unsigned int inheritMask)
  : ResolvedPositionOrientationLocator(parentLoc, inheritMask)
{}

// strip out all orientation components, so that this locator returns a position with an identity orientation
bool ResolvedPositionLocator::getOrientation_(osg::Matrixd& ori, unsigned int comps) const
{
  return false;
}

