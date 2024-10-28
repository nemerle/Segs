/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */
#pragma once
#include "Common/Containers/Vector.h"
#include <EASTL/functional.h>
#include <stdint.h>
/*!
 * @addtogroup Components
 * @{
 */
class EventSrc;
namespace SEGSEvents
{
class Event;
void register_event_type(const char *name, uint32_t type_id, eastl::function<Event *()> constructor);
Event *create_by_id(uint32_t type_id, EventSrc *src=nullptr);
Event *create_by_name(const char* name,EventSrc *src=nullptr);
const char *event_name(uint32_t type_id);
Event *from_storage(const Vector<uint8_t> &istr);
void to_storage(Vector<uint8_t> &ostr,Event *ev);
[[nodiscard]] inline Vector<uint8_t> to_storage(Event *ev) {
    Vector<uint8_t> ostr;
    to_storage(ostr,ev);
    return ostr;
}
}
//! @}
