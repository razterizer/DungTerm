//
//  DungGine.h
//  DungGine
//
//  Created by Rasmus Anthin on 2024-06-13.
//

#pragma once
#include "BSPTree.h"
#include "DungGineStyles.h"
#include <Termin8or/Keyboard.h>
#include <Termin8or/MessageHandler.h>


namespace dung
{

  enum class ScreenScrollingMode { AlwaysInCentre, PageWise, WhenOutsideScreen };

  class DungGine
  {
    BSPTree* m_bsp_tree;
    std::vector<BSPNode*> m_leaves;
    
    using WallType = drawing::OutlineType; //{ Hash, Masonry, Masonry1, Masonry2, Masonry3, Temple };
    using Direction = drawing::Direction;
    using Style = styles::Style;
    using HiliteFGStyle = styles::HiliteFGStyle;
    enum class FloorType { None, Sand, Grass, Stone, Stone2, Water, Wood, NUM_ITEMS };
    
    struct RoomStyle
    {
      WallType wall_type = WallType::Hash;
      Style wall_style { Color::DarkGray, Color::LightGray };
      FloorType floor_type = FloorType::None;
      bool is_underground = true;
      
      void init_rand()
      {
        wall_type = rnd::rand_enum<WallType>();
        WallBasicType wall_basic_type = WallBasicType::Other;
        switch (wall_type)
        {
          case WallType::Masonry:
          case WallType::Masonry2:
          case WallType::Masonry3:
          case WallType::Masonry4:
            wall_basic_type = WallBasicType::Masonry;
          case WallType::Temple:
            wall_basic_type = WallBasicType::Temple;
          case WallType::Line:
          case WallType::Hash:
          default:
            wall_basic_type = WallBasicType::Other;
            break;
        }
        wall_style = styles::get_random_style(wall_palette[wall_basic_type]);
        floor_type = rnd::rand_enum<FloorType>();
        is_underground = rnd::rand_bool();
      }
      
      char get_fill_char() const
      {
        switch (floor_type)
        {
          case FloorType::Sand:
            return ':';
          case FloorType::Grass:
            return '|';
          case FloorType::Stone:
            return 'H';
          case FloorType::Stone2:
            return '8';
          case FloorType::Water:
            return '~';
          case FloorType::Wood:
            return 'W';
          case FloorType::None:
          default:
            return ' ';
            break;
        }
      }
      
      char get_shadow_char() const
      {
        return get_fill_char();
      }
      
      Style get_fill_style() const
      {
        Style style;
        switch (floor_type)
        {
          case FloorType::Sand:
            style = styles::make_shaded_style(Color::Yellow, color::ShadeType::Bright);
            break;
          case FloorType::Grass:
            style = styles::make_shaded_style(Color::Green, color::ShadeType::Bright);
            break;
          case FloorType::Stone:
          case FloorType::Stone2:
            style = styles::make_shaded_style(Color::LightGray, color::ShadeType::Bright);
            break;
          case FloorType::Water:
            style = styles::make_shaded_style(Color::Blue, color::ShadeType::Bright);
            break;
          case FloorType::Wood:
            style = { Color::DarkRed, Color::Yellow };
            break;
          case FloorType::None:
          default:
            style = { Color::DarkGray, Color::LightGray };
            break;
        }
        if (is_underground)
          style.swap();
        return style;
      }

    };
    
    std::map<BSPNode*, RoomStyle> m_room_styles;
    
    Direction m_sun_dir = Direction::E;
    Direction m_shadow_dir = Direction::W;
    float m_sun_minutes_per_day = 20.f;
    float m_sun_t_offs = 0.f;
    
    struct Item
    {
      RC pos; // world pos
      bool picked_up = false;
      Style style;
      char character = '?';
      bool fog_of_war = true;
    };
  
    struct Key : Item
    {
      Key()
      {
        character = 'F';
        style.fg_color = Color::Green;
        style.bg_color = Color::Transparent2;
      }
      
      int key_id = 0;
      
      void randomize_fg_color()
      {
        style.fg_color = color::get_random_color(key_fg_palette);
      }
    };
    
    struct Lamp : Item
    {
      Lamp()
      {
        character = 'Y';
        style.fg_color = Color::Yellow;
        style.bg_color = Color::Transparent2;
      }
      enum class LampType { Isotropic, Directional, NUM_ITEMS };
      LampType type = LampType::Isotropic;
    };
    
    struct Player
    {
      char character = '@';
      Style style = { Color::Magenta, Color::White };
      RC world_pos;
      bool is_spawned = false;
      BSPNode* curr_room = nullptr;
      Corridor* curr_corridor = nullptr;
      
      std::vector<int> key_idcs;
      std::vector<int> lamp_idcs;
      int inv_sel_idx = 0;
      bool show_inventory = false;
      RC line_of_sight;
    };
    
    Player m_player;
    ttl::Rectangle m_screen_in_world;
    // Value between 0 and 1 where 1 means a full screen vertically or horizontally.
    // Fraction of screen that will be scrolled (when in PageWise scroll mode).
    float t_scroll_amount = 0.2f;
    ScreenScrollingMode scr_scrolling_mode = ScreenScrollingMode::AlwaysInCentre;
    
    // (0,0) world pos
    // +--------------------+
    // | (5,8) scr world pos|
    // |    +-------+       |
    // |    |       |       |
    // |    |    @  |       |  <---- (8, 20) player world pos
    // |    +-------+       |
    // |                    |
    // |                    |
    // +--------------------+
    
    std::vector<Key> all_keys;
    std::vector<Lamp> all_lamps;
    
    std::unique_ptr<MessageHandler> message_handler;
    bool use_fog_of_war = false;
    
    RC get_screen_pos(const RC& world_pos) const
    {
      return world_pos - m_screen_in_world.pos();
    }
    
    void update_sun(float sim_time_s)
    {
      float t_solar_period = std::fmod(m_sun_t_offs + (sim_time_s / 60.f) / m_sun_minutes_per_day, 1);
      //int idx = static_cast<int>(m_sun_dir);
      //idx += t_solar_period * (static_cast<float>(Direction::NUM_ITEMS) - 1.f);
      static constexpr auto dp = 1.f/8.f; // solar period step (delta period).
      for (int i = 0; i < 8; ++i)
      {
        // 2 means east: S(0), SE(1), E(2). The sun comes up from the east.
        int curr_dir_idx = (i+2) % 8;
        if ((static_cast<int>(m_sun_dir) - 1) != curr_dir_idx)
        {
          if (math::in_range<float>(t_solar_period, i*dp, (i+1)*dp, Range::ClosedOpen))
          {
            m_sun_dir = static_cast<Direction>(curr_dir_idx + 1);
            break;
          }
        }
      }
    }
    
    // #NOTE: Only for unwalled area!
    bool is_inside_any_room(const RC& pos)
    {
      for (auto* leaf : m_leaves)
        if (leaf->bb_leaf_room.is_inside_offs(pos, -1))
          return true;
      return false;
    }
    
  public:
    DungGine(bool use_fow)
      : message_handler(std::make_unique<MessageHandler>())
      , use_fog_of_war(use_fow)
    {}
    
    void load_dungeon(BSPTree* bsp_tree)
    {
      m_bsp_tree = bsp_tree;
      m_leaves = m_bsp_tree->fetch_leaves();
    }
    
    void style_dungeon()
    {
      for (auto* leaf : m_leaves)
      {
        RoomStyle room_style;
        room_style.init_rand();
        m_room_styles[leaf] = room_style;
      }
    }
    
    void set_player_character(char ch) { m_player.character = ch; }
    bool place_player(const RC& screen_size, std::optional<RC> world_pos = std::nullopt)
    {
      const auto world_size = m_bsp_tree->get_world_size();
      m_screen_in_world.set_size(screen_size);
    
      if (world_pos.has_value())
        m_player.world_pos = world_pos.value();
      else
        m_player.world_pos = world_size / 2;
      
      const auto& room_corridor_map = m_bsp_tree->get_room_corridor_map();
      
      const int c_max_num_iters = 1e5;
      int num_iters = 0;
      do
      {
        for (const auto& cp : room_corridor_map)
          if (cp.second->is_inside_corridor(m_player.world_pos))
          {
            m_player.is_spawned = true;
            m_player.curr_corridor = cp.second;
            m_screen_in_world.set_pos(m_player.world_pos - m_screen_in_world.size()/2);
            return true;
          }
        m_player.world_pos += { rnd::rand_int(-2, +2), rnd::rand_int(-2, +2) };
        m_player.world_pos = m_player.world_pos.clamp(0, world_size.r, 0, world_size.c);
      } while (++num_iters < c_max_num_iters);
      return false;
    }
    
    // Randomizes the starting direction of the sun.
    void configure_sun(float minutes_per_day)
    {
      Direction sun_dir = static_cast<Direction>(rnd::rand_int(0, 7) + 1);
      configure_sun(sun_dir, minutes_per_day);
    }
    
    void configure_sun(Direction sun_dir, float minutes_per_day)
    {
      if (sun_dir == Direction::None || sun_dir == Direction::NUM_ITEMS)
      {
        std::cerr << "ERROR: invalid value of sun direction supplied to function DungGine::configure_sun()!" << std::endl;
        return;
      }
      m_sun_dir = sun_dir;
      m_sun_minutes_per_day = minutes_per_day;
      // None, S, SE, E, NE, N, NW, W, SW, NUM_ITEMS
      // 0     1  2   3  4   5  6   7  8
      m_sun_t_offs = (static_cast<int>(sun_dir) - 1) / 8.f;
    }
    
    bool place_keys()
    {
      const auto world_size = m_bsp_tree->get_world_size();
      const auto& door_vec = m_bsp_tree->fetch_doors();
      const int c_max_num_iters = 1e5;
      int num_iters = 0;
      for (auto* d : door_vec)
      {
        if (d->is_locked)
        {
          Key key;
          key.key_id = d->key_id;
          key.randomize_fg_color();
          do
          {
            key.pos =
            {
              rnd::rand_int(0, world_size.r),
              rnd::rand_int(0, world_size.c)
            };
          } while (num_iters++ < c_max_num_iters && !is_inside_any_room(key.pos));
          
          if (!is_inside_any_room(key.pos))
            return false;
            
          all_keys.emplace_back(key);
        }
      }
      return true;
    }
    
    bool place_lamps(int num_lamps)
    {
      const auto world_size = m_bsp_tree->get_world_size();
      const int c_max_num_iters = 1e5;
      int num_iters = 0;
      for (int lamp_idx = 0; lamp_idx < num_lamps; ++lamp_idx)
      {
        Lamp lamp;
        lamp.type = rnd::rand_enum<Lamp::LampType>();
        do
        {
          lamp.pos =
          {
            rnd::rand_int(0, world_size.r),
            rnd::rand_int(0, world_size.c)
          };
        } while (num_iters++ < c_max_num_iters && !is_inside_any_room(lamp.pos));
        
        if (!is_inside_any_room(lamp.pos))
          return false;
        
        all_lamps.emplace_back(lamp);
      }
      return true;
    }
    
    void set_screen_scrolling_mode(ScreenScrollingMode mode, float t_page = 0.2f)
    {
      scr_scrolling_mode = mode;
      if (mode == ScreenScrollingMode::PageWise)
        t_scroll_amount = t_page;
    }
    
    void update(double sim_time_s, const keyboard::KeyPressData& kpd)
    {
      update_sun(static_cast<float>(sim_time_s));
      int sun_dir_idx = static_cast<int>(m_sun_dir) - 1;
      m_shadow_dir = static_cast<Direction>(((sun_dir_idx + 4) % 8) + 1);
      
      auto is_inside_curr_bb = [&](int r, int c) -> bool
      {
        if (m_player.curr_corridor != nullptr && m_player.curr_corridor->is_inside_corridor({r, c}))
          return true;
        if (m_player.curr_room != nullptr && m_player.curr_room->is_inside_room({r, c}))
          return true;
        return false;
      };
      
      auto& curr_pos = m_player.world_pos;
      if (str::to_lower(kpd.curr_key) == 'a')
      {
        if (is_inside_curr_bb(curr_pos.r, curr_pos.c - 1))
          curr_pos.c--;
      }
      else if (str::to_lower(kpd.curr_key) == 'd')
      {
        if (is_inside_curr_bb(curr_pos.r, curr_pos.c + 1))
          curr_pos.c++;
      }
      else if (str::to_lower(kpd.curr_key) == 's')
      {
        if (is_inside_curr_bb(curr_pos.r + 1, curr_pos.c))
          curr_pos.r++;
      }
      else if (str::to_lower(kpd.curr_key) == 'w')
      {
        if (is_inside_curr_bb(curr_pos.r - 1, curr_pos.c))
          curr_pos.r--;
      }
      else if (kpd.curr_key == ' ')
      {
        if (m_player.curr_corridor != nullptr && m_player.curr_corridor->is_inside_corridor(curr_pos))
        {
          auto* door_0 = m_player.curr_corridor->doors[0];
          auto* door_1 = m_player.curr_corridor->doors[1];
          if (door_0 != nullptr && (!door_0->is_locked && door_0->is_door) && distance(curr_pos, door_0->pos) == 1.f)
            math::toggle(door_0->is_open);
          if (door_1 != nullptr && (!door_1->is_locked && door_1->is_door) && distance(curr_pos, door_1->pos) == 1.f)
            math::toggle(door_1->is_open);
        }
        else if (m_player.curr_room != nullptr && m_player.curr_room->is_inside_room(curr_pos))
        {
          for (auto* door : m_player.curr_room->doors)
          {
            if ((!door->is_locked && door->is_door) && distance(curr_pos, door->pos) == 1.f)
            {
              math::toggle(door->is_open);
              break;
            }
          }
        }

        for (size_t key_idx = 0; key_idx < all_keys.size(); ++key_idx)
        {
          auto& key = all_keys[key_idx];
          if (key.pos == curr_pos)
          {
            m_player.key_idcs.emplace_back(key_idx);
            key.picked_up = true;
            message_handler->add_message(static_cast<float>(sim_time_s),
                                         "You picked up a key!", MessageHandler::Level::Guide);
          }
        }
        for (size_t lamp_idx = 0; lamp_idx < all_lamps.size(); ++lamp_idx)
        {
          auto& lamp = all_lamps[lamp_idx];
          if (lamp.pos == curr_pos)
          {
            m_player.lamp_idcs.emplace_back(lamp_idx);
            lamp.picked_up = true;
            message_handler->add_message(static_cast<float>(sim_time_s),
                                         "You picked up a lamp!", MessageHandler::Level::Guide);
          }
        }
      }
      else if (str::to_lower(kpd.curr_key) == 'i')
      {
        math::toggle(m_player.show_inventory);
      }
      
      // Update current room and current corridor.
      if (m_player.curr_corridor != nullptr)
      {
        auto* door_0 = m_player.curr_corridor->doors[0];
        auto* door_1 = m_player.curr_corridor->doors[1];
        if (door_0 != nullptr && curr_pos == door_0->pos)
          m_player.curr_room = door_0->room;
        else if (door_1 != nullptr && curr_pos == door_1->pos)
          m_player.curr_room = door_1->room;
      }
      if (m_player.curr_room != nullptr)
      {
        for (auto* door : m_player.curr_room->doors)
          if (curr_pos == door->pos)
          {
            m_player.curr_corridor = door->corridor;
            break;
          }
      }
      
      // Fog of war
      if (use_fog_of_war)
      {
        const auto c_fow_dist = 2.3f;
        for (auto& key : all_keys)
          if (distance(key.pos, curr_pos) <= c_fow_dist)
            key.fog_of_war = false;
        
        ttl::Rectangle bb;
        RC local_pos;
        RC size;
        bool_vector* fog_of_war = nullptr;
        auto set_fow_pos = [&](const RC& p)
        {
          if (fog_of_war == nullptr)
            return;
          if (p.r < 0 || p.c < 0 || p.r > bb.r_len || p.c > bb.c_len)
            return;
          // ex:
          // +---+
          // |   |
          // +---+
          // r_len = 2, c_len = 4
          // FOW size = 3*5
          // idx = 0 .. 14
          // r = 2, c = 4 => idx = r * (c_len + 1) + c = 2*5 + 4 = 14.
          int idx = p.r * (size.c + 1) + p.c;
          if (0 <= idx && idx < fog_of_war->size())
            (*fog_of_war)[idx] = false;
        };
        
        auto update_fow = [&]() // #FIXME: FHXFTW
        {
          local_pos = curr_pos - bb.pos();
          size = bb.size();
          
          //    ###
          //   #####
          //    ###
          set_fow_pos(local_pos);
          for (int c = -1; c <= +1; ++c)
          {
            set_fow_pos(local_pos + RC { -1, c });
            set_fow_pos(local_pos + RC { +1, c });
          }
          for (int c = -2; c <= +2; ++c)
            set_fow_pos(local_pos + RC { 0, c });
          
          int r_room = -1;
          int c_room = -1;
          if (curr_pos.r - bb.top() <= 1)
            r_room = 0;
          else if (bb.bottom() - curr_pos.r <= 1)
            r_room = bb.r_len;
          if (curr_pos.c - bb.left() <= 1)
            c_room = 0;
          else if (bb.right() - curr_pos.c <= 1)
            c_room = bb.c_len;
          
          if (r_room >= 0 && c_room >= 0)
            set_fow_pos({ r_room, c_room });
        };
        
        if (m_player.curr_corridor != nullptr && m_player.curr_corridor->is_inside_corridor(curr_pos))
        {
          bb = m_player.curr_corridor->bb;
          fog_of_war = &m_player.curr_corridor->fog_of_war;
          
          auto* door_0 = m_player.curr_corridor->doors[0];
          auto* door_1 = m_player.curr_corridor->doors[1];
          update_fow();
          
          if (distance(door_0->pos, curr_pos) <= c_fow_dist)
            door_0->fog_of_war = false;
          if (distance(door_1->pos, curr_pos) <= c_fow_dist)
            door_1->fog_of_war = false;
        }
        if (m_player.curr_room != nullptr && m_player.curr_room->is_inside_room(curr_pos))
        {
          bb = m_player.curr_room->bb_leaf_room;
          fog_of_war = &m_player.curr_room->fog_of_war;
          update_fow();
          
          for (auto* door : m_player.curr_room->doors)
            if (distance(door->pos, curr_pos) <= c_fow_dist)
              door->fog_of_war = false;
        }
      }
      
      // Scrolling mode.
      switch (scr_scrolling_mode)
      {
        case ScreenScrollingMode::AlwaysInCentre:
          m_screen_in_world.set_pos(curr_pos - m_screen_in_world.size()/2);
          break;
        case ScreenScrollingMode::PageWise:
        {
          int offs_v = -static_cast<int>(std::round(m_screen_in_world.r_len*t_scroll_amount));
          int offs_h = -static_cast<int>(std::round(m_screen_in_world.c_len*t_scroll_amount));
          if (!m_screen_in_world.is_inside_offs(curr_pos, offs_v, offs_h))
            m_screen_in_world.set_pos(curr_pos - m_screen_in_world.size()/2);
          break;
        }
        case ScreenScrollingMode::WhenOutsideScreen:
          if (!m_screen_in_world.is_inside(curr_pos))
          {
            if (curr_pos.r < m_screen_in_world.top())
              m_screen_in_world.r -= m_screen_in_world.r_len;
            else if (curr_pos.r > m_screen_in_world.bottom())
              m_screen_in_world.r += m_screen_in_world.r_len;
            else if (curr_pos.c < m_screen_in_world.left())
              m_screen_in_world.c -= m_screen_in_world.c_len;
            else if (curr_pos.c > m_screen_in_world.right())
              m_screen_in_world.c += m_screen_in_world.c_len;
          }
          break;
        default:
          break;
      }
    }
    
    template<int NR, int NC>
    void draw(SpriteHandler<NR, NC>& sh, double sim_time_s) const
    {
      const auto& room_corridor_map = m_bsp_tree->get_room_corridor_map();
      const auto& door_vec = m_bsp_tree->fetch_doors();
      
      message_handler->update(sh, static_cast<float>(sim_time_s), true);
      
      if (m_player.show_inventory)
      {
        sh.write_buffer(str::adjust_str("Inventory", str::Adjustment::Center, NC - 1), 4, 0, Color::White, Color::Transparent2);
        drawing::draw_box(sh, 2, 2, NR - 5, NC - 5, drawing::OutlineType::Line, { Color::White, Color::DarkGray }, { Color::White, Color::DarkGray }, ' ');
      }
      
      if (m_player.is_spawned)
      {
        auto player_scr_pos = get_screen_pos(m_player.world_pos);
        sh.write_buffer(std::string(1, m_player.character), player_scr_pos.r, player_scr_pos.c, m_player.style);
      }
      
      for (auto* door : door_vec)
      {
        auto door_pos = door->pos;
        auto door_scr_pos = get_screen_pos(door_pos);
        std::string door_ch = "^";
        if (door->is_door)
        {
          if (door->is_open)
            door_ch = "L";
          else if (door->is_locked)
            door_ch = "G";
          else
            door_ch = "D";
        }
        sh.write_buffer(door_ch, door_scr_pos.r, door_scr_pos.c, Color::Black, (use_fog_of_war && door->fog_of_war) ? Color::Black : Color::Yellow);
      }
      
      for (const auto& key : all_keys)
      {
        if (key.picked_up || (use_fog_of_war && key.fog_of_war))
          continue;
        auto key_scr_pos = get_screen_pos(key.pos);
        sh.write_buffer(std::string(1, key.character), key_scr_pos.r, key_scr_pos.c, key.style);
      }
      
      for (const auto& lamp : all_lamps)
      {
        if (lamp.picked_up)
          continue;
        auto lamp_scr_pos = get_screen_pos(lamp.pos);
        sh.write_buffer(std::string(1, lamp.character), lamp_scr_pos.r, lamp_scr_pos.c, lamp.style);
      }
      
      auto shadow_type = m_shadow_dir;
      for (const auto& room_pair : m_room_styles)
      {
        auto* room = room_pair.first;
        const auto& bb = room->bb_leaf_room;
        const auto& room_style = room_pair.second;
        auto bb_scr_pos = get_screen_pos(bb.pos());
        
        // Fog of war
        if (use_fog_of_war)
        {
          for (int r = 0; r <= bb.r_len; ++r)
          {
            for (int c = 0; c <= bb.c_len; ++c)
            {
              if (room->fog_of_war[r * (bb.c_len + 1) + c])
                sh.write_buffer(".", bb_scr_pos.r + r, bb_scr_pos.c + c, Color::Black, Color::Black);
            }
          }
        }
        
        drawing::draw_box(sh,
                          bb_scr_pos.r, bb_scr_pos.c, bb.r_len, bb.c_len,
                          room_style.wall_type,
                          room_style.wall_style,
                          room_style.get_fill_style(),
                          room_style.get_fill_char(),
                          room_style.is_underground ? Direction::None : shadow_type,
                          styles::shade_style(room_style.get_fill_style(), color::ShadeType::Dark),
                          room_style.get_fill_char());
      }
      
      for (const auto& cp : room_corridor_map)
      {
        auto* corr = cp.second;
        const auto& bb = corr->bb;
        auto bb_scr_pos = get_screen_pos(bb.pos());
        
        // Fog of war
        if (use_fog_of_war)
        {
          for (int r = 0; r <= bb.r_len; ++r)
          {
            for (int c = 0; c <= bb.c_len; ++c)
            {
              if (corr->fog_of_war[r * (bb.c_len + 1) + c])
                sh.write_buffer(".", bb_scr_pos.r + r, bb_scr_pos.c + c, Color::Black, Color::Black);
            }
          }
        }
        
        drawing::draw_box(sh,
                          bb_scr_pos.r, bb_scr_pos.c, bb.r_len, bb.c_len,
                          WallType::Masonry4,
                          { Color::LightGray, Color::Black }, //wall_palette[WallBasicType::Masonry],
                          { Color::DarkGray, Color::LightGray },
                          '8',
                          shadow_type,
                          { Color::LightGray, Color::DarkGray },
                          '8');
      }
    }
    
  };
  
}
