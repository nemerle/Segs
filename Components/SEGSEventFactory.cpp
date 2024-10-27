#include "SEGSEventFactory.h"

#include "Components/SEGSEvent.h"
#include "Components/Logging.h"

#include "Common/Containers/HashMap.h"
#include "Common/Containers/Vector.h"
#include <cassert>
#include <cereal/archives/memory_binary.hpp>
namespace
{
    struct EventDescriptorEntry
    {
        eastl::function<SEGSEvents::Event *()> constructor;
        const char *name;
        uint32_t type_id;
    };
    Vector<EventDescriptorEntry> s_all_event_descriptors;
    HashMap<String,size_t> s_name_to_event_descriptor;
    HashMap<uint32_t,size_t> s_id_to_event_descriptor;
} // end of anonymous namespace
namespace SEGSEvents
{
void register_event_type(const char *name, uint32_t type_id, eastl::function<Event *()> constructor)
{
    assert(s_name_to_event_descriptor.find(name)==s_name_to_event_descriptor.end());
    assert(s_id_to_event_descriptor.find(type_id)==s_id_to_event_descriptor.end());
    s_all_event_descriptors.push_back({constructor,name,type_id});
    s_name_to_event_descriptor[name] = s_all_event_descriptors.size()-1;
    s_id_to_event_descriptor[type_id] = s_all_event_descriptors.size()-1;
}

Event *create_by_id(uint32_t type_id,EventSrc *src)
{
    auto iter = s_id_to_event_descriptor.find(type_id);
    if(iter==s_id_to_event_descriptor.end())
        return nullptr;
    auto res = s_all_event_descriptors[iter->second].constructor();
    res->src(src);
    return res;
}
Event *create_by_name(const char *name, EventSrc *src)
{
    auto iter = s_name_to_event_descriptor.find(name);
    if(iter==s_name_to_event_descriptor.end())
        return nullptr;
    auto res = s_all_event_descriptors[iter->second].constructor();
    res->src(src);
    return res;
}
const char *event_name(uint32_t type_id)
{
    auto iter = s_id_to_event_descriptor.find(type_id);
    if(iter==s_id_to_event_descriptor.end())
        return nullptr;
    return s_all_event_descriptors[iter->second].name;
}
Event *from_storage(const Vector<uint8_t> &istr)
{
    Event * ev;
    uint32_t type_id;
    // we create a Vector input to read the type_id;
    {
        cereal::VectorInputArchive iarchive(istr);
        iarchive(type_id);
        ev = create_by_id(type_id);
    }
    if(!ev)
    {
        sCritical()<<"Unknown Event type" << type_id;
        return nullptr;
    }
    ev->serialize_from(istr);
    return ev;
}
void to_storage(Vector<uint8_t> &ostr, Event *ev)
{
    assert(ev);
    uint32_t type_id = ev->type();
    // we create a Vector input to read the type_id;
    {
        cereal::VectorOutputArchive oarchive(ostr);
        oarchive(type_id);
        ev = create_by_id(type_id);
    }
    ev->do_serialize(ostr);
}
}
