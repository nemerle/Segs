/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/Containers/StringView.h"
#include <stdint.h>
#include <cassert>

#include <cereal/cereal.hpp>
#include <Components/Logging.h>

enum ReticleVisibility : uint32_t {
  rv_HideAlways             = 0,
  rv_Always                 = 1,
  rv_OnMouseOver            = 2,
  rv_Selected               = 4,
  rv_SelectedOrMouseOver    = 6,
};

struct ClientOption
{
    enum eType
    {
        t_int           = 1,
        t_string        = 2,
        t_float         = 3,
        t_sentence      = 4,
        t_quant_angle   = 5,
        t_mat4          = 6,
        t_vec3          = 7,
        t_date          = 9,
        t_unknown
    };
    struct Arg
    {
        Arg(int t,void *v) : type(eType(t)),tgt(v){}
        eType type;
        void *tgt;
    };
    String name;
    Vector<Arg> m_args;
    //ClientOption(const char *v) : name(v) {}
};

class ClientOptions
{
    Vector<ClientOption> m_opts;
    void init();
public:
    ClientOptions()
    {
        init();
        m_mouse_speed = 0.6f;
    }

    // ClientOptions serialization class version
    enum : uint32_t { class_version = 1 };

    // Other Options
    int32_t control_debug       = 0;
    int32_t no_strafe           = 0;
    int32_t alwaysmobile        = 0; // 1 - player is always mobile (can't be immobilized by powers)
    int32_t repredict           = 0; // 1 - client is out of sync with server, asking for new physics state info.
    int32_t neterrorcorrection  = 0;
    float   speed_scale         = 0;

    int32_t svr_lag,svr_lag_vary,svr_pl,svr_oo_packets,client_pos_id;
    int32_t atest0,atest1,atest2,atest3,atest4,atest5,atest6,atest7,atest8,atest9;
    int32_t predict,notimeout,selected_ent_server_index;

    // Options
    bool    m_first_person_view = false;

    // Options Window Settings
    bool                m_mouse_invert         = false;
    float               m_mouse_speed          = 0;
    float               m_turn_speed           = 3;    // client displays this as pct (*100)
    bool                m_fade_chat_wnd        = true;
    bool                m_fade_nav_wnd         = true;
    bool                m_show_tooltips        = true;
    bool                m_allow_profanity      = false;
    bool                m_chat_balloons        = true;
    ReticleVisibility   m_show_archetype       = rv_OnMouseOver;
    ReticleVisibility   m_show_supergroup      = rv_OnMouseOver;
    ReticleVisibility   m_show_player_name     = rv_Always;
    ReticleVisibility   m_show_player_bars     = rv_SelectedOrMouseOver;
    ReticleVisibility   m_show_enemy_name      = rv_SelectedOrMouseOver;
    ReticleVisibility   m_show_enemy_bars      = rv_SelectedOrMouseOver;
    ReticleVisibility   m_show_player_reticles = rv_SelectedOrMouseOver;
    ReticleVisibility   m_show_enemy_reticles  = rv_SelectedOrMouseOver;
    ReticleVisibility   m_show_assist_reticles = rv_SelectedOrMouseOver;
    uint8_t             m_chat_font_size       = 12;   // 12 default size

    ClientOption *get(int idx)
    {
        if(idx<0)
            return nullptr;
        assert((size_t(idx)<m_opts.size()) && "Unknown option requested!!");
        return &m_opts[idx];
    }

    void clientOptionsDump() const;

    template<class Archive>
    void serialize(Archive &archive, uint32_t const version)
    {
        if(version != ClientOptions::class_version)
        {
            sCritical() << "Failed to serialize ClientOptions, incompatible serialization format version " << version;
            return;
        }

        archive(cereal::make_nvp("FirstPersonView", m_first_person_view));
        archive(cereal::make_nvp("MouseSpeed", m_mouse_speed));
        archive(cereal::make_nvp("TurnSpeed", m_turn_speed));
        archive(cereal::make_nvp("InvertMouse", m_mouse_invert));
        archive(cereal::make_nvp("FadeChatWindow", m_fade_chat_wnd));
        archive(cereal::make_nvp("FadeNavWindow", m_fade_nav_wnd));
        archive(cereal::make_nvp("ShowTooltips", m_show_tooltips));
        archive(cereal::make_nvp("AllowProfanity", m_allow_profanity));
        archive(cereal::make_nvp("ChatBalloons", m_chat_balloons));
        archive(cereal::make_nvp("ShowArchetype", uint32_t(m_show_archetype)));
        archive(cereal::make_nvp("ShowSuperGroup", uint32_t(m_show_supergroup)));
        archive(cereal::make_nvp("PlayerName", uint32_t(m_show_player_name)));
        archive(cereal::make_nvp("PlayerBars", uint32_t(m_show_player_bars)));
        archive(cereal::make_nvp("EnemyName", uint32_t(m_show_enemy_name)));
        archive(cereal::make_nvp("EnemyBars", uint32_t(m_show_enemy_bars)));
        archive(cereal::make_nvp("PlayerReticles", uint32_t(m_show_player_reticles)));
        archive(cereal::make_nvp("EnemyReticles", uint32_t(m_show_enemy_reticles)));
        archive(cereal::make_nvp("AssistReticles", uint32_t(m_show_assist_reticles)));
        archive(cereal::make_nvp("FontSize", uint32_t(m_chat_font_size)));
    }
};
CEREAL_CLASS_VERSION(ClientOptions, ClientOptions::class_version)
