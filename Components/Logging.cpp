/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup Components
 * @{
 */

#include "Components/Logging.h"
#include "Components/Settings.h"
#include "Utils/IServiceLocator.h"
#include "Utils/string_utils.h"

Vector<LoggingCategory *>  LoggingCategory::m_registered_categories;

static constexpr int categoryToLogLevel(SegsLogLevel level) {
    switch(level) {

    case SegsLogLevel::Debug:
        return 0;
    case SegsLogLevel::Info:
        return 1;
    case SegsLogLevel::Warning:
        return 2;
    case SegsLogLevel::Critical:
        return 3;
    }
    return 4;
}
DebugOutput::~DebugOutput() {
    thread_local String buffer;
    // write out the message
    if(m_buffer.empty())
        return;
    auto sl=SEGS::getServiceLocator();
    auto logger = sl ? sl->getLogger() : nullptr;

    buffer.clear();
    if(m_category!=nullptr)
    {
        buffer=m_category;
        buffer.push_back(':');
    }
    buffer.append(m_buffer);

    if(!logger) {
        fprintf(stderr,"NO_LOGGER:%s",buffer.c_str());
    } else {
        logger->logString(categoryToLogLevel(m_level),buffer.c_str());
    }
}

#define SEGS_LOGGING_CATEGORY(name, string) \
    LoggingCategory &name() \
    { \
        static LoggingCategory category(string); \
        return category; \
    }

SEGS_LOGGING_CATEGORY(logLogging,      "log.logging")
SEGS_LOGGING_CATEGORY(logKeybinds,     "log.keybinds")
SEGS_LOGGING_CATEGORY(logSettings,     "log.settings")
SEGS_LOGGING_CATEGORY(logGUI,          "log.gui")
SEGS_LOGGING_CATEGORY(logTeams,        "log.teams")
SEGS_LOGGING_CATEGORY(logDB,           "log.db")
SEGS_LOGGING_CATEGORY(logInput,        "log.input")
SEGS_LOGGING_CATEGORY(logPosition,     "log.position")
SEGS_LOGGING_CATEGORY(logOrientation,  "log.orientation")
SEGS_LOGGING_CATEGORY(logMovement,     "log.movement")
SEGS_LOGGING_CATEGORY(logChat,         "log.chat")
SEGS_LOGGING_CATEGORY(logInfoMsg,      "log.infomsg")
SEGS_LOGGING_CATEGORY(logEmotes,       "log.emotes")
SEGS_LOGGING_CATEGORY(logTarget,       "log.target")
SEGS_LOGGING_CATEGORY(logCharSel,      "log.charsel")
SEGS_LOGGING_CATEGORY(logPlayerSpawn,  "log.playerspawn")
SEGS_LOGGING_CATEGORY(logNpcSpawn,     "log.npcspawn")
SEGS_LOGGING_CATEGORY(logMapEvents,    "log.mapevents")
SEGS_LOGGING_CATEGORY(logMapXfers,     "log.mapxfers")
SEGS_LOGGING_CATEGORY(logSlashCommand, "log.slashcommand")
SEGS_LOGGING_CATEGORY(logDescription,  "log.description")
SEGS_LOGGING_CATEGORY(logFriends,      "log.friends")
SEGS_LOGGING_CATEGORY(logMiniMap,      "log.minimap")
SEGS_LOGGING_CATEGORY(logLFG,          "log.lfg")
SEGS_LOGGING_CATEGORY(logNPCs,         "log.npcs")
SEGS_LOGGING_CATEGORY(logAnimations,   "log.animations")
SEGS_LOGGING_CATEGORY(logPowers,       "log.powers")
SEGS_LOGGING_CATEGORY(logTrades,       "log.trades")
SEGS_LOGGING_CATEGORY(logTailor,       "log.tailor")
SEGS_LOGGING_CATEGORY(logScripts,      "log.scripts")
SEGS_LOGGING_CATEGORY(logSceneGraph,   "log.scenegraph")
SEGS_LOGGING_CATEGORY(logStores,       "log.stores")
SEGS_LOGGING_CATEGORY(logTasks,        "log.tasks")
SEGS_LOGGING_CATEGORY(logRPC,          "log.rpc")
SEGS_LOGGING_CATEGORY(logAFK,          "log.afk")
SEGS_LOGGING_CATEGORY(logConnection,   "log.connection")
SEGS_LOGGING_CATEGORY(logMigration,    "log.migration")

void LoggingCategory::setFilterRules(StringView rules)
{
    Vector<StringView> rule_vector;
    String::split_ref(rule_vector,rules,'\n');
    for(StringView rule : rule_vector)
    {
        auto eq_pos = rule.find_first_of('=');
        if(eq_pos==StringView::npos)
            continue;
        StringView matcher = rule.substr(0,eq_pos);
        bool value = rule.substr(eq_pos+1)=="true";
        // matcher can contain optional suffixes like .debug, .info, .warning, .critical
        // we detect those and set the appropriate flags
        bool is_debug = matcher.ends_with(".debug");
        bool is_info = matcher.ends_with(".info");
        bool is_warning = matcher.ends_with(".warning");
        bool is_critical = matcher.ends_with(".critical");
        if(is_debug || is_info || is_warning || is_critical)
            matcher = matcher.substr(0,matcher.find_last_of('.'));
        // find matching categories that this rule applies to.
        for(LoggingCategory *cat : m_registered_categories)
        {
            if(StringUtils::match(matcher,cat->name()))
            {
                if(is_debug)
                    cat->switches.m_debug = value;
                else if(is_info)
                    cat->switches.m_info = value;
                else if(is_warning)
                    cat->switches.m_warning = value;
                else if(is_critical)
                    cat->switches.m_critical = value;
                else
                    cat->m_enabled = value;
            }

        }
    }
}


void setLoggingFilter()
{
    Settings config(Settings::getSettingsPath());

    config.beginGroup("Logging");
    String filter_rules = config.value<String>("log_generic","*.debug=true\n");
    filter_rules += "\nlog.logging="        + config.value<String>("log_logging","false");
    filter_rules += "\nlog.keybinds="       + config.value<String>("log_keybinds","false");
    filter_rules += "\nlog.settings="       + config.value<String>("log_settings","false");
    filter_rules += "\nlog.gui="            + config.value<String>("log_gui","false");
    filter_rules += "\nlog.teams="          + config.value<String>("log_teams","false");
    filter_rules += "\nlog.db="             + config.value<String>("log_db","false");
    filter_rules += "\nlog.input="          + config.value<String>("log_input","false");
    filter_rules += "\nlog.position="       + config.value<String>("log_position","false");
    filter_rules += "\nlog.orientation="    + config.value<String>("log_orientation","false");
    filter_rules += "\nlog.movement="       + config.value<String>("log_movement","false");
    filter_rules += "\nlog.chat="           + config.value<String>("log_chat","false");
    filter_rules += "\nlog.infomsg="        + config.value<String>("log_infomsg","false");
    filter_rules += "\nlog.emotes="         + config.value<String>("log_emotes","true");
    filter_rules += "\nlog.target="         + config.value<String>("log_target","false");
    filter_rules += "\nlog.charsel="        + config.value<String>("log_charsel","false");
    filter_rules += "\nlog.playerspawn="    + config.value<String>("log_playerspawn","false");
    filter_rules += "\nlog.npcspawn="       + config.value<String>("log_npcspawn","false");
    filter_rules += "\nlog.mapevents="      + config.value<String>("log_mapevents","true");
    filter_rules += "\nlog.mapxfers="       + config.value<String>("log_mapxfers", "false");
    filter_rules += "\nlog.slashcommand="   + config.value<String>("log_slashcommand","true");
    filter_rules += "\nlog.description="    + config.value<String>("log_description","false");
    filter_rules += "\nlog.friends="        + config.value<String>("log_friends","false");
    filter_rules += "\nlog.minimap="        + config.value<String>("log_minimap","false");
    filter_rules += "\nlog.lfg="            + config.value<String>("log_lfg","false");
    filter_rules += "\nlog.npcs="           + config.value<String>("log_npcs","false");
    filter_rules += "\nlog.animations="     + config.value<String>("log_animations","false");
    filter_rules += "\nlog.powers="         + config.value<String>("log_powers","false");
    filter_rules += "\nlog.trades="         + config.value<String>("log_trades","false");
    filter_rules += "\nlog.tailor="         + config.value<String>("log_tailor","false");
    filter_rules += "\nlog.scripts="        + config.value<String>("log_scripts","false");
    filter_rules += "\nlog.scenegraph="     + config.value<String>("log_scenegraph","false");
    filter_rules += "\nlog.stores="         + config.value<String>("log_stores","false");
    filter_rules += "\nlog.tasks="          + config.value<String>("log_tasks","false");
    filter_rules += "\nlog.rpc="            + config.value<String>("log_rpc","false");
    filter_rules += "\nlog.afk="            + config.value<String>("log_afk","false");
    filter_rules += "\nlog.connection="     + config.value<String>("log_connection","false");
    filter_rules += "\nlog.migration="      + config.value<String>("log_migration","false");
    config.endGroup(); // Logging

    LoggingCategory::setFilterRules(filter_rules);

    sCDebug(logLogging) << "Logging FilterRules:" << filter_rules; // so meta
}

void toggleLogging(StringView category)
{
    if(category.empty())
        return;

    LoggingCategory *cat = nullptr;

    if(category.contains("logging",false))
        cat = &logLogging();
    else if(category.contains("keybinds",false))
        cat = &logKeybinds();
    else if(category.contains("settings",false))
        cat = &logSettings();
    else if(category.contains("gui",false))
        cat = &logGUI();
    else if(category.contains("teams",false))
        cat = &logTeams();
    else if(category.contains("db",false))
        cat = &logDB();
    else if(category.contains("charsel",false))
        cat = &logCharSel();
    else if(category.contains("input",false))
        cat = &logInput();
    else if(category.contains("position",false))
        cat = &logPosition();
    else if(category.contains("orientation",false))
        cat = &logOrientation();
    else if(category.contains("movement",false))
        cat = &logMovement();
    else if(category.contains("chat",false))
        cat = &logChat();
    else if(category.contains("infomsg",false))
        cat = &logInfoMsg();
    else if(category.contains("emotes",false))
        cat = &logEmotes();
    else if(category.contains("target",false))
        cat = &logTarget();
    else if(category.contains("playerspawn",false))
        cat = &logPlayerSpawn();
    else if(category.contains("npcspawn",false))
        cat = &logNpcSpawn();
    else if(category.contains("mapevents",false))
        cat = &logMapEvents();
    else if(category.contains("mapxfers",false))
        cat = &logMapXfers();
    else if(category.contains("slashcommand",false))
        cat = &logSlashCommand();
    else if(category.contains("description",false))
        cat = &logDescription();
    else if(category.contains("friends",false))
        cat = &logFriends();
    else if(category.contains("minimap",false))
        cat = &logMiniMap();
    else if(category.contains("lfg",false))
        cat = &logLFG();
    else if(category.contains("npcs",false))
        cat = &logNPCs();
    else if(category.contains("animations",false))
        cat = &logAnimations();
    else if(category.contains("powers",false))
        cat = &logPowers();
    else if(category.contains("trades",false))
        cat = &logTrades();
    else if(category.contains("tailor",false))
        cat = &logTailor();
    else if(category.contains("scripts", false))
        cat = &logScripts();
    else if(category.contains("scenegraph",false))
        cat = &logSceneGraph();
    else if(category.contains("stores", false))
        cat = &logStores();
    else if(category.contains("tasks",false))
        cat = &logTasks();
    else if(category.contains("rpc",false))
        cat = &logRPC();
    else if(category.contains("afk",false))
        cat = &logAFK();
    else if(category.contains("connection",false))
        cat = &logConnection();
    else if(category.contains("migration",false))
        cat = &logMigration();
    else
        return;

    cat->toggleLogging();
    dumpLogging();
}

void dumpLogging()
{
    String output = "Current Logging Categories:";
    output += "\n\t logging: "      + eastl::to_string(logLogging().isDebugEnabled());
    output += "\n\t keybinds: "     + eastl::to_string(logKeybinds().isDebugEnabled());
    output += "\n\t settings: "     + eastl::to_string(logSettings().isDebugEnabled());
    output += "\n\t gui: "          + eastl::to_string(logGUI().isDebugEnabled());
    output += "\n\t teams: "        + eastl::to_string(logTeams().isDebugEnabled());
    output += "\n\t db: "           + eastl::to_string(logDB().isDebugEnabled());
    output += "\n\t input: "        + eastl::to_string(logInput().isDebugEnabled());
    output += "\n\t position: "     + eastl::to_string(logPosition().isDebugEnabled());
    output += "\n\t orientation: "  + eastl::to_string(logOrientation().isDebugEnabled());
    output += "\n\t movement: "     + eastl::to_string(logMovement().isDebugEnabled());
    output += "\n\t chat: "         + eastl::to_string(logChat().isDebugEnabled());
    output += "\n\t infomsg: "      + eastl::to_string(logInfoMsg().isDebugEnabled());
    output += "\n\t emotes: "       + eastl::to_string(logEmotes().isDebugEnabled());
    output += "\n\t target: "       + eastl::to_string(logTarget().isDebugEnabled());
    output += "\n\t charsel: "      + eastl::to_string(logCharSel().isDebugEnabled());
    output += "\n\t playerspawn: "  + eastl::to_string(logPlayerSpawn().isDebugEnabled());
    output += "\n\t npcspawn: "     + eastl::to_string(logNpcSpawn().isDebugEnabled());
    output += "\n\t mapevents: "    + eastl::to_string(logMapEvents().isDebugEnabled());
    output += "\n\t mapxfers: "     + eastl::to_string(logMapXfers().isDebugEnabled());
    output += "\n\t slashcommand: " + eastl::to_string(logSlashCommand().isDebugEnabled());
    output += "\n\t description: "  + eastl::to_string(logDescription().isDebugEnabled());
    output += "\n\t friends: "      + eastl::to_string(logFriends().isDebugEnabled());
    output += "\n\t minimap: "      + eastl::to_string(logMiniMap().isDebugEnabled());
    output += "\n\t lfg: "          + eastl::to_string(logLFG().isDebugEnabled());
    output += "\n\t npcs: "         + eastl::to_string(logNPCs().isDebugEnabled());
    output += "\n\t animations: "   + eastl::to_string(logAnimations().isDebugEnabled());
    output += "\n\t powers: "       + eastl::to_string(logPowers().isDebugEnabled());
    output += "\n\t trades: "       + eastl::to_string(logTrades().isDebugEnabled());
    output += "\n\t tailor: "       + eastl::to_string(logTailor().isDebugEnabled());
    output += "\n\t scripts: "      + eastl::to_string(logScripts().isDebugEnabled());
    output += "\n\t scenegraph: "   + eastl::to_string(logSceneGraph().isDebugEnabled());
    output += "\n\t stores: "       + eastl::to_string(logStores().isDebugEnabled());
    output += "\n\t tasks: "        + eastl::to_string(logTasks().isDebugEnabled());
    output += "\n\t rpc: "          + eastl::to_string(logRPC().isDebugEnabled());
    output += "\n\t afk: "          + eastl::to_string(logAFK().isDebugEnabled());
    output += "\n\t connection: "   + eastl::to_string(logConnection().isDebugEnabled());
    output += "\n\t migration: "    + eastl::to_string(logMigration().isDebugEnabled());

    sDebug() << output;
}

//! @}

DebugOutput LogChannels::debug(const char *file, int line, const char *func, const char *category) {
    return DebugOutput(file,line,func,SegsLogLevel::Debug, category);
}

DebugOutput LogChannels::info(const char *file, int line, const char *func, const char *category) {
    return DebugOutput(file,line,func,SegsLogLevel::Info, category);

}

DebugOutput LogChannels::warning(const char *file, int line, const char *func, const char *category) {
    return DebugOutput(file,line,func,SegsLogLevel::Warning, category);

}

DebugOutput LogChannels::critical(const char *file, int line, const char *func, const char *category) {
    return DebugOutput(file,line,func,SegsLogLevel::Critical, category);
}
