#include "../plugin_sdk/plugin_sdk.hpp"
#include "jax.h"

namespace jax
{
    // Define the colors that will be used in on_draw()
    //
#define Q_DRAW_COLOR (MAKE_COLOR ( 62, 129, 237, 255 ))  //Red Green Blue Alpha
#define E_DRAW_COLOR (MAKE_COLOR ( 235, 12, 223, 255 ))  //Red Green Blue Alpha

// To declare a spell, it is necessary to create an object and registering it in load function
    script_spell* q = nullptr;
    script_spell* w = nullptr;
    script_spell* e = nullptr;
    script_spell* r = nullptr;

    // Declaration of menu objects
    TreeTab* main_tab = nullptr;

    namespace draw_settings
    {
        TreeEntry* draw_range_q = nullptr;
        TreeEntry* draw_range_e = nullptr;
    }

    namespace combo
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* q_only_when_e_ready = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_r = nullptr;
        TreeEntry* r_myhero_hp_under = nullptr;
        TreeEntry* r_only_when_enemies_nearby = nullptr;
    }

    namespace harass
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_e = nullptr;
    }

    namespace laneclear
    {
        TreeEntry* spell_farm = nullptr;
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_e = nullptr;
    }

    namespace jungleclear
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_e = nullptr;
    }

    namespace fleemode
    {
        TreeEntry* use_q;
    }


    // Event handler functions
    void on_update();
    void on_draw();
    void on_before_attack(game_object_script sender, bool* process);

    // Declaring functions responsible for spell-logic
    //
    void q_logic();
    void w_logic();
    void e_logic();
    void r_logic();

    void load()
    {
        // Registering a spells
        //
        q = plugin_sdk->register_spell(spellslot::q, 700);
        w = plugin_sdk->register_spell(spellslot::w, 0);
        e = plugin_sdk->register_spell(spellslot::e, 300);
        r = plugin_sdk->register_spell(spellslot::r, 0);


        // Create a menu according to the description in the "Menu Section"
        //
        main_tab = menu->create_tab("jax", "Jax");
        main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
        {
            auto combo = main_tab->add_tab(myhero->get_model() + ".combo", "Combo Settings");
            {
                combo::use_q = combo->add_checkbox(myhero->get_model() + ".comboUseQ", "Use Q", true);
                auto q_config = combo->add_tab(myhero->get_model() + ".comboQConfig", "Q Config");
                {
                    combo::q_only_when_e_ready = q_config->add_checkbox(myhero->get_model() + ".comboQOnlyWhenEReady", "Use Q only when E is ready", false);
                }
                combo::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                combo::use_w = combo->add_checkbox(myhero->get_model() + ".comboUseW", "Use W", true);
                combo::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                combo::use_e = combo->add_checkbox(myhero->get_model() + ".comboUseE", "Use E", true);
                combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                combo::use_r = combo->add_checkbox(myhero->get_model() + ".comboUseR", "Use R", true);
                combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                auto r_config = combo->add_tab(myhero->get_model() + ".comboRConfig", "R Config");
                {
                    combo::r_myhero_hp_under = r_config->add_slider(myhero->get_model() + ".comboRMyheroHpUnder", "Myhero HP is under (in %)", 50, 0, 100);
                    combo::r_only_when_enemies_nearby = r_config->add_checkbox(myhero->get_model() + ".comboROnlyWhenEnemiesNearby", "Only when enemies are nearby", true);
                }
            }

            auto harass = main_tab->add_tab(myhero->get_model() + ".harass", "Harass Settings");
            {
                harass::use_q = harass->add_checkbox(myhero->get_model() + ".harassUseQ", "Use Q", false);
                harass::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                harass::use_w = harass->add_checkbox(myhero->get_model() + ".harassUseW", "Use W", true);
                harass::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                harass::use_e = harass->add_checkbox(myhero->get_model() + ".harassUseE", "Use E", false);
                harass::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
            }

            auto laneclear = main_tab->add_tab(myhero->get_model() + ".laneclear", "Lane Clear Settings");
            {
                laneclear::spell_farm = laneclear->add_hotkey(myhero->get_model() + ".laneclearToggleSpellFarm", "Toggle Spell Farm", TreeHotkeyMode::Toggle, 'H', true);
                laneclear::use_q = laneclear->add_checkbox(myhero->get_model() + ".laneclearUseQ", "Use Q", false);
                laneclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                laneclear::use_w = laneclear->add_checkbox(myhero->get_model() + ".laneclearUseW", "Use W", true);
                laneclear::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                laneclear::use_e = laneclear->add_checkbox(myhero->get_model() + ".laneclearUseE", "Use E", false);
                laneclear::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
            }

            auto jungleclear = main_tab->add_tab(myhero->get_model() + ".jungleclear", "Jungle Clear Settings");
            {
                jungleclear::use_q = jungleclear->add_checkbox(myhero->get_model() + ".jungleclearUseQ", "Use Q", true);
                jungleclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                jungleclear::use_w = jungleclear->add_checkbox(myhero->get_model() + ".jungleclearUseW", "Use W", true);
                jungleclear::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                jungleclear::use_e = jungleclear->add_checkbox(myhero->get_model() + ".jungleclearUseE", "Use E", false);
                jungleclear::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
            }


            auto fleemode = main_tab->add_tab(myhero->get_model() + ".fleemode", "Flee Mode");
            {
                fleemode::use_q = fleemode->add_checkbox(myhero->get_model() + ".fleemodeUseQ", "Use Q to jump on ally", true);
                fleemode::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
            }

            auto draw_settings = main_tab->add_tab(myhero->get_model() + ".drawings", "Drawings Settings");
            {
                draw_settings::draw_range_q = draw_settings->add_checkbox(myhero->get_model() + ".drawingW", "Draw Q range", true);
                draw_settings::draw_range_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                draw_settings::draw_range_e = draw_settings->add_checkbox(myhero->get_model() + ".drawingE", "Draw E range", true);
                draw_settings::draw_range_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
            }
        }

        // To add a new event you need to define a function and call add_calback
        //
        event_handler<events::on_update>::add_callback(on_update);
        event_handler<events::on_draw>::add_callback(on_draw);
        event_handler<events::on_before_attack_orbwalker>::add_callback(on_before_attack);
    }

    void unload()
    {
        // Always remove all declared spells
        //
        plugin_sdk->remove_spell(q);
        plugin_sdk->remove_spell(w);
        plugin_sdk->remove_spell(e);
        plugin_sdk->remove_spell(r);

        // Remove menu tab
        //
        menu->delete_tab("jax");

        // VERY important to remove always ALL events
        //
        event_handler<events::on_update>::remove_handler(on_update);
        event_handler<events::on_draw>::remove_handler(on_draw);
        event_handler<events::on_before_attack_orbwalker>::remove_handler(on_before_attack);
    }

    // Main update script function
    void on_update()
    {
        if (myhero->is_dead())
        {
            return;
        }

        if (r->is_ready() && combo::use_r->get_bool())
        {
            r_logic();
        }

        // Very important if can_move ( extra_windup ) 
        // Extra windup is the additional time you have to wait after the aa
        // Too small time can interrupt the attack
        if (orbwalker->can_move(0.05f))
        {
            //Checking if the user has combo_mode() (Default SPACE)
            if (orbwalker->combo_mode())
            {
                if (q->is_ready() && combo::use_q->get_bool())
                {
                    q_logic();
                }

                if (w->is_ready() && combo::use_w->get_bool())
                {
                    w_logic();
                }

                if (e->is_ready() && combo::use_e->get_bool())
                {
                    e_logic();
                }
            }

            //Checking if the user has selected harass() (Default C)
            if (orbwalker->harass())
            {
                // Get a target from a given range
                auto target = target_selector->get_target(e->range(), damage_type::physical);

                // Always check an object is not a nullptr!
                if (target != nullptr)
                {
                    if (!myhero->is_under_enemy_turret())
                    {
                        if (q->is_ready() && harass::use_q->get_bool())
                        {
                            q_logic();
                        }

                        if (w->is_ready() && harass::use_w->get_bool())
                        {
                            w_logic();
                        }

                        if (e->is_ready() && harass::use_e->get_bool())
                        {
                            e_logic();
                        }
                    }
                }
            }

            // Checking if the user has selected flee_mode() (Default Z)
            if (orbwalker->flee_mode())
            {
                if (q->is_ready() && fleemode::use_q->get_bool())
                {
                    auto lane_minions = entitylist->get_ally_minions();

                    //std::sort -> sort lane minions by distance
                    std::sort(lane_minions.begin(), lane_minions.end(), [](game_object_script a, game_object_script b)
                        {
                            return a->get_distance(hud->get_hud_input_logic()->get_game_cursor_position()) < b->get_distance(hud->get_hud_input_logic()->get_game_cursor_position());
                        });

                    // You can use this function to delete minions that aren't in the specified range
                    lane_minions.erase(std::remove_if(lane_minions.begin(), lane_minions.end(), [](game_object_script x)
                        {
                            return !x->is_valid_target(q->range());
                        }), lane_minions.end());

                    if (!lane_minions.empty())
                    {
                        if (myhero->get_distance(lane_minions.front()) < q->range())
                        {
                            if (q->cast(lane_minions.front()))
                            {
                                return;
                            }
                        }
                    }
                }
            }

            // Checking if the user has selected lane_clear_mode() (Default V)
            if (orbwalker->lane_clear_mode() && laneclear::spell_farm->get_bool())
            {
                // Gets enemy minions from the entitylist
                auto lane_minions = entitylist->get_enemy_minions();

                // Gets jugnle mobs from the entitylist
                auto monsters = entitylist->get_jugnle_mobs_minions();

                // You can use this function to delete minions that aren't in the specified range
                lane_minions.erase(std::remove_if(lane_minions.begin(), lane_minions.end(), [](game_object_script x)
                    {
                        return !x->is_valid_target(q->range());
                    }), lane_minions.end());

                // You can use this function to delete monsters that aren't in the specified range
                monsters.erase(std::remove_if(monsters.begin(), monsters.end(), [](game_object_script x)
                    {
                        return !x->is_valid_target(q->range());
                    }), monsters.end());

                //std::sort -> sort lane minions by distance
                std::sort(lane_minions.begin(), lane_minions.end(), [](game_object_script a, game_object_script b)
                    {
                        return a->get_position().distance(myhero->get_position()) < b->get_position().distance(myhero->get_position());
                    });

                //std::sort -> sort monsters by max health
                std::sort(monsters.begin(), monsters.end(), [](game_object_script a, game_object_script b)
                    {
                        return a->get_max_health() > b->get_max_health();
                    });

                if (!lane_minions.empty())
                {
                    if (q->is_ready() && laneclear::use_q->get_bool())
                    {
                        if (myhero->is_under_enemy_turret())
                        {
                            if (myhero->count_enemies_in_range(q->range()) == 0)
                            {
                                if (q->cast(lane_minions.front()))
                                {
                                    return;
                                }
                            }
                        }
                        if (q->cast(lane_minions.front()))
                            return;
                    }

                    if (w->is_ready() && laneclear::use_w->get_bool())
                    {
                        if (myhero->is_under_enemy_turret())
                        {
                            if (myhero->count_enemies_in_range(myhero->get_attack_range() + 50) == 0)
                            {
                                if (w->cast())
                                {
                                    return;
                                }
                            }
                        }
                        if (w->cast())
                            return;
                    }

                    if (e->is_ready() && laneclear::use_e->get_bool())
                    {
                        if (myhero->is_under_enemy_turret())
                        {
                            if (myhero->count_enemies_in_range(e->range()) == 0)
                            {
                                if (e->cast())
                                {
                                    return;
                                }
                            }
                        }
                        if (e->cast())
                            return;
                    }
                }


                if (!monsters.empty())
                {
                    // Logic responsible for monsters
                    if (q->is_ready() && jungleclear::use_q->get_bool())
                    {
                        if (q->cast(monsters.front()))
                            return;
                    }

                    if (w->is_ready() && jungleclear::use_w->get_bool())
                    {
                        if (w->cast())
                            return;
                    }

                    if (e->is_ready() && jungleclear::use_e->get_bool())
                    {
                        if (e->cast())
                            return;
                    }
                }
            }
        }
    }

#pragma region q_logic
    void q_logic()
    {
        // Get a target from a given range
        auto target = target_selector->get_target(q->range(), damage_type::physical);

        // Always check an object is not a nullptr!
        if (target != nullptr)
        {
            if (combo::q_only_when_e_ready->get_bool())
            {
                if (e->is_ready())
                {
                    e->cast();
                    q->cast(target);
                }
                return;
            }

            if (q->cast(target))
                return;
        }
    }
#pragma endregion

#pragma region w_logic
    void w_logic()
    {
        // Get a target from a given range
        auto target = target_selector->get_target(myhero->get_attack_range() + 50, damage_type::magical);

        // Always check an object is not a nullptr!
        if (target != nullptr)
        {
            w->cast();
        }
    }
#pragma endregion

#pragma region e_logic
    void e_logic()
    {
        // Get a target from a given range
        auto target = target_selector->get_target(e->range(), damage_type::physical);

        // Always check an object is not a nullptr!
        if (target != nullptr)
        {
            e->cast();
        }
    }
#pragma endregion

#pragma region r_logic
    void r_logic()
    {
        if (r->is_ready() && combo::use_r->get_bool())
        {
            if (combo::r_only_when_enemies_nearby->get_bool() && myhero->count_enemies_in_range(1000) == 0)
            {
                return;
            }

            if (!myhero->has_buff(buff_hash("JaxR")) && myhero->get_health_percent() < combo::r_myhero_hp_under->get_int())
            {
                if (r->cast())
                {
                    return;
                }
            }
        }
    }
#pragma endregion

    void on_before_attack(game_object_script sender, bool* process)
    {
    }

    void on_draw()
    {

        if (myhero->is_dead())
        {
            return;
        }

        // Draw Q range
        if (q->is_ready() && draw_settings::draw_range_q->get_bool())
            draw_manager->add_circle(myhero->get_position(), q->range(), Q_DRAW_COLOR);

        // Draw E range
        if (e->is_ready() && draw_settings::draw_range_e->get_bool())
            draw_manager->add_circle(myhero->get_position(), e->range(), E_DRAW_COLOR);

        auto pos = myhero->get_position();
        renderer->world_to_screen(pos, pos);
        auto spellfarm = laneclear::spell_farm->get_bool();
        draw_manager->add_text_on_screen(pos + vector(0, 40), (spellfarm ? 0xFF00FF00 : 0xFF0000FF), 14, "FARM %s", (spellfarm ? "ON" : "OFF"));
    }
};