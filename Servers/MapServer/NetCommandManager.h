/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Components/BitStream.h"
#include "Containers/HashMap.h"
#include "Containers/Vector.h"

#include <ace/Singleton.h>
#include <ace/Thread_Mutex.h>

struct MapClientSession;

class NetCommand
{
    float normalizedCircumferenceToFloat(int number,int numbits)
    {
        // something like this : ((number*3.141592)/(1<<numbits))-3.141592
        float f=((float(number)*3.141592f)/(1<<numbits))-3.141592f;
        return f;
    }
public:

    struct Argument
    {
        int type;
        void *targetvar;
    };
    NetCommand(int acl,const String &name,Vector<Argument> &args):m_arguments(args)
    {
        m_required_access_level=acl;
        m_name=name;
    }
    int              serializefrom(BitStream &bs);
    int              clientside_idx;
    int              m_required_access_level;
    String           m_name;
    Vector<Argument> m_arguments;

};

class NetCommandManager
{
using   vNetCommand = Vector<NetCommand *>;

        HashMap<String, NetCommand *> m_name_to_command;
        vNetCommand                  m_commands_level0;
        void                         serializeto(BitStream &tgt, const vNetCommand &commands);

public:
        void        UpdateCommandShortcuts(MapClientSession *client, Vector<String> &commands);
        NetCommand *getCommandByName(const String &name);
        void        addCommand(NetCommand *cmd);
};
typedef ACE_Singleton<NetCommandManager,ACE_Thread_Mutex> NetCommandManagerSingleton; // AdminServer Interface
