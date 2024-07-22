//
//  NPC.h
//  DungGine
//
//  Created by Rasmus Anthin on 2024-07-19.
//

#pragma once
#include "Items.h"

namespace dung
{
  
  enum class Race { Human, Elf, Half_Elf, Gnome, Halfling, Dwarf, Half_Orc, Ogre, Hobgoblin, Goblin, Orc, Troll, Monster, Lich, Lich_King, Basilisk, Bear, Kobold, Skeleton, Giant, Huge_Spider, Wolf, Wyvern, Griffin, Ghoul, Dragon, NUM_ITEMS };
  enum class Class { Warrior_Fighter, Warrior_Ranger, Warrior_Paladin, Warrior_Barbarian, Priest_Cleric, Priest_Druid, Priest_Monk, Priest_Shaman, Wizard_Mage, Wizard_Sorcerer, Rogue_Thief, Rogue_Bard, NUM_ITEMS };
  
  struct NPC final
  {
    RC pos;
    float pos_r = 0.f;
    float pos_c = 0.f;
    float vel_r = 0.f;
    float vel_c = 0.f;
    float acc_r = 0.f;
    float acc_c = 0.f;
    const float px_aspect = 1.5f;
    float acc_step = 10.f;
    float acc_lim = 25.f;
    float vel_lim = 12.f;
    int prob_change_acc = 7;
    int prob_slow_fast = 20;
    const float acc_slowness_factor = 0.6f;
    const float vel_slowness_factor = 0.2f;
    
    bool wall_coll_resolve = false;
    int wall_coll_resolve_ctr = 0;
    
    Style style { Color::Green, Color::DarkYellow };
    char character = 'O';
    bool fog_of_war = true;
    bool light = false;
    bool is_underground = false;
    BSPNode* curr_room = nullptr;
    Corridor* curr_corridor = nullptr;
    bool last_in_room = false;
    bool last_in_corridor = false;
    
    bool enemy = true;
    
    int health = 100;
    int strength = 10;
    int dexterity = 10;
    int constitution = 10;
    int thac0 = 1;
    int armor_class = 2;
    
    Race npc_race = Race::Ogre;
    Class npc_class = Class::Warrior_Barbarian;
    int weapon_idx = -1;
    
    void init()
    {
      pos_r = pos.r;
      pos_c = pos.c;
      
      npc_race = rnd::rand_enum<Race>();
      npc_class = rnd::rand_enum<Class>();
      
      switch (npc_race)
      {
        case Race::Human:
          character = '@';
          style = { Color::Magenta, Color::LightGray };
          //acc_sigma = rnd::randn_range(30.f, 200.f);
          acc_step = rnd::randn_range(2.f, 20.f)/10.f;
          acc_lim = rnd::randn_range(20.f, 50.f);
          vel_lim = rnd::randn_range(4.f, 15.f);
          prob_change_acc = rnd::randn_range_int(4, 10);
          break;
        case Race::Elf:
          character = '@';
          style = { Color::Magenta, Color::DarkGreen };
          //acc_sigma = rnd::randn_range(50.f, 300.f);
          acc_step = rnd::randn_range(4.f, 40.f)/10.f;
          acc_lim = rnd::randn_range(25.f, 70.f);
          vel_lim = rnd::randn_range(6.f, 20.f);
          prob_change_acc = rnd::randn_range_int(4, 10);
          break;
        case Race::Half_Elf:
          character = '@';
          style = { Color::Magenta, Color::DarkYellow };
          //acc_sigma = rnd::randn_range(40.f, 250.f);
          acc_step = rnd::randn_range(3.f, 30.f)/10.f;
          acc_lim = rnd::randn_range(25.f, 60.f);
          vel_lim = rnd::randn_range(5.f, 17.f);
          prob_change_acc = rnd::randn_range_int(4, 10);
          break;
        case Race::Gnome:
          character = 'b';
          style = { Color::Magenta, Color::LightGray };
          //acc_sigma = rnd::randn_range(5.f, 60.f);
          acc_step = rnd::randn_range(1.f, 10.f)/10.f;
          acc_lim = rnd::randn_range(10.f, 20.f);
          vel_lim = rnd::randn_range(0.5f, 2.5f);
          prob_change_acc = rnd::randn_range_int(1, 4);
          break;
        case Race::Halfling:
          character = 'b';
          style = { Color::Magenta, Color::LightGray };
          //acc_sigma = rnd::randn_range(3.f, 80.f);
          acc_step = rnd::randn_range(1.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(11.f, 25.f);
          vel_lim = rnd::randn_range(0.7f, 3.f);
          prob_change_acc = rnd::randn_range_int(1, 5);
          break;
        case Race::Dwarf:
          character = '0';
          style = { Color::White, Color::DarkGray };
          //acc_sigma = rnd::randn_range(15.f, 150.f);
          acc_step = rnd::randn_range(1.5f, 18.f)/10.f;
          acc_lim = rnd::randn_range(12.f, 30.f);
          vel_lim = rnd::randn_range(0.4f, 4.f);
          prob_change_acc = rnd::randn_range_int(5, 20);
          break;
        case Race::Half_Orc:
          character = '3';
          style = { Color::Yellow, Color::Green };
          //acc_sigma = rnd::randn_range(20.f, 180.f);
          acc_step = rnd::randn_range(1.5f, 20.f)/10.f;
          acc_lim = rnd::randn_range(30.f, 80.f);
          vel_lim = rnd::randn_range(1.5f, 5.f);
          prob_change_acc = rnd::randn_range_int(2, 18);
          break;
        case Race::Ogre:
          character = 'O';
          style = { Color::Green, Color::DarkYellow };
          //acc_sigma = rnd::randn_range(20.f, 180.f);
          acc_step = rnd::randn_range(4.f, 10.f)/10.f;
          acc_lim = rnd::randn_range(2.f, 8.f);
          vel_lim = rnd::randn_range(1.f, 6.f);
          prob_change_acc = rnd::randn_range_int(4, 10);
          break;
        case Race::Hobgoblin:
          character = 'a';
          style = { Color::Yellow, Color::Cyan };
          //acc_sigma = rnd::randn_range(20.f, 180.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(10.f, 50.f);
          vel_lim = rnd::randn_range(4.f, 9.f);
          prob_change_acc = rnd::randn_range_int(4, 14);
          break;
        case Race::Goblin:
          character = 'G';
          style = { Color::Green, Color::DarkCyan };
          //acc_sigma = rnd::randn_range(20.f, 180.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(8.f, 45.f);
          vel_lim = rnd::randn_range(4.5f, 10.f);
          prob_change_acc = rnd::randn_range_int(3, 12);
          break;
        case Race::Orc:
          character = '2';
          style = { Color::DarkYellow, Color::Cyan };
          //acc_sigma = rnd::randn_range(60.f, 250.f);
          acc_step = rnd::randn_range(5.f, 25.f)/10.f;
          acc_lim = rnd::randn_range(50.f, 80.f);
          vel_lim = rnd::randn_range(6.f, 18.f);
          prob_change_acc = rnd::randn_range_int(4, 8);
          break;
        case Race::Troll:
          character = 'R';
          style = { Color::LightGray, Color::DarkRed };
          //acc_sigma = rnd::randn_range(1.f, 60.f);
          acc_step = rnd::randn_range(1.f, 14.f)/10.f;
          acc_lim = rnd::randn_range(5.f, 15.f);
          vel_lim = rnd::randn_range(2.f, 12.f);
          prob_change_acc = rnd::randn_range_int(10, 40);
          break;
        case Race::Monster:
          character = 'M';
          style = { Color::Cyan, Color::DarkGreen };
          //acc_sigma = rnd::randn_range(20.f, 180.f);
          acc_step = rnd::randn_range(0.5f, 25.f)/10.f;
          acc_lim = rnd::randn_range(2.f, 25.f);
          vel_lim = rnd::randn_range(1.f, 8.f);
          prob_change_acc = rnd::randn_range_int(8, 25);
          break;
        case Race::Lich:
          character = 'z';
          style = { Color::DarkYellow, Color::DarkBlue };
          //acc_sigma = rnd::randn_range(20.f, 180.f);
          acc_step = rnd::randn_range(4.f, 30.f)/10.f;
          acc_lim = rnd::randn_range(25.f, 55.f);
          vel_lim = rnd::randn_range(2.f, 9.f);
          prob_change_acc = rnd::randn_range_int(5, 8);
          break;
        case Race::Lich_King:
          character = 'Z';
          style = { Color::Yellow, Color::DarkBlue };
          //acc_sigma = rnd::randn_range(30.f, 200.f);
          acc_step = rnd::randn_range(5.f, 35.f)/10.f;
          acc_lim = rnd::randn_range(25.f, 60.f);
          vel_lim = rnd::randn_range(2.5f, 10.f);
          prob_change_acc = rnd::randn_range_int(4, 6);
          break;
        case Race::Basilisk:
          character = 'S';
          style = { Color::Green, Color::DarkGray };
          //acc_sigma = rnd::randn_range(20.f, 220.f);
          acc_step = rnd::randn_range(5.f, 18.f)/10.f;
          acc_lim = rnd::randn_range(2.f, 25.f);
          vel_lim = rnd::randn_range(4.f, 8.f);
          prob_change_acc = rnd::randn_range_int(16, 28);
          break;
        case Race::Bear:
          character = 'B';
          style = { Color::Red, Color::DarkRed };
          //acc_sigma = rnd::randn_range(10.f, 250.f);
          acc_step = rnd::randn_range(10.f, 25.f)/10.f;
          acc_lim = rnd::randn_range(3.f, 10.f);
          vel_lim = rnd::randn_range(3.f, 18.f);
          prob_change_acc = rnd::randn_range_int(5, 8);
          break;
        case Race::Kobold:
          character = 'x';
          style = { Color::Blue, Color::LightGray };
          //acc_sigma = rnd::randn_range(20.f, 110.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(25.f, 40.f);
          vel_lim = rnd::randn_range(2.f, 10.f);
          prob_change_acc = rnd::randn_range_int(3, 9);
          break;
        case Race::Skeleton:
          character = '%';
          style = { Color::White, Color::DarkGray };
          //acc_sigma = rnd::randn_range(2.f, 25.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(10.f, 60.f);
          vel_lim = rnd::randn_range(1.f, 4.f);
          prob_change_acc = rnd::randn_range_int(11, 19);
          break;
        case Race::Giant:
          character = 'O';
          style = { Color::DarkMagenta, Color::LightGray };
          //acc_sigma = rnd::randn_range(15.f, 60.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(1.f, 5.f);
          vel_lim = rnd::randn_range(0.5f, 4.5f);
          prob_change_acc = rnd::randn_range_int(20, 40);
          break;
        case Race::Huge_Spider:
          character = 'W';
          style = { Color::DarkGray, Color::White };
          //acc_sigma = rnd::randn_range(20.f, 260.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(10.f, 70.f);
          vel_lim = rnd::randn_range(3.f, 20.f);
          prob_change_acc = rnd::randn_range_int(3, 17);
          break;
        case Race::Wolf:
          character = 'm';
          style = { Color::LightGray, Color::DarkGray };
          //acc_sigma = rnd::randn_range(110.f, 330.f);
          acc_step = rnd::randn_range(15.f, 35.f)/10.f;
          acc_lim = rnd::randn_range(15.f, 60.f);
          vel_lim = rnd::randn_range(10.f, 24.f);
          prob_change_acc = rnd::randn_range_int(2, 9);
          break;
        case Race::Wyvern:
          character = 'w';
          style = { Color::DarkMagenta, Color::Blue };
          //acc_sigma = rnd::randn_range(90.f, 220.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(2.f, 15.f);
          vel_lim = rnd::randn_range(8.f, 20.f);
          prob_change_acc = rnd::randn_range_int(7, 15);
          break;
        case Race::Griffin:
          character = 'g';
          style = { Color::DarkRed, Color::Blue };
          //acc_sigma = rnd::randn_range(100.f, 250.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(10.f, 25.f);
          vel_lim = rnd::randn_range(9.f, 21.f);
          prob_change_acc = rnd::randn_range_int(10, 20);
          break;
        case Race::Ghoul:
          character = 'h';
          style = { Color::LightGray, Color::Yellow };
          //acc_sigma = rnd::randn_range(35.f, 115.f);
          acc_step = rnd::randn_range(5.f, 15.f)/10.f;
          acc_lim = rnd::randn_range(30.f, 60.f);
          vel_lim = rnd::randn_range(10.f, 20.f);
          prob_change_acc = rnd::randn_range_int(1, 5);
          break;
        case Race::Dragon:
          character = 'R';
          style = { Color::Red, Color::DarkMagenta };
          //acc_sigma = rnd::randn_range(70.f, 350.f);
          acc_step = rnd::randn_range(5.f, 45.f)/10.f;
          acc_lim = rnd::randn_range(7.f, 30.f);
          vel_lim = rnd::randn_range(11.f, 29.f);
          prob_change_acc = rnd::randn_range_int(14, 30);
          break;
        case Race::NUM_ITEMS:
          break;
      }
    }
    
    void update(float dt)
    {
      if (health == 0)
      {
        character = '&';
        style = { Color::Red, Color::DarkGray };
        return;
      }
    
      if (wall_coll_resolve)
      {
        if (wall_coll_resolve_ctr++ < 2)
        {
          wall_coll_resolve_ctr = 0;
          wall_coll_resolve = false;
        }
      }
      else if (rnd::rand_int(0, prob_change_acc) == 0)
      {
        acc_r += rnd::randn_range(-acc_step, +acc_step);
        acc_c += rnd::randn_range(-acc_step*px_aspect, +acc_step*px_aspect);
        acc_r = math::clamp<float>(acc_r, -acc_lim, +acc_lim);
        acc_c = math::clamp<float>(acc_c, -acc_lim*px_aspect, +acc_lim*px_aspect);
      }
      vel_r += acc_r*dt;
      vel_c += acc_c*dt;
      vel_r = math::clamp<float>(vel_r, -vel_lim, +vel_lim);
      vel_c = math::clamp<float>(vel_c, -vel_lim*px_aspect, +vel_lim*px_aspect);
      pos_r += vel_r*dt;
      pos_c += vel_c*dt;
      auto r = math::roundI(pos_r);
      auto c = math::roundI(pos_c);
      auto location_corr = ttl::BBLocation::None;
      auto location_room = ttl::BBLocation::None;
      if (curr_corridor != nullptr)
        curr_corridor->is_inside_corridor({r, c}, &location_corr);//check_bb_location(r, c);
      if (curr_room != nullptr)
        curr_room->is_inside_room({r, c}, &location_corr);
      if (location_corr == ttl::BBLocation::Inside || location_room == ttl::BBLocation::Inside)
      {
        pos.r = r;
        pos.c = c;
        wall_coll_resolve_ctr = 0;
        wall_coll_resolve = false;
      }
      else if (!wall_coll_resolve && rnd::rand_int(0, 5) == 0)
      {
        auto location = ttl::BBLocation::None;
        if (location_room != ttl::BBLocation::None && location_corr != ttl::BBLocation::None)
        {
          //if (rnd::rand_int(0, 1) == 0)
          //  location = location_room;
          //else
          //  location = location_corr;
          location = ttl::BBLocation::None;
          acc_r = 0.f;
          vel_r = 5.f;
        }
        else if (location_room != ttl::BBLocation::None)
          location = location_room;
        else if (location_corr != ttl::BBLocation::None)
          location = location_corr;
        pos_r = pos.r;
        pos_c = pos.c;
        const float c_res_acc = 0.f;
        const float c_res_vel = 5.f;
        switch (location)
        {
          case ttl::BBLocation::OutsideTop:
            acc_r = c_res_acc;
            vel_r = c_res_vel;
            break;
          case ttl::BBLocation::OutsideTopLeft:
            acc_r = c_res_acc;
            acc_c = c_res_acc;
            vel_r = c_res_vel;
            vel_c = c_res_vel;
            break;
          case ttl::BBLocation::OutsideLeft:
            acc_c = c_res_acc;
            vel_c = c_res_vel;
            break;
          case ttl::BBLocation::OutsideBottomLeft:
            acc_r = -c_res_acc;
            acc_c = c_res_acc;
            vel_r = -c_res_vel;
            vel_c = c_res_vel;
            break;
          case ttl::BBLocation::OutsideBottom:
            acc_r = -c_res_acc;
            vel_r = -c_res_vel;
            break;
          case ttl::BBLocation::OutsideBottomRight:
            acc_r = -c_res_acc;
            acc_c = -c_res_acc;
            vel_r = -c_res_vel;
            vel_c = -c_res_vel;
            break;
          case ttl::BBLocation::OutsideRight:
            acc_c = -c_res_acc;
            vel_c = -c_res_vel;
            break;
          case ttl::BBLocation::OutsideTopRight:
            acc_r = c_res_acc;
            acc_c = -c_res_acc;
            vel_r = c_res_vel;
            vel_c = -c_res_vel;
            break;
          default:
            acc_r = 0.f;
            acc_c = 0.f;
            vel_r = 0.f;
            vel_c = 0.f;
            break;
        }
        
        wall_coll_resolve = true;
      }
      
      if (wall_coll_resolve)
        style.bg_color = Color::DarkBlue;
      else
        style.bg_color = Color::DarkYellow;
      
      // Update current room and current corridor.
      if (curr_corridor != nullptr)
      {
        auto* door_0 = curr_corridor->doors[0];
        auto* door_1 = curr_corridor->doors[1];
        if (door_0 != nullptr && pos == door_0->pos)
        {
          curr_room = door_0->room;
          //curr_corridor = nullptr;
        }
        else if (door_1 != nullptr && pos == door_1->pos)
        {
          curr_room = door_1->room;
          //curr_corridor = nullptr;
        }
        else
        {
          last_in_corridor = true;
          last_in_room = false;
        }
      }
      if (curr_room != nullptr)
      {
        bool found_door = false;
        for (auto* door : curr_room->doors)
          if (pos == door->pos)
          {
            curr_corridor = door->corridor;
            //curr_room = nullptr;
            found_door = true;
            break;
          }
        if (!found_door)
        {
          last_in_corridor = false;
          last_in_room = true;
        }
      }
    }
  };
  
}
