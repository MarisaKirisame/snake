#pragma once
#include "environment.hpp"
namespace snake
{
  int example( void )
  {
    env game( 10, 20, snake::env::representing_char( 'w', 's', 'f' ), 1, { snake::env::coord( 0,0 ) } );
    while ( ! game.is_game_over( ) )
    {
      auto dir = game.get_dir( );
      cout << dir << endl;
      cout << game << endl;
      game.move_snake( dir );
      this_thread::sleep_for( chrono::milliseconds( 100 ) );
    }
    cout << game.score( );
    return 0;
  }
}
