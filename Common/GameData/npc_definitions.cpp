#include "npc_definitions.h"
#include "Utils/string_utils.h"

namespace
{
struct BodyTypeName
{
    BodyType type;
    const char *m_ent_typename;
    const char *m_tex_prefix; // used to build texture names for costume parts
};
// note: original game was using a fixed set of possible BodyTypes, maybe we should consider making this more flexible
// using a dictionary ?
static const BodyTypeName s_BodyTypes[7] =
{
  { BodyType::Male, "male", "SM" },
  { BodyType::Female, "fem", "SF" },
  { BodyType::BasicMale, "bm", "BM" },
  { BodyType::BasicFemale, "bf", "BF" },
  { BodyType::Huge, "huge", "SH" },
  { BodyType::Enemy, "enemy", "EY" },
  { BodyType::Villain, "enemy", "EY" }
};
} // end of anonymous namespace

BodyType bodyTypeForEntType(const String &enttypename)
{
    for (const BodyTypeName &bdt : s_BodyTypes  )
    {
        if ( 0==StringUtils::compare(enttypename,bdt.m_ent_typename,StringUtils::CaseInsensitive) )
            return bdt.type;
    }
    return BodyType::Male;
}

String entTypeFileName(const Parse_Costume *costume)
{
    if (!costume->m_EntTypeFile.empty())
    {
        assert(costume->m_BodyType == bodyTypeForEntType(costume->m_EntTypeFile));
        return costume->m_EntTypeFile;
    }

    if (costume->m_BodyType != BodyType::Villain)
        return s_BodyTypes[int(costume->m_BodyType)].m_ent_typename;

    return String();
}
String bodytype_prefix_fixup(const Parse_Costume *a1, const String &a2)
{
    String name;
    String str;

    if ( a1->m_BodyType == BodyType::Villain )
    {
        name = entTypeFileName(a1);
        str = name+"_"+a2+".tga";
    }
    else
    {
        str = String(s_BodyTypes[(int)a1->m_BodyType].m_tex_prefix) + "_"+a2+".tga";
    }
    return str.left(str.size()-4);
}
