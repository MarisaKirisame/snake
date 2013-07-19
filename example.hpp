#pragma once
#include "environment.hpp"
namespace snake
{
  int example( void )
  {
    int i;
    cin >> i;
    env game( 10, 10, snake::env::representing_char( 'w', 's', 'f' ), 1, { snake::env::coord( 0,0 ) } );
    while ( ! game.is_game_over( ) )
    {
      auto dir = game.get_dir( );
      cout << game << endl;
      game.move_snake( dir );
      this_thread::sleep_for( chrono::milliseconds( 50 ) );
    }
    cout << game.score( );
    return 0;
  }
}
