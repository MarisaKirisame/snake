#pragma once
#include <thread>
#include <list>
#include <set>
#include <boost/cast.hpp>
#include <random>
#include <functional>
#include "represent_char.hpp"
#include "coord.hpp"
#include "point/point.hpp"
#include "path.hpp"
using namespace std;
namespace snake
{

  enum point_status
  {
    connected_to_others, not_coonected_to_others_yet, wall
  };

  struct point_fact
  {
    function< point * ( size_t, env &, coord && ) > snake_fact;
    function< point * ( env &, coord && ) > food_fact;
    function< point * ( env &, coord && ) > space_fact;
    point_fact( function< point * ( size_t, env &, coord && ) > && snake_fact, function< point * ( env &, coord && ) > && food_fact, function< point * ( env &, coord && ) > && space_fact ) :
      snake_fact( snake_fact ), food_fact( food_fact ), space_fact( space_fact ) { }
  };

  struct env
  {
    size_t score( ) { return s_snake.size( ) - 1; }
    point_fact fact;
    env( point_fact && pf, size_t length, size_t width, representing_char && rc, size_t food_num, list< coord >&& snake ) : fact( pf ) { reset( length, width, move( rc ), food_num, move( snake ) ); }
    void reset( size_t length, size_t width, representing_char && rc, size_t food_num, list< coord >&& snake );
    bool is_game_over( ) const { return ! is_snake_alive; }
    bool is_in_map_after_move( const coord &, direction dir ) const;
    bool is_alive_after_move( direction dir ) const;
    bool can_pass_to( const coord &, direction, size_t time = 0 ) const;
    void move_snake( direction dir );
    bool gen_food( size_t num );
    coord cur_head;
    size_t cur_map_length;
    size_t cur_map_width;
    size_t food_in_snake;
    bool is_snake_alive;
    representing_char rc;
    set< coord > s_food;
    set< coord > s_snake;
    set< coord > s_space;
    vector< vector< shared_ptr< point > > > vec;
  };
  ostream & operator << ( ostream & os, env & e );
}
namespace snake
{
  void env::reset( size_t length, size_t width, representing_char && rc, size_t food_num, list<coord> && snake )
  {
    this->rc = rc;
    food_in_snake = 0;
    srand( time( 0 ) );
    vec.clear( );
    is_snake_alive = true;
    assert( length > 0 );
    assert( width > 0 );
    cur_map_length = length;
    cur_map_width = width;
    vec.resize( cur_map_length, vector< shared_ptr< point > >( cur_map_width ) );
    for ( size_t i = 0; i < cur_map_length; ++i )
    {
      for ( size_t ii = 0; ii < cur_map_width; ++ii )
      {
        vec[ i ][ ii ] = shared_ptr< point >( fact.space_fact( * this, move( coord( i, ii ) ) ) );
      }
    }
    cur_head = snake.front( );
    size_t life = snake.size( );
    assert( life > 0 );
    for ( auto & i : snake )
    {
      vec[ i.y ][ i.x ]->pass( shared_ptr< point >( fact.snake_fact( life, * this, move( i ) ) ) );
      --life;
    }
    assert( life == 0 );
    gen_food( food_num );
  }

  bool env::is_in_map_after_move( const coord & c, direction dir ) const
  {
    switch ( dir )
    {
    case up:
      return c.y != 0;
    case down:
      return c.y + 1 < cur_map_length;
    case left:
      return c.x != 0;
    default:
      assert( dir == right );
      return c.x + 1 < cur_map_width;
    }
  }

  bool env::is_alive_after_move( direction dir ) const
  {
    return can_pass_to( cur_head, dir );
  }

  bool env::can_pass_to( const coord & c, direction dir, size_t time ) const
  {
    if ( is_in_map_after_move( c, dir ) )
    {
      auto next = c( dir );
      return vec[ next.y ][ next.x ]->can_pass_after( time );
    }
    else
    {
      return false;
    }
  }

  void env::move_snake( direction dir )
  {
    assert( is_snake_alive );
    if ( ! is_alive_after_move( dir ) )
    {
      is_snake_alive = false;
    }
    else
    {
      coord next = cur_head( dir );
      auto next_square = vec[ next.y ][ next.x ];
      auto cur_square = vec[ cur_head.y ][ cur_head.x ];
      next_square->pass( shared_ptr< point >( fact.snake_fact( cur_square->get_snake( )->can_pass_after( ) + 1, * this, move( next ) ) ) );
      for ( size_t i = 0; i < cur_map_length; ++i )
      {
        for ( size_t ii = 0; ii < cur_map_width; ++ii )
        {
          vec[ i ][ ii ]->pass( );
        }
      }
      cur_head = next;
    }
  }

  bool env::gen_food( size_t num )
  {
    if( s_space.empty( ) )
    {
      return false;
    }
    else
    {
      for ( size_t i = 0; i < num; ++i )
      {
        size_t dis = rand( ) % s_space.size( );
        auto it = s_space.begin( );
        advance( it, dis );
        assert( it != s_space.end( ) );
        auto pos = * it;
        vec[ pos.y ][ pos.x ]->pass( shared_ptr< point >( fact.food_fact( * this, move( pos ) ) ) );
      }
      return true;
    }

  }

  ostream & operator << ( ostream & os, env & e )
  {
    assert( e.is_snake_alive );
    os << string( e.cur_map_width + 2, e.rc.wall ) << endl;
    for( auto & i : e.vec )
    {
      assert( i.size( ) == size_t( e.cur_map_width ) );
      os << e.rc.wall;
      for ( auto & ii : i ) { os << ii->representing_char( ); }
      os << e.rc.wall;
      os << endl;
    }
    os << string( e.cur_map_width + 2, e.rc.wall ) << endl;
    return os;
  }

}
