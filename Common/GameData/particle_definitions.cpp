#include "particle_definitions.h"

void cleanupPSystemName(String &name)
{
    auto idx = name.find("/FX/");
    if(idx==String::npos && name.starts_with("FX/"))
        idx=0;
    if(idx!=String::npos)
    {
        name = name.substr(idx+3).to_upper();
    }
}

