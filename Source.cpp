#include "Cigarette_Butt.h"
#include "LD_31.h"

void Initialize_Game() {
  cig::Text::Load_Font("courier.ttf", 12);
}

int main(int argc, char* argv[]) {
  cig::Set_Up();
  Initialize_Game();
  Game::Goto_Next_Level(Game::Stages::Menu);

  auto prev_time = SDL_GetTicks();
  float p_m_x = 0, p_m_y = 0;

  cig::Animation* smoke_sheet = new cig::Animation("smoke_spritesheet.png",10,20,10);
 
  cig::Object* t_smoke = nullptr;

  const int Smoke_max_counter = 100;
  int smoke_counter = 0;

  cig::Object* t = t_smoke;
  bool one_pressed = 0;
  while ( true ) {
    prev_time = SDL_GetTicks();
    cig::Refresh_Keys();

    if ( Game::current_stage == Game::Stages::Playing ) {
      if ( t_smoke == nullptr ) 
        t_smoke = cig::Add_Obj(new cig::Object(smoke_sheet, 5,
            Game::gl_player->R_Pos_X() + 15 - 3,
            Game::gl_player->R_Pos_Y() + 7 - 18));

      t_smoke->Set_Pos_X(Game::gl_player->R_Pos_X() );
      t_smoke->Set_Pos_Y(Game::gl_player->R_Pos_Y() + 7 - 20);

      Game::gl_player->Refresh();

      if ( Game::gl_player == nullptr ) // game over
        goto EXIT_STAGE_THING;

      for ( int i = 0; i < Game::enemies.size(); ++ i ) {
        Game::enemies[i]->Refresh();
      }

      if ( Game::gl_player == nullptr ) // game over
        goto EXIT_STAGE_THING;

      for ( int i = 0; i < Game::call_refresh_objs.size(); ++ i ) {
        Game::call_refresh_objs[i]->Refresh();
      }

      if ( Game::gl_player == nullptr ) // game over
        goto EXIT_STAGE_THING;

      if ( Game::gl_player->R_Health() <= 0 )
        Game::Goto_Next_Level(Game::Stages::Dead);

    } else if ( Game::current_stage == Game::Stages::Menu ) {
      t_smoke = nullptr;
      if ( cig::mouse_left ) {
        Game::current_level = 0;
        Game::Goto_Next_Level(Game::Stages::Playing);
      }
    } else if ( Game::current_stage == Game::Stages::Dead ) {
      t_smoke = nullptr;
      if ( cig::mouse_left )
        Game::Goto_Next_Level(Game::Stages::Playing);
    }

    EXIT_STAGE_THING:

    cig::Refresh_Screen();

    if ( SDL_GetTicks() - prev_time < 1000 / 30 ) {
      SDL_Delay( (1000/30) - (SDL_GetTicks() - prev_time) );
    }


    SDL_Event event;
    while ( SDL_PollEvent( &event ) ) {
      if ( event.type == SDL_QUIT )
        goto END_GAME;
    }
    if ( cig::keys[SDLK_ESCAPE] )
      goto END_GAME;
    if ( cig::keys[SDLK_1] && !one_pressed ) {
      ++Game::current_level;
      Game::Goto_Next_Level(Game::Stages::Playing);
    }
    if ( cig::keys[SDLK_r] )
      if ( Game::gl_player )
        Game::gl_player->Add_Health(-2000);
    one_pressed = cig::keys[SDLK_1];
  }

  END_GAME:
  cig::Clean_Up();
  return 0;
}