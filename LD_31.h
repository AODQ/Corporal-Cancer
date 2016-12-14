#ifndef __LD_31_H_
#define __LD_31_H_

#include "Cigarette_Butt.h"
#include <fstream>

namespace Game {

  class Tile : public cig::Object {
  public:
    Tile(cig::Sprite* img, int l, int x = 0, int y = 0);
  };

  class Keyhole;

  extern std::vector<std::vector<Tile*>> map;
  extern std::vector<cig::Object*> enemies;
  extern std::vector<Keyhole*> keyholes;
  extern std::vector<std::string> map_names, map_text_names;
  extern int current_level;

  enum Stages { Playing, Menu, Dead };

  extern int current_stage;

  class Cig_Node;

  extern std::vector<Cig_Node*> player_cigs;
  // IMAGES
  extern cig::Sprite* spr_cig_node, *spr_turtle, *spr_hipster, *spr_MJ, *spr_enemy_cig_node,
                     *spr_jump_pad, *spr_health_bar, *spr_key, *spr_keyhole, *spr_next_level,
                     *spr_death, * spr_GUI, * spr_hp_outline, *spr_pl_health, * spr_shit_menu,
                     *spr_deathscreen, * spr_turtle_heart, * spr_tile, *spr_enemy_turtle,
                     *spr_enemy_hipster, *spr_player_img;
  extern cig::Animation* spr_upgrade_node, *anim_enemy_spinner, *spr_enemy_mj;
  extern std::vector<cig::Object*> call_refresh_objs;
  extern int map_height, map_width;
  // GUI
  class Health_Bar : public cig::GUI {
    cig::Object* owner;
  public:
    void Set_Owner(cig::Object* own) { owner = own; }
    cig::Object* R_Owner() { return owner; }
    SDL_Rect R_Rect();
  };
  // PLAYER
  class Player;
  extern Player* gl_player;

  void Load_Map(const char* fil_name);
  void Load_Dialogue(const char* fil_name);
  void Clear_Map();

  class Cig_Node;

  class Player : public cig::Object {
    static const float Max_vel;
    static const int Max_health;
    int health;
    Cig_Node* head_node;
    int node_amount;
  public:
    Player(cig::Sprite* img, cig::Hitbox hit, int l, int x = 0, int y = 0);
    int R_Node_Amount() { return node_amount; }
    int R_Health();
    int R_Max_Health();
    void Add_Health(int);
    void Refresh();
    void Add_Node();
  };
  
  // NODES
  class Cig_Node : public cig::Object {
    static const float Max_vel, Burnoff;
    Object* child;
    Object* parent; // either cig node, player, etc
    bool is_head;
  public:
    Cig_Node(cig::Sprite* img, cig::Hitbox hit, int l, int x = 0, int y = 0);
    ~Cig_Node();
    void Set_Child_node(Object*);
    void Set_Parent_Node(Object*, bool is_head = 0);

    bool R_Is_Head() const;

    void Rem_Child();

    Cig_Node* R_Child_Node();
    Object* R_Parent() const;

    virtual void Refresh(float vx, float vy, float off_x, float off_y, bool refresh_child = 1);
    static void Push(Cig_Node*);
  };

  class Enemy_Cig_Node : public Cig_Node {
    float e_force;
    float off_x, off_y;
    Object* owner; // parent's parent's ... parent
  public:
    Enemy_Cig_Node(cig::Sprite* img, cig::Hitbox hit, int l, Object* ownr, int x = 0, int y = 0, int p_x = 0, int p_y = 0);
    ~Enemy_Cig_Node();
    void Refresh(float vel_x, float vel_y, float e_force);
  };

  // misc.
  class Jump_Pad : public cig::Object {
  public:
    Jump_Pad(cig::Sprite* img, int l, int x, int y);
    void Refresh();
  };

  class Next_Level : public cig::Object {
  public:
    Next_Level(cig::Sprite* img, int l, int x, int y);
    void Refresh();
  };

  // enemies
  class Cig_Spinner : public cig::Object {
    std::pair<Object*, Enemy_Cig_Node*> static_props[4];
  public:
    Cig_Spinner(cig::Sprite* img, cig::Hitbox hit, int l, int ch_am, int x = 0, int y = 0);

    void Refresh();
  };

  // only used as an extra hitbox
  class Turtle_Heart : public cig::Object {
  public:
    Turtle_Heart(cig::Sprite* img, int l, int x, int y);
    bool t_collision(cig::Object* obj);
  };
  class Health_Bar;
  class AI : public cig::Object {
  protected:
    int health;
    const float Max_x, Max_y;
    Game::Health_Bar* healthbar;
  public:
    AI(cig::Sprite* img, cig::Hitbox hit, int l, float m_v_x, float m_v_y, int x = 0, int y = 0);
    AI(cig::Sprite* img, int l, float m_v_x, float m_v_y, int x = 0, int y = 0);

    ~AI();

    void Add_Health(int x); //{ health += x);

    // call this in your own refresh after you modify vel_x/y. Does hitbox detection and limits speed
    virtual void Refresh();
    virtual int R_Max_Health() = 0;
  };
  
  class Turtle : public AI {
    Enemy_Cig_Node* top[2];
    Turtle_Heart* heart;
    static const int Max_health;
  public:
    Turtle(cig::Sprite* img, int l, int ch_am, int x = 0, int y = 0);
    ~Turtle();

    void Refresh();
    int R_Max_Health() { return Max_health; }
  };

  class Death_Block : public cig::Object {
  public:
    Death_Block(cig::Sprite* img, int l, int x = 0, int y = 0);

    void Refresh();
  };

  class Hipster : public AI {
    Enemy_Cig_Node* top;
    static const int Max_health;
  public:
    Hipster(cig::Sprite* img, int l, int ch_am, int x = 0, int y = 0);
    ~Hipster();

    void Refresh();
    int R_Max_Health() { return Max_health; }
  };

  class MJ : public AI {
    static const int Max_health;
    int player_timer;
  public:
    MJ(cig::Sprite* img, int l, int x = 0, int y = 0);

    void Refresh();
    int R_Max_Health() { return Max_health; }
  };

  
  class Key : public AI {
    int o_x, o_y; // in case falls off map
  public :
    Key(cig::Sprite* img, int l, int x = 0, int y = 0);

    void Refresh();
    int R_Max_Health() { return -1; }
  };

  class Keyhole : public cig::Object {
    Tile* to_destroy;
  public:
    Keyhole(cig::Sprite* img, Tile* t_dest, int l, int x = 0, int y = 0);
    Tile* R_Tile_To_Destroy() { return to_destroy; }
  };

  // UPGRADES
  class Node_Upgrade : public cig::Object {
  public:
    Node_Upgrade(cig::Animation* img, int l, int x = 0, int y = 0);

    void Refresh();
  };

  // utility
  inline float R_Angle(cig::Object*, cig::Object*);
  inline float R_Distance(cig::Object*, cig::Object*);
  inline void  Add_Vel(cig::Object*, float, float);
  void Rem_Health_Bar(Health_Bar*);
  void Rem_Refresh(cig::Object*);

  void Game_Over();
  void Goto_Next_Level(Stages stage);

  // p_x and p_y denote the point at which the cig chain originates (the offset from head->x + head->w/2, etc)
  Enemy_Cig_Node* Create_Cig_Chain(cig::Object* head, int count, int p_x, int p_y);
};

#endif