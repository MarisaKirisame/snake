#pragma once
#include "environment.hpp"
namespace snake
{
  int example( void )
  {
    //int i;
    //cin >> i;
    env game( 15, 30, representing_char( 'w', 's', 'f', 'h', 't' ), 1, { coord( 0,0 ) } );
    while ( ! game.is_game_over( ) )
    {
      cout << game << endl;
      auto dir = game.get_dir( );
      game.move_snake( dir );
      //this_thread::sleep_for( chrono::milliseconds( 100 ) );
    }
    cout << game.score( );
    return 0;
  }
}
