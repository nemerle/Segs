#pragma once
#include "HandleBasedStorage.h"
#include "Common/Containers/String.h"


namespace SEGS
{
struct AnimTrack;
using HAnimationTrack = SingularStoreHandleT<20,12,AnimTrack>;
struct GeoSet;
}

SEGS::HAnimationTrack getOrLoadAnimationTrack(const String &name);
SEGS::GeoSet *animLoad(const String &filename, bool background_load =false, bool header_only =false);
