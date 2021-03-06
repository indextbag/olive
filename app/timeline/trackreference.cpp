/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2019 Olive Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#include "trackreference.h"

TrackReference::TrackReference() :
  type_(kTrackTypeNone),
  index_(0)
{
}

TrackReference::TrackReference(const TrackType &type, const int &index) :
  type_(type),
  index_(index)
{
}

const TrackType &TrackReference::type() const
{
  return type_;
}

const int &TrackReference::index() const
{
  return index_;
}

bool TrackReference::operator==(const TrackReference &ref) const
{
  return type_ == ref.type_ && index_ == ref.index_;
}
