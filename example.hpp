#pragma once
#include "environment.hpp"
namespace snake
{
  int example( void )
  {
    srand( time( 0 ) );
    env game;
    game.length = 10;
    game.width = 20;
    game.rc.wall = 'w';
    game.rc.snake = 's';
    game.rc.food = 'f';
    typedef pair< size_t, size_t > point;
    game.snake = list< point >( { point( 0,0 ) } );
    game.food_num = 1;
    game.reset( );
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
