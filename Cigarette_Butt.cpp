#include <SDL.h>
#include <SDL_image.h>
#include <SDL_TTF.h>
#include <SDL_mixer.h>
#include <fstream>
#include <vector>
#include "Cigarette_Butt.h"
#include "LD_31.h"

namespace cig {

  // SPRITE
  Sprite::Sprite() { }

  Sprite::Sprite(const char* img_name) {
    Load_Surface(img_name);
  }

  Sprite::~Sprite() {
    SDL_FreeSurface(surface);
  }

  int Sprite::R_Width() const  { return surface->w; }
  int Sprite::R_Height() const { return surface->h; }

  SDL_Rect* Sprite::R_Frame() { return nullptr; }
  SDL_Rect* Sprite::R_Frame(int i) { return nullptr; }
  SDL_Surface* Sprite::R_Surface() { return surface; }

  void Sprite::Load_Surface(const char* img_name) {
    auto i = SDL_RWFromFile(img_name, "rb");
    if ( i == nullptr ) {
      debug << "Error in RWFromFile from Sprite::Load_Surface: " << IMG_GetError();
      Clean_Up();
      exit(0);
    }
    surface = IMG_LoadPNG_RW(i);
    if ( surface == nullptr ) {
      debug << "Error in IMG_LOadPNG_RW from Sprite::Load_Surface: " << IMG_GetError();
      Clean_Up();
      exit(0);
    }
  }

  // ANIMATION

  Animation::Animation(const char* img_name, int a_width, int a_height, int a_speed) {
    anim_width = a_width; anim_height = a_height; anim_speed = a_speed;
    Load_Surface(img_name);
    if ( a_width <= 0 )
      a_width = surface->w;
    if ( a_height <= 0 )
      a_height = surface->h;
    Refresh_Frames();
  }

  Animation::~Animation() {
    
  }

  int Animation::R_Anim_Width() const { return anim_width; }
  int Animation::R_Anim_Height() const { return anim_height; }
  int Animation::R_Anim_Speed() const { return anim_speed; }
  int Animation::R_Frame_Size() const { return frames.size(); }

  void Animation::Set_Anim_Width(int x) {
    anim_width = x;
    Refresh_Frames();
  }
  void Animation::Set_Anim_Height(int x) {
    anim_height = x;
    Refresh_Frames();
  }
  void Animation::Set_Anim_Speed(int x) {
    anim_speed = x;
  }
  SDL_Rect* Animation::R_Frame(int i) {
    if ( i < frames.size() )
      return &frames[i];
    else
      return nullptr;
  }

  void Animation::Refresh_Frames() {
    // t_width/height is the amount of iterations needed to react limit
    int t_width = surface->w/anim_width, t_height = surface->h/anim_height;
    frames.erase(frames.begin(), frames.end());

    int supposed_size = t_width * t_height;
    
    for ( int h = 0; h < t_height;  ++ h )
    for ( int w = 0; w < t_width; ++ w )
      frames.push_back(Create_Hitbox(w*anim_width, h*anim_height, anim_width, anim_height));
    
    if ( supposed_size != frames.size() ) {
      debug << "In Animation::Refresh_Frames, theoretical size and actual frame size differ: " <<
        "theoretical: " << supposed_size << ", actual: " << frames.size() << '\n';
    }
  }
  // OBJECT

  Hitbox* Object::R_Hitbox() { return &hitbox; }
  float Object::R_Pos_X() const { return pos_x; }
  float Object::R_Pos_Y() const { return pos_y; }
  float Object::R_Vel_X() const { return vel_x; }
  float Object::R_Vel_Y() const { return vel_y; }
  int Object::R_Frame() const { return current_frame; }
  int Object::R_Layer() const { return layer; }

  void Object::Set_Pos_X(int x) { pos_x = x; }
  void Object::Set_Pos_Y(int y) { pos_y = y; }
  void Object::Set_Vel_X(float x) { vel_x = x; }
  void Object::Set_Vel_Y(float y) { vel_y = y; }
  void Object::Set_Frame(int i) { current_frame = i; }

  void Object::Refresh() {
    // this will equal negative one because we set it to this value 
    // in R_Next_Frame is last frame hit and dest_anim_done
    if ( is_animation == 0 ) return;

    if ( current_frame == -1 )
      for ( int i = 0; i != objs[layer].size(); ++ i ) {
        if ( objs[layer][i] == this ) {
          objs[layer].erase(objs[layer].begin() + i,
                    objs[layer].begin() + i + 1);
          delete this;
          return;
        }
      }
    return;
  }

  SDL_Rect* Object::R_Next_Frame(bool iterate_frame) {
    it_frame += iterate_frame;
    if ( it_frame >= sprite->R_Anim_Speed() ) {
      it_frame = 0;
      if ( ++current_frame >= sprite->R_Frame_Size() ) {
        current_frame = 0;
        if ( dest_anim_done ) current_frame = -1;
      }
    }
    return sprite->R_Frame(current_frame);
  }

  bool Object::Mouse_On() {
    int mx  = pos_x + hitbox.x - camera_offset_x,
        my  = pos_y + hitbox.y - camera_offset_y,
        mxm = pos_x + hitbox.x + hitbox.w - camera_offset_x,
        mym = pos_y + hitbox.y + hitbox.h - camera_offset_y;
    return mouse_x > mx  && mouse_x < mxm &&
           mouse_y > mxm && mouse_y < mym;
  }

  bool Object::Collision(Object* o) {
    Hitbox* ohitbox = o->R_Hitbox();
    int mx = pos_x + hitbox.x,
        ox = o->R_Pos_X() + ohitbox->x,
        my = pos_y + hitbox.y,
        oy = o->R_Pos_Y() + ohitbox->y;
    
    return !(ox              > mx + hitbox.w
          || ox + ohitbox->w < mx
          || oy              > my + hitbox.h
          || oy + ohitbox->h < my);
  }
  
  Animation* Object::R_Animation() { return sprite; }

  Object::Object(int x, int y) {
    pos_x = x; pos_y = y; layer = 0;
    vel_x = vel_y = current_frame = it_frame = 0;
    sprite = nullptr;
    is_animation = dest_anim_done = 0;
  }

  Object::Object(Sprite* img, int l, int x, int y) {
    pos_x = x; pos_y = y; layer = l;
    vel_x = vel_y = current_frame = it_frame = 0;
    sprite = static_cast<Animation*>(img);
    hitbox = Create_Hitbox(0,0,sprite->R_Width(),
                               sprite->R_Height());
    is_animation = 0;
  }
  Object::Object(Sprite* img, Hitbox hit, int l, int x, int y) {
    pos_x = x; pos_y = y; layer = l;
    vel_x = vel_y = current_frame = it_frame = 0;
    sprite = static_cast<Animation*>(img);
    hitbox = hit;
    is_animation = 0;
  }
  Object::Object(Animation* img, int l, int x, int y, bool anim_d) {
    pos_x = x; pos_y = y; layer = l;
    vel_x = vel_y = current_frame = it_frame = 0;
    sprite = img;
    hitbox = Create_Hitbox(0,0,sprite->R_Anim_Width(),
                               sprite->R_Anim_Height());
    is_animation = 1;
    dest_anim_done = anim_d;
  }
  Object::Object(Animation* img, Hitbox hit, int l, int x, int y) {
    pos_x = x; pos_y = y; layer = l;
    vel_x = vel_y = current_frame = it_frame = 0;
    sprite = img;
    hitbox = hit;
    is_animation = 1;
    dest_anim_done = 0;
  }

  Object::~Object() {
    
  }

// TEXT
  
  Text::Text(std::string t, Colour c, int x, int y, bool static_) {
    static_pos = static_; text = t; colour = c; pos_x = x; pos_y = y;
    Refresh_Message();
  }
  
  Text::~Text() {
    SDL_FreeSurface(message);
  }

  std::string Text::R_Text() const { return text; }
  int Text::R_Pos_X() const { return pos_x; }
  int Text::R_Pos_Y() const { return pos_y; }
  Colour Text::R_Colour() const { return colour; }
  SDL_Surface* Text::R_Surface() const { return message; }
  bool Text::R_Static() const { return static_pos; }

  void Text::Set_Text(std::string t) {
    Refresh_Message();
    text = t;
  }
  void Text::Set_Colour(Colour i) {
    Refresh_Message();
    colour = i;
  }
  void Text::Set_Pos_X(int i) { pos_x = i; }
  void Text::Set_Pos_Y(int i) { pos_y = i; }

  TTF_Font* Text::font = nullptr;
  void Text::Load_Font(const char* fname, int pt_size) {
    if ( font != nullptr )
      TTF_CloseFont(font);
    font = TTF_OpenFont(fname, pt_size);
    if ( font == nullptr ) {
      debug << "Error at TTF_OpenFont in Text::Load_Font: " << TTF_GetError();
      Clean_Up();
      exit(0);
    }
  }

  void Text::Refresh_Message() {
    SDL_FreeSurface(message);
    message = TTF_RenderText_Solid( font, text.c_str(), colour );
    if ( message == nullptr ) {
      debug << "Error at TTF_RenderText_Solid in Text::Refresh_Message: " << TTF_GetError();
      Clean_Up();
      exit(0);
    }
  }

// MISC TOOLS
  // don't forget to Load_Font on Text
  void Set_Up() {
    SDL_Init( SDL_INIT_EVERYTHING );
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mixer::Set_Up();

    window = SDL_SetVideoMode( 1024, 768, 32, SDL_SWSURFACE );
  }
  void Clean_Up() {
    debug.close();
    for ( int i = 0; i != 4; ++ i )
      for ( int o = 0; o != sprs[i].size(); ++ o )
        delete sprs[i][o];
    for ( int i = 0; i != objs.size(); ++ i )
      for ( int o = 0; o != objs[i].size(); ++ o ) {
        delete objs[i][o];
      }
    for ( int i = 0; i != texts.size(); ++ i )
      delete texts[i];

    SDL_FreeSurface(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
  }

  Object* Add_Obj(Object* obj) {
    while ( objs.size() <= obj->R_Layer() )
      objs.push_back(std::vector<Object*>());
    objs[obj->R_Layer()].push_back(obj);
    return obj;
  }

  void Rem_Obj(Object* obj) {
    for ( int i = 0; i != objs[obj->R_Layer()].size(); ++ i )
      if ( objs[obj->R_Layer()][i] == obj ) {
        objs[obj->R_Layer()].erase(objs[obj->R_Layer()].begin() + i,
                                   objs[obj->R_Layer()].begin() + i + 1);
        delete obj;
        return;
      }
  }

  SDL_Rect GUI::R_Rect() {
    return Create_Hitbox(0,0, spr->R_Width(), spr->R_Height());
  }

  GUI* Add_Spr(GUI* spr, Spr_Layer layer) {
    auto& sprz = sprs;
    sprs[layer].push_back(spr);
    return spr;
  }
  Text* Add_Text(Text* txt) {
    texts.push_back(txt);
    return txt;
  }

  void Refresh_Keys() {
    SDL_PumpEvents();
    keys = SDL_GetKeyState(nullptr);

    auto mouse = SDL_GetMouseState(&mouse_x,&mouse_y);
    mouse_left =  static_cast<bool>(mouse & SDL_BUTTON(SDL_BUTTON_LEFT));
    mouse_right = static_cast<bool>(mouse & SDL_BUTTON(SDL_BUTTON_RIGHT));
    scroll_down = static_cast<bool>(mouse & SDL_BUTTON(SDL_BUTTON_WHEELUP));
    scroll_up   = static_cast<bool>(mouse & SDL_BUTTON(SDL_BUTTON_WHEELDOWN));
  }
  void Refresh_Screen() {
    SDL_FillRect(window, nullptr, 0x000000);

    SDL_Rect off;
    auto& ifz = sprs[0];
    for ( int i = 0; i != sprs[0].size(); ++ i ) {
      off.x = sprs[0][i]->pos_x;
      off.y = sprs[0][i]->pos_y;
      SDL_BlitSurface(sprs[0][i]->spr->R_Surface(), &sprs[0][i]->R_Rect(), window, &off);
    }

    for ( int i = 0; i != sprs[1].size(); ++ i ) {
      off.x = sprs[1][i]->pos_x;
      off.y = sprs[1][i]->pos_y;
      SDL_BlitSurface(sprs[1][i]->spr->R_Surface(), &sprs[1][i]->R_Rect(), window, &off);
    }

    off.x = -camera_offset_x;
    off.y = -camera_offset_y;
    auto& asdf = Game::map;
    for ( int iy = camera_offset_y/32-1; iy != (camera_offset_y+ 768)/32 + 1; ++ iy ) {
      if ( iy < 0 ) continue;
      if ( iy >= Game::map_height/32 || Game::map.size() <= iy ) break;
      for ( int ix = camera_offset_x/32-1; ix != (camera_offset_x+1024)/32 + 1; ++ ix ) {
        if ( ix < 0 ) continue;
        if ( ix >= Game::map_width/32 || Game::map[iy].size() <= ix ) break;
        if ( Game::map[iy][ix] == nullptr ) continue;
        off.x = Game::map[iy][ix]->R_Pos_X() - camera_offset_x;
        off.y = Game::map[iy][ix]->R_Pos_Y() - camera_offset_y;
        off.w = Game::map[iy][ix]->R_Animation()->R_Anim_Width();
        off.h = Game::map[iy][ix]->R_Animation()->R_Anim_Height();
        SDL_BlitSurface(Game::map[iy][ix]->R_Animation()->R_Surface(),
                        Game::map[iy][ix]->R_Next_Frame(1), window, &off);
      }
    }

    /*
        return !(ox              > mx + hitbox.w
          || ox + ohitbox->w < mx
          || oy              > my + hitbox.h
          || oy + ohitbox->h < my);
    */

    auto& t_obj = objs;
    for ( int i = 0; i < objs.size(); ++ i )
      for ( int o = 0; o < objs[i].size(); ++ o ) {
        //if (  (objs[i][o]->R_Pos_X() < camera_offset_x/32 || objs[i][o]->R_Pos_X() > (camera_offset_x+1024)/32 ||
        //       objs[i][o]->R_Pos_Y() < camera_offset_x/32 || objs[i][o]->R_Pos_Y() > (camera_offset_y+ 768)/32) )
        //  continue;
        if ( objs[i][o]->R_Animation()->R_Surface() != nullptr ) {
          off.x = objs[i][o]->R_Pos_X() - camera_offset_x;
          off.y = objs[i][o]->R_Pos_Y() - camera_offset_y;
          SDL_BlitSurface(objs[i][o]->R_Animation()->R_Surface(),objs[i][o]->R_Next_Frame(1), window, &off);
        }
      }
    auto& t = texts;
    for ( int i = 0; i != texts.size(); ++ i ) {
      off.x = texts[i]->R_Pos_X();
      off.y = texts[i]->R_Pos_Y();
      if ( !texts[i]->R_Static() ) {
        off.x -= camera_offset_x;
        off.y -= camera_offset_y;
      }
      off.w = texts[i]->R_Surface()->w;
      off.h = texts[i]->R_Surface()->h;
      SDL_BlitSurface(texts[i]->R_Surface(), nullptr, window, &off);
    }

    for ( int i = 0; i != sprs[2].size(); ++ i ) {
      off.x = sprs[2][i]->pos_x;
      off.y = sprs[2][i]->pos_y;
      SDL_BlitSurface(sprs[2][i]->spr->R_Surface(), &sprs[2][i]->R_Rect(), window, &off);
    }

    for ( int i = 0; i != sprs[3].size(); ++ i ) {
      off.x = sprs[3][i]->pos_x;
      off.y = sprs[3][i]->pos_y;
      SDL_BlitSurface(sprs[3][i]->spr->R_Surface(), &sprs[3][i]->R_Rect(), window, &off);
    }

    SDL_Flip( window );
  }

  Hitbox Create_Hitbox(int x, int y, int w, int h) {
    Hitbox hit;
    hit.x = x; hit.y = y; hit.w = w; hit.h = h;
    return hit;
  }
  Colour Create_Colour(int r, int g, int b) {
    Colour col;
    col.b = b;
    col.g = g;
    col.r = r;
    return col;
  }


  namespace Mixer {
    const int Max_channels = 255;
    int channel_playing[255];

    std::vector<Mix_Chunk*> sounds;
    std::vector<Mix_Music*> music;

    void Set_Up() {
      Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
      Mix_Init(MIX_INIT_MP3);
      Mix_AllocateChannels(Max_channels);
      Mix_ChannelFinished(Pause_Channel);
    }

    int Load_Sound(const char* fname) {
      Mix_Chunk* x = Mix_LoadWAV(fname);
      if ( x == nullptr ) {
        debug << "Error at Mix_LoadWAV in Load_Sound, argument " << fname << ", "
              << Mix_GetError();
      }
      sounds.push_back(x);
      return sounds.size()-1;
    }

    int Load_Song(const char* fname) {
      Mix_Music* x = Mix_LoadMUS(fname);
      if ( x == nullptr ) {
        debug << "Error at Mix_LoadWAV in Load_Song, argument " << fname << ", "
              << Mix_GetError();
        Clean_Up();
        exit(0);
      }
      music.push_back(x);
      return music.size()-1;
    }

    int Play_Sound(int i) {
      int a = Mix_PlayChannel(-1,sounds[i],0);
      channel_playing[a] = 1;
      return a;
    }
    void Play_Music(int i) {
      Mix_PlayMusic(music[i],-1);
    }
   void Play_Music() {
      Mix_ResumeMusic();
    }
    void Pause_Music() {
      Mix_PauseMusic();
    }

    void Pause_Channel(int ch) {
      Mix_Pause(ch);
    }

    void Start_Channel(int ch) {
      Mix_Resume(ch);
    }

    void Stop_Channel(int ch) {
      Mix_HaltChannel(ch);
      channel_playing[ch] = 0;
    }
  };

// since multiple sprites can be used, clean them up using this:

// vars
  unsigned char* keys = nullptr;
  int camera_offset_x = 0,
      camera_offset_y = 0;
  bool mouse_left = 0, mouse_right = 0, scroll_up = 0, scroll_down = 0;
  int mouse_x = 0, mouse_y = 0;
  std::vector<std::vector<Object*>> objs;
  std::vector<GUI*> sprs[4];
  std::vector<Text*> texts;
  SDL_Surface* window = nullptr;

  std::ofstream debug("Cigarette_Butt_Debug.log");
}