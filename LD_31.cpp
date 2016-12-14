#include "Cigarette_Butt.h"
#include "LD_31.h"
#include "Cigarette_Butt.h"

#include <fstream>
#include <tuple>

namespace Game {
  Tile::Tile(cig::Sprite* img, int l, int x, int y) : Object(img, l, x, y) {}

  std::vector<std::vector<Tile*>> map;
  std::vector<cig::Object*> enemies;
  Player* gl_player = nullptr;
  std::vector<Cig_Node*> player_cigs;
  std::vector<Keyhole*> keyholes;

  int map_width = 0, map_height = 0;

  std::vector<cig::Object*> call_refresh_objs;

  const float _Pi = 3.14159f;

  std::vector<std::string> map_names {
    "map001.txt", "map002.txt", "map003.txt", "map004.txt", "map005.txt", "map006.txt",
    "map007.txt", "map008.txt", "map009.txt", "map010.txt", "map011.txt"
  };
  std::vector<std::string> map_text_names {
    "map001_dialogue.txt", "map002_dialogue.txt", "map003_dialogue.txt", "map004_dialogue.txt", "map005_dialogue.txt", "map006_dialogue.txt",
    "map007_dialogue.txt", "map008_dialogue.txt", "map009_dialogue.txt", "map010_dialogue.txt", "map011_dialogue.txt"
  };
  int current_level = -1, current_stage = 0;

  // IMAGES
  cig::Sprite* spr_cig_node = new cig::Sprite("cig_node_img.png"),
             * spr_turtle   = new cig::Sprite("enemy_turtle.png"),
             * spr_hipster  = new cig::Sprite("enemy_hipster.png"),
             * spr_MJ       = new cig::Sprite("enemy_MJ.png"),
             * spr_enemy_cig_node = new cig::Sprite("enemy_cig_node_img.png"),
             * spr_jump_pad = new cig::Sprite("Jump_Pad.png"),
             * spr_health_bar = new cig::Sprite("st_healthbar.png"),
             * spr_key        = new cig::Sprite("key.png"),
             * spr_keyhole    = new cig::Sprite("keyhole.png"),
             * spr_next_level = new cig::Sprite("next_level.png"),
             * spr_death      = new cig::Sprite("death_block.png"),
             * spr_GUI          = new cig::Sprite("shitty_GUI.png"), 
             * spr_turtle_heart = new cig::Sprite("Turtle_Heart.png"),
             * spr_hp_outline  =  new cig::Sprite("healthbar_outline.png"),
             * spr_pl_health   =  new cig::Sprite("healthbar.png"),
             * spr_shit_menu   =  new cig::Sprite("shitty_menu.png"),
             * spr_deathscreen =  new cig::Sprite("Death_Screen.png"),
             * spr_enemy_turtle = new cig::Sprite("enemy_turtle.png"),
             * spr_enemy_hipster = new cig::Sprite("enemy_hipster.png"),
             * spr_player_img    = new cig::Sprite("player_img.png"),
             * spr_tile          = new cig::Sprite("tile_img_0.png");
  cig::Animation* spr_upgrade_node = new cig::Animation("upgrade_node.png",32,32,2),
                * anim_enemy_spinner = new cig::Animation("enemy_spinner.png",64,64,1),
                * spr_enemy_mj      = new cig::Animation("enemy_MJ.png",15,32,2);
  cig::Animation* spr_explosion    = new cig::Animation("explosion.png",32,32,4);

  void Goto_Next_Level(Game::Stages stage) {
    // clean everything up
    /*for ( int i = 0; i != map.size(); ++ i ) {
      for ( int o = 0; o != map[i].size(); ++ o ) {
        delete map[i][o];
      }
      if ( map.size() < i )
        map[i].erase(map[i].begin(),map[i].end());
    }*/
    map.erase(map.begin(), map.end());
    using namespace cig;

    /*for ( int i = 0; i != call_refresh_objs.size(); ++ i ) {
      // first remove from objs
      for ( int x = 0; x != objs.size(); ++ x )
        for ( int y = 0; y != objs[x].size(); ++ y )
          if ( objs[x][y] == call_refresh_objs[i] ) {
            objs[x][y] = nullptr;
          }
      delete call_refresh_objs[i];
    }*/

    
    for ( int i = 0; i != 4; ++ i ) {
    ///  for ( int o = 0; o != sprs[i].size(); ++ o )
       // delete sprs[i][o];
      sprs[i].erase(sprs[i].begin(), sprs[i].end());
    }

    /*for ( int i = 0; i != objs.size(); ++ i )
      for ( int o = 0; o != objs[i].size(); ++ o ) {
        delete objs[i][o];
      }
    for ( int i = 0; i != texts.size(); ++ i )
      delete texts[i];*/

    cig::objs.erase(cig::objs.begin(), cig::objs.end());
    cig::texts.erase(cig::texts.begin(), cig::texts.end());
    Game::enemies.erase(enemies.begin(), enemies.end());
    keyholes.erase(keyholes.begin(), keyholes.end());

    call_refresh_objs.erase(call_refresh_objs.begin(), call_refresh_objs.end());
    
    gl_player = nullptr;
    camera_offset_x = camera_offset_y = 0;
    auto& asdf = sprs[0];
    // load map
    if ( stage == Stages::Playing ) {
      current_stage = Stages::Playing;
      Load_Dialogue(map_text_names[current_level].c_str());
      Load_Map(map_names[current_level].c_str());
      // load GUI
      cig::GUI* t = new cig::GUI();
      t->pos_x = 0;
      t->pos_y = 768 - 80;
      t->spr = spr_GUI;
      cig::Add_Spr(t,cig::Spr_Layer::Pre_Foreground);
      t = new cig::GUI();
      t->pos_x = 200;
      t->pos_y = 768 - 80 + 20;
      t->spr = spr_hp_outline;
      cig::Add_Spr(t,cig::Spr_Layer::Foreground);
      t = new Game::Health_Bar();
      t->pos_x = 200;
      t->pos_y = 768 - 80 + 20;
      t->spr = spr_pl_health;
      static_cast<Game::Health_Bar*>(t)->Set_Owner(Game::gl_player);
      cig::Add_Spr(t,cig::Spr_Layer::Foreground);

    } else if ( stage == Stages::Menu ) { // load menu
      current_stage = Stages::Menu;
      GUI* zt = new cig::GUI;
      zt->pos_x = 0;
      zt->pos_y = 0;
      zt->spr = spr_shit_menu;
      sprs[0].push_back(zt);
    } else { // dead
      current_stage = Stages::Dead;
      GUI* zt = new cig::GUI;
      zt->pos_x = 0;
      zt->pos_y = 0;
      zt->spr = spr_deathscreen;
      sprs[0].push_back(zt);
    }
  }


  void Load_Dialogue(const char* fil_name) {
    std::ifstream file(fil_name);
    if ( !file.good() ) {
      // some maps may just not have dialogues so don't exit over it
      cig::debug << "File " << fil_name << " not found in Load Dialogue";
      return;
    }
    char x;
    int px = 0, py = 0;
    std::string str;
    while ( file.good() ) {
      while ( (x = file.get()) && (x != ' ') && file.good())
        px = px*10 + x-'0';
      while ( (x = file.get()) && (x != ' ') && file.good() )
        py = py*10 + x-'0';
      while ( (x = file.get()) && (x != '\n') && file.good() ) {
        if ( x == '#' ) x = ' ';
        str += x;
      }
      cig::texts.push_back(new cig::Text(str, cig::Create_Colour(250,20,20), px, py, 0));
      str = "";
      px = py = 0;
    }
  }

  //
  void Load_Map(const char* fil_name) {
    Tile* tile = new Tile(spr_tile,2,0,0);

    std::ifstream file(fil_name);
    if ( !file.good() ) {
      cig::debug << "File " << fil_name << " not found in Load Map\n";
      cig::Clean_Up();
      exit(0);
    }
    char c;
    int x = 0, y = 0;
    map.push_back(std::vector<Tile*>());
    Tile* t;
    while ( file.good() ) {
      c = file.get();
      x = (map[map.size()-1].size())*32;
      y = (map.size() - 1)*32;
      switch ( c ) {
        case '\n': map.push_back(std::vector<Tile*>()); break;
        case '_':  map[map.size()-1].push_back(nullptr); break;
        case '#':
          t = new Tile(*tile);
          map[map.size()-1].push_back(t);
          t->Set_Pos_X((map[map.size()-1].size()-1)*32);
          t->Set_Pos_Y((map.size()-1)*32);
        break;
        case '$':
          call_refresh_objs.push_back(
              cig::Add_Obj(new Node_Upgrade(spr_upgrade_node, 1, x, y))
          );
          map[map.size()-1].push_back(nullptr);
        break;
        case 'k':
          call_refresh_objs.push_back(cig::Add_Obj(
            new Key(spr_key, 6, x, y))
          );
          map[map.size()-1].push_back(nullptr);
        break;
        case 'K':
          t = new Tile(*tile);
          map[map.size()-1].push_back(t);
          t->Set_Pos_X(x); t->Set_Pos_Y(y);
          keyholes.push_back(static_cast<Keyhole*>(cig::Add_Obj(
            new Keyhole(spr_keyhole, t, 7, x, y))
          ));
        break;
        case '1':
          Game::enemies.push_back(cig::Add_Obj(
              new Game::Cig_Spinner(anim_enemy_spinner,
                          cig::Create_Hitbox(0,0,64,64), 6, 6, x, y)
              )
          );
          map[map.size()-1].push_back(nullptr);
        break;
        case '2':
          Game::enemies.push_back(cig::Add_Obj(
              new Game::Turtle(spr_turtle, 3, 4, x, y)) );
          map[map.size()-1].push_back(nullptr);
        break;
        case '3':
          Game::enemies.push_back(cig::Add_Obj(
              new Game::Hipster(spr_hipster, 3, 4, x, y)) );
          map[map.size()-1].push_back(nullptr);
        break;
        case '4':
          Game::enemies.push_back(cig::Add_Obj(
              new Game::MJ(spr_enemy_mj, 7, x + 5, y - 20)) );
          map[map.size()-1].push_back(nullptr);
        break;
        case 'j':
          call_refresh_objs.push_back(
              cig::Add_Obj(new Jump_Pad(spr_jump_pad, 1, x + 5, y - 1))
          );
          map[map.size()-1].push_back(nullptr);
        break;
        case '@':
          if ( Game::gl_player != nullptr ) {
            cig::debug << "Player already exists in Load_Map";
            cig::Clean_Up();
            exit(0);
          }
          Game::gl_player = static_cast<Game::Player*>(cig::Add_Obj(
              new Game::Player(spr_player_img,
                      cig::Create_Hitbox(3, 2, 9, 26), 3, x, y)));
          map[map.size()-1].push_back(nullptr);
        break;
        case '+':
          call_refresh_objs.push_back(
            cig::Add_Obj(new Next_Level(spr_next_level, 15, x, y)));
          map[map.size()-1].push_back(nullptr);
        break;
        case '*':
          call_refresh_objs.push_back(
            cig::Add_Obj(new Death_Block(spr_death, 15, x, y)));
          t = new Tile(*tile);
          map[map.size()-1].push_back(t);
          t->Set_Pos_X((map[map.size()-1].size()-1)*32);
          t->Set_Pos_Y((map.size()-1)*32);
        break;
        default:
          map[map.size()-1].push_back(nullptr);
        break;
      }
    }

    delete tile;
    map_width  = map[0].size()*32;
    map_height = map.size()*32;
  }

  void Clear_Map() {

  }
  Player::Player(cig::Sprite* img, cig::Hitbox hit, int l, int x, int y):Object(img,hit,l,x,y) {
    health = 100;
    node_amount = 0;
  }
  int Player::R_Health() { return health; }
  int Player::R_Max_Health() { return Max_health; }
  void Player::Add_Health(int x) { health += x; }
  void Player::Add_Node() {
    Cig_Node* x = head_node, *y = nullptr;
    if ( head_node == nullptr ) {
      head_node = static_cast<Game::Cig_Node*>(cig::Add_Obj(
                    new Game::Cig_Node(spr_cig_node, cig::Create_Hitbox(0,0,8,8), 5,
                                       gl_player->R_Pos_X(), gl_player->R_Pos_Y())));
      head_node->Set_Parent_Node(this, true);
      player_cigs.push_back(head_node);
    } else {
      while ( true ) {
        y = x;
        x = x->R_Child_Node();
        if ( x == nullptr ) {
          y->Set_Child_node(
            static_cast<Game::Cig_Node*>(cig::Add_Obj(
                    new Game::Cig_Node(spr_cig_node, cig::Create_Hitbox(0,0,8,8), 5,
                                       gl_player->R_Pos_X(), gl_player->R_Pos_Y())))
          );
          y->R_Child_Node()->Set_Parent_Node(y);
          player_cigs.push_back(y);
          return;
        }
      }
    }

    ++node_amount;
  }
  void Player::Refresh() {
    if ( cig::keys[SDLK_a] ) {
      if ( vel_x > 0 ) vel_x *= .1f;
      vel_x -= .85f;
    } else if ( cig::keys[SDLK_d] ) {
      if ( vel_x < 0 ) vel_x *= .1f;
      vel_x += .85f;
    } else
      vel_x *= .7f;
    // gravity
    vel_y += 1;


    if ( vel_x >  Max_vel ) {
      vel_x *= .85f;
      if ( vel_x < Max_vel ) vel_x = Max_vel;
      if ( vel_x > Max_vel*1.5f ) vel_x = Max_vel*1.5f;
    }
    if ( vel_x < -Max_vel ) {
      vel_x *= .85f;
      if ( vel_x > -Max_vel ) vel_x = -Max_vel;
      if ( vel_x < -Max_vel*1.5f ) vel_x = -Max_vel*1.5f;
    }
    
    if ( vel_y >  12 ) {
      vel_y *= .3f;
      if ( vel_y < 12 )      vel_y = 12;
      if ( vel_y > 12*1.5f ) vel_y = 12*1.5f;
    }
    if ( vel_y < -12.f ) vel_y = -12.f;

    // assume no collision at this point
    pos_x += vel_x;
    { // check for collision
      // could do some serious optimization later on...
      for ( int x  = 0; x  != 2; ++  x ) 
      for ( int y  = 0; y  != 2; ++  y )
      for ( int ix = 0; ix != 2; ++ ix )
      for ( int iy = 0; iy != 2; ++ iy ) {
        int t_x = (pos_x + (x>0?hitbox.w:0) + (ix>0?2:-2))/32,
            t_y = (pos_y + (y>0?hitbox.h:0) + (iy>0?2:-2))/32;
        if ( t_x < 0 || t_y < 0 || t_y >= map.size() || t_x >= map[t_y].size() ) continue;
        Object* obj = map[t_y][t_x];
        if ( obj != nullptr && Collision(obj) ) {
          if ( vel_x > 0 )
            pos_x = obj->R_Pos_X() - hitbox.w - hitbox.x - 1;
          else
            pos_x = obj->R_Pos_X() + obj->R_Hitbox()->w - hitbox.x + 1;
          vel_x = 0;
        }
      }
    }

    // check y
    pos_y += vel_y;
    { // check for collision
      // could do some serious optimization later on...
      for ( int x  = 0; x  != 2; ++  x ) 
      for ( int y  = 0; y  != 2; ++  y )
      for ( int ix = 0; ix != 2; ++ ix )
      for ( int iy = 0; iy != 2; ++ iy ) {
        int t_x = (pos_x + (x>0?hitbox.w:0) + (ix>0?2:-2))/32,
            t_y = (pos_y + (y>0?hitbox.h:0) + (iy>0?2:-2))/32;
        if ( t_x < 0 || t_y < 0 || t_y >= map.size() || t_x >= map[t_y].size() ) continue;
        Object* obj = map[t_y][t_x];
        if ( obj != nullptr && Collision(obj) ) {
          if ( vel_y >= 0 )
            pos_y = obj->R_Pos_Y() - hitbox.h - hitbox.y - 1;
          else
            pos_y = obj->R_Pos_Y() + obj->R_Hitbox()->h - hitbox.y + 1;
          vel_y = 0;
        }
      }
    }
    // camera offsets
    cig::camera_offset_x = pos_x - 1024/2;
    cig::camera_offset_y = pos_y - 768 /2;

    if ( cig::camera_offset_x < 0 ) cig::camera_offset_x = 0;
    if ( cig::camera_offset_y < 0 ) cig::camera_offset_y = 0;

    float mouse_angle = atan2f((cig::mouse_y + cig::camera_offset_y) - gl_player->R_Pos_Y() - gl_player->R_Hitbox()->w/2,
                               (cig::mouse_x + cig::camera_offset_x) - gl_player->R_Pos_X() - gl_player->R_Hitbox()->h/2);

   /* if ( cig::mouse_left ) {
      pos_x += 25*cos(mouse_angle);
      pos_y += 25*sin(mouse_angle);
    }
   */
    if ( head_node )
      head_node->Refresh(cos(mouse_angle)*5, sin(mouse_angle)*5,0,0);

    if ( pos_y > map_height )
      Game_Over();
  }

  const float Player::Max_vel  = 8.0f;
  const int Player::Max_health = 100;

  // MISC
  Jump_Pad::Jump_Pad(cig::Sprite* img, int l, int x, int y) :
                    Object(img, cig::Create_Hitbox(6,1,11,31), l, x, y) { }
  void Jump_Pad::Refresh() {
    for ( auto i : enemies ) {
      if ( Collision(i) )
        Add_Vel(i, 0, -18.f);
    }
    if ( Collision(gl_player) ) {
      gl_player->Set_Vel_Y(-16.f);
    }
  }

  Next_Level::Next_Level(cig::Sprite* img, int l, int x, int y)
    : Object(img, l, x, y) { }
  void Next_Level::Refresh() {
    if ( Collision(gl_player) ) {
      ++Game::current_level;
      Goto_Next_Level(Game::Stages::Playing);
    }
  }

  Death_Block::Death_Block(cig::Sprite* img,int l, int x, int y)
    : Object(img, cig::Create_Hitbox(-1,-1,34,34),l, x, y) { }

  void Death_Block::Refresh() {
    if ( Collision(gl_player) ) {
      gl_player->Add_Health(-100);
    }
    for ( auto i : enemies )
      if ( Collision(i) )
        static_cast<AI*>(i)->Add_Health(-500);
  }


  Key::Key(cig::Sprite* img, int l, int x, int y) :
    AI(img,cig::Create_Hitbox(0, 0, 16, 16),l,5.0f, 4.0f,x,y)
  { o_x = x; o_y = y; }

  void Key::Refresh() {
    vel_y += 0.3f;
    vel_x *= 0.2f;
    // hit a keyhole?
    float p_pos = pos_y;
    pos_y += vel_y;
    pos_x += vel_x;
    for ( int i = 0; i != keyholes.size(); ++ i )
      if ( Collision(keyholes[i]) ) {
        auto t = keyholes[i]->R_Tile_To_Destroy();
        int p_x = t->R_Pos_X()/32;
        int p_y = t->R_Pos_Y()/32;
        delete keyholes[i]->R_Tile_To_Destroy();
        map[p_y][p_x] = nullptr;
        Rem_Health_Bar(healthbar);
        Rem_Refresh(this);
        cig::Rem_Obj(keyholes[i]);
        keyholes.erase(keyholes.begin() + i, keyholes.begin() + i + 1);
        cig::Rem_Obj(this);
        return;
      }
    pos_y = p_pos;
    pos_x -= vel_x;
    // apply force from player cigs
    for ( int i = 0; i != player_cigs.size(); ++ i )
      if ( Collision(player_cigs[i]) ) {
        float angle = R_Angle(player_cigs[i], this);
        vel_x += -cos(angle) * 4.0f;
        if ( vel_y < 0 ) vel_y *= -1;
        vel_y += -sin(angle) * 3.0f;
      }
    AI::Refresh();
    if ( pos_y > map_height ) {
      pos_x = o_x;
      pos_y = o_y;
    }
  }

  Keyhole::Keyhole(cig::Sprite* img, Tile* t_dest, int l, int x, int y) :
    Object(img, l, x, y) { to_destroy = t_dest; }

  // CIG NODE
  const float Cig_Node::Max_vel = 16.0f,
              Cig_Node::Burnoff = .22f;

  Cig_Node::Cig_Node(cig::Sprite* img, cig::Hitbox hit,
      int l, int x, int y) : Object(img, hit, l, x, y) {
    parent = nullptr;
  }
  Cig_Node::~Cig_Node() {
  }

  void Cig_Node::Rem_Child() {
    if ( child ) {
      static_cast<Cig_Node*>(child)->Rem_Child();
      cig::Rem_Obj(child);
    }    
  }

  void Cig_Node::Set_Child_node(Object* node) {
    child = node;
  }
  void Cig_Node::Set_Parent_Node(Object* par, bool is_h) {
    parent = par;
    is_head = is_h;
  }
  bool Cig_Node::R_Is_Head() const { return is_head; }
  cig::Object* Cig_Node::R_Parent() const { return parent; }
  Cig_Node* Cig_Node::R_Child_Node() { 
    return static_cast<Cig_Node*>(child);
  }
  void Cig_Node::Push(Cig_Node* p) {
    if ( p == nullptr ) return;
    if (p->R_Vel_X() < .5f && p->R_Vel_X() > -.5f &&
        p->R_Vel_Y() < .5f && p->R_Vel_Y() > -.5f ) return;
    if ( p->R_Parent() ) {
      p->R_Parent()->Set_Vel_Y(p->R_Parent()->R_Vel_Y() + p->R_Vel_Y()*.85);
      if ( p->R_Parent() == Game::gl_player ) {
        float t = p->R_Parent()->R_Vel_X()>0?1.f:-1.f;
        t *= abs(p->R_Vel_X()*.85f);
        t += p->R_Parent()->R_Vel_X();
        p->R_Parent()->Set_Vel_X(t);
      } else {
        p->R_Parent()->Set_Vel_X(p->R_Parent()->R_Vel_X() + p->R_Vel_X()*.85);
      }  
    }

    if ( !p->is_head )
      Push(static_cast<Cig_Node*>(p->R_Parent()));
  }
  void Cig_Node::Refresh(float vx, float vy, float off_x, float off_y, bool refresh_child) {
    if ( vel_y >  4 ) vel_y =  4;
    if ( vel_y < -4 ) vel_y = -4;

    if ( vel_x >  4 ) vel_x =  4;
    if ( vel_x < -4 ) vel_x = -4;

    // push from parent
    if ( parent ) {
      float mx = parent->R_Pos_X() - pos_x - hitbox.w/2 + parent->R_Hitbox()->w/2 +
                                              off_x + + cos(std::atan2f(vy,vx))*7,
            my = parent->R_Pos_Y() - pos_y - hitbox.h/2 + parent->R_Hitbox()->h/2 +
                                              off_y + sin(std::atan2f(vy,vx))*7;
      if ( mx >  13 ) mx =  13;
      if ( mx < -13 ) mx = -13;
      if ( my >  13 ) my =  13;
      if ( my < -13 ) my = -13;

      pos_x += mx + vel_x;
      pos_y += my + vel_y;
    }
    // also goto child
    if ( child ) {
      pos_x += (child->R_Pos_X() - pos_x) * .01f;
      pos_y += (child->R_Pos_Y() - pos_y) * .01f;
    }
    vel_x *= .2f; vel_x *= .2f;

    if ( vel_x < .5f ) vel_x = 0.f;
    if ( vel_y < .5f ) vel_y = 0.f;

    // iterate through tiles
      for ( int x  = 0; x  != 2; ++  x ) 
      for ( int y  = 0; y  != 2; ++  y )
      for ( int ix = 0; ix != 2; ++ ix )
      for ( int iy = 0; iy != 2; ++ iy ) {
        int t_x = (pos_x + (x>0?hitbox.w:0) + (ix>0?2:-2))/32,
            t_y = (pos_y + (y>0?hitbox.h:0) + (iy>0?2:-2))/32;
        if ( t_x < 0 || t_y < 0 || t_y >= map.size() || t_x >= map[t_y].size() ) continue;
        Object* obj = map[t_y][t_x];
      if ( obj != nullptr && Collision(obj) ) {
        // angle should be based upon parent's position, force on our distance
        float angle = R_Angle(obj, parent?parent:this),
              dist  = R_Distance(obj, this)/4;
        float force_y = -sin(angle) * dist,
              force_x = -cos(angle) * dist;
        if ( force_x >  6.0f ) force_x =  6.0f;
        if ( force_x < -6.0f ) force_x = -6.0f;

        if ( force_y >  6.0f ) force_y =  6.0f;
        if ( force_y < -6.0f ) force_y = -6.0f;
        vel_x = force_x;
        vel_y = force_y;
        if ( parent ) {
          if ( !is_head )
            Push(static_cast<Cig_Node*>(parent));
        }
      }
    }

    Cig_Node* t;
    if ( refresh_child && child )
      static_cast<Cig_Node*>(child)->Refresh(vx*1.0f, vy*1.0f, 0, 0);
  }

  // CIG NODE ENEMY
  Enemy_Cig_Node::Enemy_Cig_Node(cig::Sprite* img, cig::Hitbox hit, int l, Object* own, int x, int y, int p_x, int p_y):
                      Cig_Node(img, hit, l, x, y) {
    e_force = 1.0f;
    Set_Parent_Node(nullptr, 0);
    Set_Child_node(nullptr);
    owner = own;
    off_x = p_x; off_y = p_y;
  }
  Enemy_Cig_Node::~Enemy_Cig_Node() {
    if ( R_Child_Node() )
      cig::Rem_Obj(R_Child_Node());
  }

  void Enemy_Cig_Node::Refresh(float vx, float vy, float e_force) {
    Cig_Node::Refresh(vx*e_force, vy*e_force, off_x, off_y, 0);
    if ( Collision(gl_player) ) {
      Add_Vel(gl_player, cos(R_Angle(gl_player, owner)) * e_force,
                         sin(R_Angle(gl_player, owner)) * e_force);
      gl_player->Add_Health(-1 + -e_force/10);
      gl_player->Add_Health(-10);
      vel_x = -cos(R_Angle(gl_player, owner)) * e_force;
      vel_y = -sin(R_Angle(gl_player, owner)) * e_force;
    }
    if ( R_Child_Node() )
      static_cast<Enemy_Cig_Node*>(R_Child_Node())->Refresh(vx*.8,vy*.8, 13.f);
  }


  // CIG SPINNER
  Cig_Spinner::Cig_Spinner(cig::Sprite* img, cig::Hitbox hit, int l, int ch_am, int x, int y) :
                      Object(img,hit,l,x,y) {
    for ( int i = 0; i != 3; ++ i ) {
      std::get<0>(static_props[i]) = new cig::Object(pos_x + cos(90.f*i*_Pi/180.0f) * 64,
                                                    pos_y + sin(90.f*i*_Pi/180.0f) * 64  );
      Object* t_ptr = std::get<0>(static_props[i]), *r_ptr = nullptr;
      for ( int o = 0; o != ch_am; ++ o ) {
        r_ptr = new Enemy_Cig_Node(spr_enemy_cig_node,cig::Create_Hitbox(2,2,6,6), 7, std::get<0>(static_props[i]),
                      t_ptr->R_Pos_X(), t_ptr->R_Pos_Y());
        if ( o == 0 )
          std::get<1>(static_props[i]) = static_cast<Enemy_Cig_Node*>(r_ptr);
        static_cast<Enemy_Cig_Node*>(r_ptr)->Set_Parent_Node(t_ptr, o == 0);
        cig::Add_Obj(r_ptr);
        
        static_cast<Enemy_Cig_Node*>(t_ptr)->Set_Child_node( r_ptr );
        t_ptr = r_ptr;
      }
    }
  }
  void Cig_Spinner::Refresh() {
    for ( int i = 0; i != 3; ++ i ) {
      float nx = cos(45 + 90*i + float(current_frame*4.f)*_Pi/180.f),
            ny = sin(45 + 90*i + float(current_frame*4.f)*_Pi/180.f);

      std::get<0>(static_props[i])->Set_Pos_X(pos_x + 32 + nx * 32);
      std::get<0>(static_props[i])->Set_Pos_Y(pos_y + 32 + ny* 32);

      if ( std::get<1>(static_props[i]) != nullptr )
        std::get<1>(static_props[i])->Refresh(nx * 5, ny * 5, 13.f);
    }
  }

  // AI
  AI::AI(cig::Sprite* img, cig::Hitbox h, int l, float M_x, float M_y, int x, int y) :
      cig::Object(img, h, l, x, y), Max_x(M_x), Max_y(M_y) {
    /*healthbar = new Game::Health_Bar();
    healthbar->pos_x = x;
    healthbar->pos_y = y - 8;
    healthbar->spr = spr_health_bar;
    healthbar->Set_Owner(this);
    cig::Add_Spr(healthbar, cig::Spr_Layer::Pre_Foreground);*/
  }

  void AI::Add_Health(int x) { health += x; }

  AI::AI(cig::Sprite* img, int l, float M_x, float M_y, int x, int y) :
      cig::Object(img, l, x, y), Max_x(M_x), Max_y(M_y) {
    healthbar = new Game::Health_Bar();
    healthbar->pos_x = x;
    healthbar->pos_y = y - 8;
    healthbar->spr = spr_health_bar;
    healthbar->Set_Owner(this);
    cig::Add_Spr(healthbar, cig::Spr_Layer::Pre_Foreground);
  }

  AI::~AI() {
    int o = static_cast<int>(cig::Spr_Layer::Pre_Foreground);
    for ( int i = 0; i != cig::sprs[o].size(); ++ i ) {
      if ( cig::sprs[o][i] == healthbar ) {
        cig::sprs[o].erase(cig::sprs[o].begin() + i,
                           cig::sprs[o].begin() + i + 1);
        delete healthbar;
      }
    }
    cig::Add_Obj(new cig::Object(spr_explosion, cig::Create_Hitbox(0,0,0,0), 3, pos_x, pos_y));
  }

  void AI::Refresh() {

    // speed limit
    if ( vel_x > Max_x ) vel_x = Max_x;
    if ( vel_x < -Max_x ) vel_x = -Max_x;

    if ( vel_y >  Max_y ) vel_y =  Max_y;
    if ( vel_y < -Max_y ) vel_y = -Max_y;
    // move object

    // assume no collision at this point
    pos_x += vel_x;
    { // check for collision
      // could do some serious optimization later on...
      for ( int x  = 0; x  != 2; ++  x ) 
      for ( int y  = 0; y  != 2; ++  y )
      for ( int ix = 0; ix != 2; ++ ix )
      for ( int iy = 0; iy != 2; ++ iy ) {
        int t_x = (pos_x + (x>0?hitbox.w:0) + (ix>0?2:-2))/32,
            t_y = (pos_y + (y>0?hitbox.h:0) + (iy>0?2:-2))/32;
        if ( t_x < 0 || t_y < 0 || t_y >= map.size() || t_x >= map[t_y].size() ) continue;
        Object* obj = map[t_y][t_x];
        if ( obj != nullptr && Collision(obj) ) {
          if ( vel_x > 0 )
            pos_x = obj->R_Pos_X() - hitbox.w - hitbox.x - 1;
          else
            pos_x = obj->R_Pos_X() + obj->R_Hitbox()->w - hitbox.x + 1;
          vel_x *= -1;
        }
      }
    }

    // check y
    pos_y += vel_y;
    { // check for collision
      // could do some serious optimization later on...
      for ( int x  = 0; x  != 2; ++  x ) 
      for ( int y  = 0; y  != 2; ++  y )
      for ( int ix = 0; ix != 2; ++ ix )
      for ( int iy = 0; iy != 2; ++ iy ) {
        int t_x = (pos_x + (x>0?hitbox.w:0) + (ix>0?2:-2))/32,
            t_y = (pos_y + (y>0?hitbox.h:0) + (iy>0?2:-2))/32;
        if ( t_x < 0 || t_y < 0 || t_y >= map.size() || t_x >= map[t_y].size() ) continue;
        Object* obj = map[t_y][t_x];
        if ( obj != nullptr && Collision(obj) ) {
          if ( vel_y >= 0 )
            pos_y = obj->R_Pos_Y() - hitbox.h - hitbox.y - 1;
          else
            pos_y = obj->R_Pos_Y() + obj->R_Hitbox()->h - hitbox.y + 1;
          vel_y = 0;
        }
      }
    }
    // move healthbar
    //healthbar->pos_x = pos_x - cig::camera_offset_x;
    //healthbar->pos_y = pos_y - cig::camera_offset_y;
  }

  // TURTLE HEART
  Turtle_Heart::Turtle_Heart(cig::Sprite* img, int l, int x, int y) :
                  Object(img, cig::Create_Hitbox(2,1,11,11), x, y) {}
  bool Turtle_Heart::t_collision(cig::Object* obj) {
    return Collision(obj);
  }

  // TURTLE
  const int Turtle::Max_health = 25000;
  Turtle::Turtle(cig::Sprite* img, int l, int ch_am, int x, int y) :
            AI(img, cig::Create_Hitbox(5,9,20,23), 4.0f, 4.0f, x, y)
  {
    heart = new Turtle_Heart(spr_turtle_heart, 8, 0, 0);
    cig::Add_Obj(heart);
    for ( int i = 0; i != 2; ++ i )
      top[i] = Create_Cig_Chain(this, ch_am, pos_x, pos_y);
    pos_x = x; pos_y = y;
    vel_x = 4.0f;
    health = Max_health;
    top[0]->Set_Pos_X(pos_x);
    top[1]->Set_Pos_Y(pos_y);
  }
  void Turtle::Refresh() {
    if ( health < 0 ) {
      top[0]->Rem_Child();
      top[1]->Rem_Child();
      cig::Rem_Obj(heart);
      cig::Rem_Obj(top[0]);
      cig::Rem_Obj(top[1]);
      for ( int i = 0; i != enemies.size(); ++ i )
        if ( enemies[i] == this ) {
          enemies.erase(enemies.begin() + i, enemies.begin() + i + 1);
          break;
        }
      cig::Rem_Obj(this);
      return;
    }

   top[0]->Refresh(cos((90+45)*_Pi/180) * 0.01, sin((90+45)*_Pi/180)*0.01, 3.0f);
   //top[1]->Refresh(cos((90-45)*_Pi/180) * 0.01, sin((90-45)*_Pi/180)*0.01, 3.0f);

   if ( vel_x == 0.f ) vel_x = 0.3f;
   if ( vel_x < Max_x && vel_x > Max_x ) vel_x *= 1.15f;

   // gravity
   vel_y += 1;

   // now everything is modified, refresh before setting heart
   AI::Refresh();
    
   heart->Set_Pos_X(pos_x + 24);
   heart->Set_Pos_Y(pos_y +  1);

   // check turtle heart
   for ( int i = 0; i != player_cigs.size(); ++ i )
     if ( heart->t_collision(player_cigs[i]) ) {
       health -= 5*gl_player->R_Node_Amount();
       float angle = R_Angle(player_cigs[i], heart);
       vel_x += -cos(angle) * 7.0f;
       vel_y += -sin(angle) * 7.0f;
     }
  }
    /*


    */
  //}
  Turtle::~Turtle() { }
  // HIPSTER
  const int Hipster::Max_health = 50;

  Hipster::Hipster(cig::Sprite* img, int l, int ch_am, int x, int y)
          : AI(img, cig::Create_Hitbox(3, 0, 20, 38), l, 9.5f, 18.0f, x, y) {
    top = Create_Cig_Chain(this, ch_am, 0, -9);
    health = Max_health;
    vel_x = 3.0f;
  }
  void Hipster::Refresh() {
    if ( health < 0 ) {
      top->Rem_Child();
      cig::Rem_Obj(top);
      for ( int i = 0; i != enemies.size(); ++ i )
        if ( enemies[i] == this ) {
          enemies.erase(enemies.begin() + i, enemies.begin() + i + 1);
          break;
        }
      cig::Rem_Obj(this);
      return;
    }


    top->Refresh(cos((18)*_Pi/180) * 5, sin((90)*_Pi/180) * 25, 8.0f);

    if ( vel_x < Max_x && vel_x > Max_x ) vel_x *= 1.15f;

    // gravity
    vel_y += 0.5f;
    AI::Refresh();

    if ( !(pos_x - cig::camera_offset_x > map_width  || pos_x - cig::camera_offset_x < 0 ||
           pos_y - cig::camera_offset_y > map_height || pos_y - cig::camera_offset_y < 0 ) )
    {
      for ( int i = 0; i != player_cigs.size(); ++ i )
        if ( Collision(player_cigs[i]) ) {
          health -= 5*gl_player->R_Node_Amount();;
          float angle = R_Angle(player_cigs[i], this);
          vel_x += -cos(angle) * 5.0f;
          vel_y += -sin(angle) * 5.0f;
        }
    }
  }
  Hipster::~Hipster() {
    
  }

  const int MJ::Max_health = 250;

  // MJ
  MJ::MJ(cig::Sprite* img, int l, int dx, int dy)
        : AI(img, cig::Create_Hitbox(4, 2, 21, 30), l, 8.0f, 16.0f, dx, dy) {
    health = Max_health;
    player_timer = 0;
  }
  void MJ::Refresh() {
    if ( health < 0 ) {
      for ( int i = 0; i != enemies.size(); ++ i )
        if ( enemies[i] == this ) {
          enemies.erase(enemies.begin() + i, enemies.begin() + i + 1);
          break;
        }
      cig::Rem_Obj(this);
      return;
    }

    if ( vel_x == 0 ) vel_x = 0.2f;
    if ( vel_x < Max_x && vel_x > -Max_x ) vel_x *= 1.15f;
    // gravity
    vel_y += 0.5f;
    AI::Refresh();
    if ( player_timer > 0 ) --player_timer;
    if ( Collision(gl_player) && player_timer < 1 ) {
      player_timer = 60;
      gl_player->Add_Health(-25);
      float angle = R_Angle(gl_player, this);
      Add_Vel(gl_player, -cos(angle) * 32.0f, -sin(angle) * 16.0f);
    }
    for ( int i = 0; i != player_cigs.size(); ++ i )
      if ( Collision(player_cigs[i]) ) {
        health -= 5*gl_player->R_Node_Amount();;
        float angle = R_Angle(player_cigs[i], this);
        vel_x += -cos(angle) * 4.0f;
        vel_y += -sin(angle) * 4.0f;
      }
  }

  // UTILITY
  inline float R_Angle(cig::Object* x, cig::Object* y) {
    if ( x == nullptr || y == nullptr ) return 0.0f;

    return std::atan2f((x->R_Pos_Y() + x->R_Hitbox()->h/2) -
                       (y->R_Pos_Y() + y->R_Hitbox()->h/2),
                       (x->R_Pos_X() + x->R_Hitbox()->w/2) -
                       (y->R_Pos_X() + y->R_Hitbox()->w/2));

  }
  inline float R_Distance(cig::Object* x, cig::Object* y) {
    if ( x == nullptr || y == nullptr ) return 0.0f;

    return std::sqrt(pow((x->R_Pos_Y() + x->R_Hitbox()->h/2) -
                         (y->R_Pos_Y() + y->R_Hitbox()->h/2),2) +
                     pow((x->R_Pos_X() + x->R_Hitbox()->w/2) -
                         (y->R_Pos_X() + y->R_Hitbox()->w/2),2));
  }
  inline void Add_Vel(cig::Object* obj, float x, float y) {
    obj->Set_Vel_X(obj->R_Vel_X() + x);
    obj->Set_Vel_Y(obj->R_Vel_Y() + y); 
  }
  Enemy_Cig_Node* Create_Cig_Chain(cig::Object* head, int count, int p_x, int p_y) {
    Enemy_Cig_Node* t_head = nullptr, *r = nullptr, *t = nullptr;
    while ( --count > -1 ) {
      t = static_cast<Enemy_Cig_Node*>(cig::Add_Obj(new Enemy_Cig_Node(spr_enemy_cig_node, cig::Create_Hitbox(2,2,6,6), 10, head,
                                                          head->R_Pos_X() + head->R_Hitbox()->w/2,
                                                          head->R_Pos_Y() + head->R_Hitbox()->h/2, p_x, p_y)));
      if ( t_head == nullptr ) {
        t_head = t;
        t->Set_Parent_Node(head, true);
      } else
        t->Set_Parent_Node(r);
      if ( r )
        r->Set_Child_node(t);
      r = t;
    }
    return t_head;
  }


  void Rem_Health_Bar(Health_Bar* h) {
    cig::Spr_Layer sh = cig::Spr_Layer::Pre_Foreground;
    for ( int i = 0; i != cig::sprs[sh].size(); ++ i ) {
      if ( cig::sprs[sh][i] == h ) {
        cig::sprs[sh].erase(cig::sprs[sh].begin() + i, cig::sprs[sh].begin() + i + 1);
        return;
      }
    }
  }

  void Rem_Refresh(cig::Object* o) {
    for ( int i = 0 ; i != call_refresh_objs.size(); ++ i )
      if ( call_refresh_objs[i] == o ) {
        call_refresh_objs.erase(call_refresh_objs.begin() + i,
                                call_refresh_objs.begin() + i + 1);
        return;
      }
  }


  void Game_Over() {
    // just temporary
    Goto_Next_Level(Game::Stages::Dead);
  }

  // UPGRADES
  Node_Upgrade::Node_Upgrade(cig::Animation* img, int l, int x, int y) : Object(img,l,x,y) {}
  void Node_Upgrade::Refresh() {
    if ( Collision(gl_player) ) {
      gl_player->Add_Node();
      for ( int i = 0; i != call_refresh_objs.size(); ++ i )
        if ( call_refresh_objs[i] == this ) {
          call_refresh_objs.erase(call_refresh_objs.begin() + i,
                                  call_refresh_objs.begin() + i + 1);
          break;
        }
      cig::Rem_Obj(this);
    }
  }

  // GRAPHICS
  SDL_Rect Health_Bar::R_Rect() {
    return cig::Create_Hitbox(0,0,float(owner->R_Health())/float(owner->R_Max_Health()) *
                                    float(spr->R_Width()), spr->R_Height());
  }
};