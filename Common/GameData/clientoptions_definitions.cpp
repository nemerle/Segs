#include "clientoptions_definitions.h"

#define CLIENT_OPT(type,var)\
    ClientOption {#var,{{type,&var}} }

void ClientOptions::init()
{
    m_opts = {
        CLIENT_OPT(ClientOption::t_int,control_debug),
        CLIENT_OPT(ClientOption::t_int,no_strafe),
        CLIENT_OPT(ClientOption::t_int,alwaysmobile),
        CLIENT_OPT(ClientOption::t_int,repredict),
        CLIENT_OPT(ClientOption::t_int,neterrorcorrection),
        CLIENT_OPT(ClientOption::t_float,speed_scale),
        CLIENT_OPT(ClientOption::t_int,svr_lag),
        CLIENT_OPT(ClientOption::t_int,svr_lag_vary),
        CLIENT_OPT(ClientOption::t_int,svr_pl),
        CLIENT_OPT(ClientOption::t_int,svr_oo_packets),
        CLIENT_OPT(ClientOption::t_int,client_pos_id),
        CLIENT_OPT(ClientOption::t_int,atest0),
        CLIENT_OPT(ClientOption::t_int,atest1),
        CLIENT_OPT(ClientOption::t_int,atest2),
        CLIENT_OPT(ClientOption::t_int,atest3),
        CLIENT_OPT(ClientOption::t_int,atest4),
        CLIENT_OPT(ClientOption::t_int,atest5),
        CLIENT_OPT(ClientOption::t_int,atest6),
        CLIENT_OPT(ClientOption::t_int,atest7),
        CLIENT_OPT(ClientOption::t_int,atest8),
        CLIENT_OPT(ClientOption::t_int,atest9),
        CLIENT_OPT(ClientOption::t_int,predict),
        CLIENT_OPT(ClientOption::t_int,notimeout),
        CLIENT_OPT(ClientOption::t_int,selected_ent_server_index),
    };
}

void ClientOptions::clientOptionsDump() const {
    sDebug() << "Debugging ClientOptions:"
            << "\n\t" << "Invert Mouse:" << m_mouse_invert
            << "\n\t" << "Mouse Speed:" << m_mouse_speed
            << "\n\t" << "Turn Speed:" << m_turn_speed
            << "\n\t" << "Fade Chat Window:" << m_fade_chat_wnd
            << "\n\t" << "Fade Nav Window:" << m_fade_nav_wnd
            << "\n\t" << "Show Tooltips:" << m_show_tooltips
            << "\n\t" << "Allow Profanity:" << m_allow_profanity
            << "\n\t" << "Chat Balloons:" << m_chat_balloons
            << "\n\t" << "Show Archetype:" << m_show_archetype
            << "\n\t" << "Show SuperGroup:" << m_show_supergroup
            << "\n\t" << "Show Player Name:" << m_show_player_name
            << "\n\t" << "Show Player Bars:" << m_show_player_bars
            << "\n\t" << "Show Enemy Name:" << m_show_enemy_name
            << "\n\t" << "Show Enemy Bars:" << m_show_enemy_bars
            << "\n\t" << "Show Player Reticles:" << m_show_player_reticles
            << "\n\t" << "Show Enemy Reticles:" << m_show_enemy_reticles
            << "\n\t" << "Show Assist Reticles:" << m_show_assist_reticles
            << "\n\t" << "Chat Font Size:" << m_chat_font_size;
}
#undef ADD_OPT
