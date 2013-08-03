#pragma once
#include "environment.hpp"
#include "cyclic_route.hpp"
#include "point/snake_piece.hpp"
#include "point/food.hpp"
#include "point/space.hpp"
namespace snake
{

  point * snake_factory( size_t life, env & e, coord && c )
  {
     return new snake_piece( life, e, move( c ) );
  }

  point *  food_factory( env & e, coord && c )
  {
    return new food( e, move( c ) );
  }

  point * space_factory( env & e, coord && c )
  {
    return new space( e, move( c ) );
  }

  int example( void )
  {
    //int i;
    //cin >> i;
    env game( move( point_fact( snake_factory, food_factory,space_factory ) ),2, 70, representing_char( 'w', 's', 'f', 'h', 't' ), 1, { coord( 0,0 ) } );
    cyclic_route cr( game );
    while ( ! game.is_game_over( ) )
    {
      cout << game << endl;
      auto dir = cr.get_dir( );
      game.move_snake( dir );
      this_thread::sleep_for( chrono::milliseconds( 100 ) );
    }
    cout << game.score( );
    return 0;
  }
}
