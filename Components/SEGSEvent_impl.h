#pragma once

#include "Components/SEGSEvent.h"

#include "cereal/archives/memory_binary.hpp"

namespace SEGSEvents
{

#define EVENT_IMPL(name)\
template<class Archive>\
    void serialize(Archive & archive); \
    void do_serialize(Vector<uint8_t> &os) override {\
        cereal::VectorOutputArchive oarchive(os);\
        oarchive(*this);\
}\
    void serialize_from(const Vector<uint8_t> &os) override {\
        cereal::VectorInputArchive iarchive(os);\
        iarchive(*this);\
}\
    ~name() override = default;


// [[ev_def:type]]
struct Finish final: public Event
{
public:
    Finish(EventSrc *source=nullptr) : Event(evFinish,source) {}
    static  Finish *    s_instance;
    EVENT_IMPL(Finish)
};
} // end of SEGSEventsNamespace
