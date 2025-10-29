/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup MapServer Projects/CoX/Servers/MapServer
 * @{
 */

#include "MapManager.h"
#include "GameData/map_definitions.h"
#include "Components/Logging.h"
#include "MapTemplate.h"

MapManager::MapManager( ) : m_max_instances(2)
{
}

MapManager::~MapManager()
{
    for(auto & v : m_templates)
    {
        delete v.second;
    }
}

//! \brief Loads all templates available in given directory, will populate m_templates attribute
bool MapManager::load_templates(const String &template_directory, uint8_t game_id, uint32_t map_id,
                                const ListenAndLocationAddresses &loc)
{
    auto fs = SEGS::getServiceLocator()->getFS();
    sInfo() << "Searching for maps in:" << template_directory;

    fs->visitEntries(template_directory, [&](StringView path, bool is_dir) -> SEGS::VisitResult {
        if (is_dir)
        {
            if (path != "." && path != "..")
                return SEGS::VisitResult::VisitSubdirectory;
            return SEGS::VisitResult::VisitNext;
        }
        // QString dirname = map_dir_visitor.next();
        StringView dirname = PathUtils::get_file(path);

        if (dirname.contains("City_", false) || dirname.contains("Hazard_", false) ||
            dirname.contains("Trial_", false))
        {
            auto tpl = new MapTemplate(path, game_id, map_id, loc, false);
            m_templates[getMapIndex(tpl->base_name())] = tpl;
            m_name_to_template[tpl->client_filename()] = tpl;

            sInfo() << "Found map:" << path;
        }

        sInfo() << "Directory: " << dirname;
        if (dirname.contains("Mission_", false))
        {
            // TODO: Change this to create a single template for each mission type.
            sInfo() << "Found Mission Map: " << path;
            // fileName: DefaultMapInstances/Mission_Sewers
            auto tpl = new MapTemplate(path, game_id, map_id, loc, true);
            m_templates[getMapIndex(tpl->mission_base_name())] = tpl; // Sewers, Caves, etc
            // For missions, the client filename is only partial, as it still need the level and layouts appended to
            // actually load
            m_name_to_template[tpl->client_filename()] = tpl; // maps/missions/sewers, maps/missions/caves, etc.
        }
        return SEGS::VisitResult::VisitNext;
    });

    // (template_directory / "bin/tutorial.bin")
    return !m_templates.empty();
}

//! \brief Retrieves template specified by it's client-side path
MapTemplate *MapManager::get_template(const String &_id)
{
    String id(_id);
    // Mission templates are stored without all their variations, so need to just get the base mission type.
    if (id.contains("mission"))
    {
        int start = StringView("maps/missions/").size();
        auto index = id.find("/", start);
        id = id.substr(0, index);
    }
    if(m_name_to_template.find(id)==m_name_to_template.end())
        return nullptr;
    return m_name_to_template[id];
}

size_t MapManager::num_templates()
{
    return m_templates.size();
}

size_t MapManager::max_instances()
{
    return m_max_instances;
}

void MapManager::shut_down_all()
{
    for(eastl::pair<const uint32_t,MapTemplate *> &entry : m_templates)
    {
        entry.second->shut_down_all();
    }
}

//! @}
