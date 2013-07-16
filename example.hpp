#pragma once
#include "environment.hpp"
namespace snake
{
  int example( void )
  {
    srand( time( 0 ) );
    env game;
    game.length = 10;
    game.width = 10;
    game.rc.wall = 'w';
    game.rc.snake = 's';
    game.rc.food = 'f';
    typedef pair< size_t, size_t > point;
    game.snake = list< point >( { point( 0,0 ) } );
    game.food_num = 1;
    game.reset( );
    while ( ! game.is_game_over( ) )
    {
      cout << game << endl;
      game.move_snake( game.get_dir( ) );
      this_thread::sleep_for( chrono::milliseconds( 1000 ) );
    }
    cout << game.score( );
    return 0;
  }
}
