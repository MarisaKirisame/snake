#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <cassert>
#include <vector>
#include <memory>
#include <set>
#include <boost/cast.hpp>
#include <random>
#include "represent_char.hpp"
#include "coord.hpp"
#include "point/point.hpp"
using namespace std;
namespace snake
{
  enum direction
  {
    up, down, left, right
  };

  enum point_status
  {
    connected_to_others, not_coonected_to_others_yet, wall
  };

  struct env
  {
    bool have_path( const coord from, const coord & to, const size_t min_path_size ) const;
    direction get_dir( ) const;
    size_t score( ) { return s_snake.size( ) - 1; }
    env( ) : is_snake_alive( false ) { }
    env( size_t length, size_t width, representing_char && rc, size_t food_num, list< coord >&& snake )
    { reset( length, width, move( rc ), food_num, move( snake ) ); }
    void reset( size_t length, size_t width, representing_char && rc, size_t food_num, list< coord >&& snake );
    bool is_game_over( ) const { return ! is_snake_alive; }
    bool is_in_map_after_move( direction dir ) const;
    coord get_coord( direction dir ) const;
    bool is_alive_after_move( direction dir ) const;
    static size_t distance( const coord & x, const coord & y )
    { return max( x.first, y.first ) - min( x.first, y.first ) + max( x.second, y.second ) - min( x.second, y.second ); }
    bool is_safe_after_move( direction dir ) const;
    void move_snake( direction dir );
    void update_route_to_tail( ) { }
    size_t space_left;
    bool is_snake_alive;
    coord cur_head;
    size_t cur_map_length;
    size_t cur_map_width;
    representing_char rc;
    size_t food_in_snake;
    vector< vector< pair< bool, direction > > > route_to_tail;
    void gen_food( size_t num );
    set< coord > s_snake;
    set< coord > s_space;
    set< coord > s_food;
    vector< vector< shared_ptr< point > > > vec;
    friend ostream & operator << ( ostream & os, env & e );
  };
}
#include "point/space.hpp"
#include "point/food.hpp"
#include "point/snake_piece.hpp"
namespace snake
{
  bool env::have_path(const coord from, const coord & to, const size_t min_path_size) const
  {
    if ( distance( from, to ) >= min_path_size )
    {
      return true;
    }
    else
    {
      vector< vector< size_t > > map( cur_map_length, vector< size_t >( cur_map_width, 0 ) );
      if ( from.first > 0 )
      {
        map[ from.first - 1 ][ from.second ] = 1;
      }
      if ( from.first < cur_map_length - 1 )
      {
        map[ from.first + 1 ][ from.second ] = 1;
      }
      if ( from.second > 0 )
      {
        map[ from.first ][ from.second - 1 ] = 1;
      }
      if ( from.first < cur_map_length - 1 )
      {
        map[ from.first ][ from.second + 1 ] = 1;
      }
      size_t cur = 2;
      bool still_calculating = true;
      while ( still_calculating == true )
      {
        still_calculating = false;
        for ( size_t i = 0; i < cur_map_length; ++i )
        {
          for ( size_t ii = 0; ii < cur_map_width; ++ii )
          {
            if( vec[ i ][ ii ]->can_pass( ) && map[ i ][ ii ] == 0 &&
                ( ( i > 0 && map[ i - 1 ][ ii ] == cur - 1 ) ||
                  ( i + 1 < cur_map_length && map[ i +1 ][ ii ] == cur - 1 ) ||
                  ( ii > 0 && map[ i ][ ii - 1 ] == cur - 1 ) ||
                  ( ii + 1 < cur_map_width && map[ i ][ ii + 1 ] == cur - 1 ) ) )
            {
              if ( make_pair( i, ii ) == to )
              {
                return cur_map_length >= min_path_size;
              }
              else if ( make_pair( i, ii ) != from )
              {
                map[ i ][ ii ] = cur;
              }
              still_calculating = true;
            }
          }
        }
        ++cur;
      }
      return false;
    }
  }

  direction env::get_dir( ) const
  {
    if ( s_food.empty( ) )
    {
      if ( is_safe_after_move( down ) )
      {
        return down;
      }
      else if ( is_safe_after_move( up ) )
      {
        return up;
      }
      else if ( is_safe_after_move( left ) )
      {
        return left;
      }
      else if ( is_safe_after_move( right ) )
      {
        return right;
      }
      if ( is_alive_after_move( down ) )
      {
        return down;
      }
      else if ( is_alive_after_move( up ) )
      {
        return up;
      }
      else if ( is_alive_after_move( left ) )
      {
        return left;
      }
      else
      {
        return right;
      }
    }
    else
    {
      auto f_pos = * s_food.begin( );
      if ( f_pos.first > cur_head.first )
      {
        if ( f_pos.first == cur_map_length - 1 )
        {
          if ( f_pos.second > cur_head.second )
          {
            if ( is_safe_after_move( right ) )
            {
              return right;
            }
          }
          else if ( f_pos.second < cur_head.second )
          {
            if ( is_safe_after_move( left ) )
            {
              return left;
            }
          }
        }
        if ( is_safe_after_move( down ) )
        {
          return down;
        }
      }
      else if ( f_pos.first < cur_head.first )
      {
        if ( f_pos.first == 0 )
        {
          if ( f_pos.second > cur_head.second )
          {
            if ( is_safe_after_move( right ) )
            {
              return right;
            }
          }
          else if ( f_pos.second < cur_head.second )
          {
            if ( is_safe_after_move( left ) )
            {
              return left;
            }
          }
        }
        if ( is_safe_after_move( up ) )
        {
          return up;
        }
      }
      if ( f_pos.second > cur_head.second )
      {
        if ( is_safe_after_move( right ) )
        {
          return right;
        }
      }
      else if ( f_pos.second < cur_head.second )
      {
        if ( is_safe_after_move( left ) )
        {
          return left;
        }
      }
      if ( is_safe_after_move( down ) )
      {
        return down;
      }
      else if ( is_safe_after_move( up ) )
      {
        return up;
      }
      else if ( is_safe_after_move( left ) )
      {
        return left;
      }
      else if ( is_safe_after_move( right ) )
      {
        return right;
      }
      if ( is_safe_after_move( down ) )
      {
        return down;
      }
      else if ( is_safe_after_move( up ) )
      {
        return up;
      }
      else if ( is_safe_after_move( left ) )
      {
        return left;
      }
      else if ( is_safe_after_move( right ) )
      {
        return right;
      }
      if ( f_pos.first > cur_head.first )
      {
        if ( is_alive_after_move( down ) )
        {
          return down;
        }
      }
      else if ( f_pos.first < cur_head.first )
      {
        if ( is_alive_after_move( up ) )
        {
          return up;
        }
      }
      if ( f_pos.second > cur_head.second )
      {
        if ( is_alive_after_move( right ) )
        {
          return right;
        }
      }
      else if ( f_pos.second < cur_head.second )
      {
        if ( is_alive_after_move( left ) )
        {
          return left;
        }
      }
      if ( is_alive_after_move( down ) )
      {
        return down;
      }
      else if ( is_alive_after_move( up ) )
      {
        return up;
      }
      else if ( is_alive_after_move( left ) )
      {
        return left;
      }
      else
      {
        return right;
      }
    }
  }

  void env::reset( size_t length, size_t width, representing_char && rc, size_t food_num, list<coord> && snake )
  {
    this->rc = rc;
    food_in_snake = 0;
    srand( time( 0 ) );
    vec.clear( );
    is_snake_alive = true;
    assert( length > 0 );
    assert( width > 0 );
    space_left = 0;
    cur_map_length = length;
    cur_map_width = width;
    vec.resize( cur_map_length, vector< shared_ptr< point > >( cur_map_width ) );
    route_to_tail.resize( cur_map_length, vector< pair< bool, direction > >( cur_map_width, make_pair( false, up ) ) );
    for ( size_t i = 0; i < cur_map_length; ++i )
    {
      for ( size_t ii = 0; ii < cur_map_width; ++ii )
      {
        //vec[ i ][ ii ] = shared_ptr< point >( new space( * this, move( make_pair( i, ii ) ) ) );
      }
    }
    cur_head = snake.front( );
    size_t life = snake.size( );
    assert( life > 0 );
    for ( auto & i : snake )
    {
      vec[ i.first ][ i.second ]->pass( shared_ptr< point >( new snake_piece( life, * this, move( i ) ) ) );
      --life;
    }
    assert( life == 0 );
    gen_food( food_num );
  }

  bool env::is_in_map_after_move(direction dir) const
  {
    switch ( dir )
    {
    case up:
      return cur_head.first != 0;
    case down:
      return cur_head.first + 1 < cur_map_length;
    case left:
      return cur_head.second != 0;
    case right:
      return cur_head.second + 1 < cur_map_width;
    default:
      assert( false );
    }
  }

  coord env::get_coord( direction dir ) const
  {
    assert( is_in_map_after_move( dir ) );
    coord next = cur_head;
    switch ( dir )
    {
    case up:
      --next.first;
      break;
    case down:
      ++next.first;
      break;
    case left:
      --next.second;
      break;
    case right:
      ++next.second;
      break;
    default:
      assert( false );
    }
    return next;
  }

  bool env::is_alive_after_move( direction dir ) const
  {
    if ( is_in_map_after_move( dir ) )
    {
      auto next = get_coord( dir );
      return vec[ next.first ][ next.second ]->can_pass( );
    }
    else
    {
      return false;
    }
  }

  bool env::is_safe_after_move( direction dir ) const
  {
    if ( s_snake.size( ) <= 8 )
    {
      return is_alive_after_move( dir );//Impossible to run into deadend
    }
    else if ( is_alive_after_move( dir ) )
    {
      auto next = get_coord( dir );
      vector< vector< point_status > > map( cur_map_length, vector< point_status >( cur_map_width ) );
      for ( size_t i = 0; i < cur_map_length; ++i )
      {
        for ( size_t ii = 0; ii < cur_map_width; ++ii )
        {
          if ( next == make_pair( i, ii ) ) { map[ i ][ ii ] = wall; }
          else if ( vec[ i ][ ii ]->can_pass_after( 1 ) )
          {
            if ( distance( make_pair( i, ii ), next ) == 1 )
            {
              if ( ! vec[ i ][ ii ]->can_pass( ) )
              {
                return true;
              }
              else
              {
                map[ i ][ ii ] = connected_to_others;
              }
            }
            else { map[ i ][ ii ] = not_coonected_to_others_yet; }
          }
          else { map[ i ][ ii ] = wall; }
        }
      }
      bool still_calculating = true;
      while ( still_calculating )
      {
        still_calculating = false;
        for ( size_t i = 0; i < cur_map_length; ++i )
        {
          for ( size_t ii = 0; ii < cur_map_width; ++ii )
          {
            if ( ( distance( make_pair( i, ii ), next ) > food_in_snake ) &&
                 ( ! vec[ i ][ ii ]->can_pass( ) ) &&
                 ( ( i > 0 && map[ i - 1 ][ ii ] == connected_to_others ) ||
                   ( i + 1 < cur_map_length && map[ i +1 ][ ii ] == connected_to_others ) ||
                   ( ii > 0 && map[ i ][ ii - 1 ] == connected_to_others ) ||
                   ( ii + 1 < cur_map_width && map[ i ][ ii + 1 ] == connected_to_others ) ) &&
                 have_path( next, make_pair( i, ii ), vec[ i ][ ii ]->can_pass_after( ) + food_in_snake + 1 ) )
            {
              return true;
            }
            else if ( ( map[ i ][ ii ] == not_coonected_to_others_yet ) &&
                      ( ( i > 0 && map[ i - 1 ][ ii ] == connected_to_others ) ||
                        ( i + 1 < cur_map_length && map[ i +1 ][ ii ] == connected_to_others ) ||
                        ( ii > 0 && map[ i ][ ii - 1 ] == connected_to_others ) ||
                        ( ii + 1 < cur_map_width && map[ i ][ ii + 1 ] == connected_to_others ) ) )
            {
              map[ i ][ ii ] = connected_to_others;
              still_calculating = true;
            }
          }
        }
      }
      return false;
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
      coord next = get_coord( dir );
      auto next_square = vec[ next.first ][ next.second ];
      auto cur_square = vec[ cur_head.first ][ cur_head.second ];
      next_square->pass( shared_ptr< snake_piece >( new snake_piece
                                                    ( boost::polymorphic_cast< snake_piece * >( cur_square->get_snake( ).get( ) )->life + 1, * this, move( next ) ) ) );
      list< coord > zombie_snake_piece;
      for ( auto & i : s_snake )
      {
        size_t & life = boost::polymorphic_cast< snake_piece * >( vec[ i.first ][ i.second ]->get_snake( ).get( ) )->life;
        --life;
        if ( life == 0 ) { zombie_snake_piece.push_back( i ); }
      }
      assert( zombie_snake_piece.size( ) <= 1 );
      for ( auto & i : zombie_snake_piece )
      {
        assert( boost::polymorphic_cast< snake_piece * >( vec[ i.first ][ i.second ]->get_snake( ).get( ) )->life == 0 );
        vec[ i.first ][ i.second ]->leave( );
      }
      cur_head = next;
      update_route_to_tail( );
    }
  }

  void env::gen_food( size_t num )
  {
    assert( ! s_space.empty( ) );
    for ( size_t i = 0; i < num; ++i )
    {
      size_t dis = rand( ) % s_space.size( );
      auto it = s_space.begin( );
      advance( it, dis );
      assert( it != s_space.end( ) );
      auto pos = * it;
      vec[ pos.first ][ pos.second ]->pass( shared_ptr< point >( new food( * this, move( pos ) ) ) );
    }
  }

  ostream & operator <<( ostream & os, env & e )
  {
    assert( e.is_snake_alive );
    os << e.space_left << endl;
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
