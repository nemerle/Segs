#include "trick_definitions.h"
#include "Common/GameData/anim_definitions.h"
#include "Components/Logging.h"

GeometryModifiers *findGeomModifier(SceneModifiers &tricks,StringView modelname, StringView trick_path)
{
    Vector<StringView> parts;
    String::split_ref(parts,modelname,"__");
    if( parts.size()<2 )
        return nullptr;
    parts.pop_front();
    String bone_trick_name = String::joined(parts,"__");
    GeometryModifiers *result = tricks.g_tricks_string_hash_tab.at(bone_trick_name.to_lower(),nullptr);
    if( result )
        return result;
    sDebug() << "Can't find modifier for" << trick_path<<modelname;
    return nullptr;
}
