/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/GameData/Contact.h"
#include "MapSceneGraph.h"

struct MapClientSession;
class MapInstance;
class ScriptingEnginePrivate;
class Entity;

class ScriptingEngine
{
public:
    ScriptingEngine();
    ~ScriptingEngine();
    void registerTypes();
    void register_GenericTypes();
    void register_CharacterTypes();
    void register_SpawnerTypes();
    int loadAndRunFile(const String &path);
    void callFuncWithMapInstance(MapInstance *mi, const char *name, int arg1);
    String callFuncWithClientContext(MapClientSession *client,const char *name,int arg1);
    String callFuncWithClientContext(MapClientSession *client,const char *name,int arg1, glm::vec3 loc);
    String callFuncWithClientContext(MapClientSession *client, const char *name, const char *arg1, glm::vec3 loc);
    String callFunc(const char *name,int arg1);
    String callFunc(const char *name,int arg1, glm::vec3 loc);
    String callFunc(const char *name, const char *arg1, glm::vec3 loc);
    String callFunc(const char *name, const Vector<Contact> &contact_list);
    void updateMapInstance(MapInstance * instance);
    void updateClientContext(MapClientSession * client);
    int runScript(const String &script_contents,const char *script_name="unnamed script");
    int runScript(MapClientSession *client,const String &script_contents,const char *script_name="unnamed script");
    bool setIncludeDir(const String &path);
private:
    eastl::unique_ptr<ScriptingEnginePrivate> m_private;

    MapInstance *mi;
    MapClientSession *cl;
    Entity *e;

};
