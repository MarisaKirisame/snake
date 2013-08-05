#pragma once
#include <algorithm>
#include "coord.hpp"
#include "direction.hpp"
#include "path.hpp"
#include "environment.hpp"
#include <functional>
#include "point/food.hpp"
using placeholders::_1;
using placeholders::_2;
namespace snake
{

  struct cyclic_route;

  struct cyclic_route_iterator : iterator< forward_iterator_tag, coord >
  {
    coord c;
    const cyclic_route & cr;
    const coord head;
    bool is_end;
    cyclic_route_iterator( const cyclic_route &, coord &&, bool is_end );
    cyclic_route_iterator( const cyclic_route &, const coord &, bool is_end );
    bool operator ==( const cyclic_route_iterator & );
    bool operator !=( const cyclic_route_iterator & );
    coord operator *( );
    coord operator ++ ( );
    coord operator ++ ( int );
  };

  struct cyclic_route
  {
    vector< vector< pair< bool, direction > > > cr;
    const env & e;
    typedef cyclic_route_iterator iterator;

    iterator begin( ) const;
    void clear( );
    cyclic_route( const env & e ) : e( e ) { resize( e.cur_map_length, e.cur_map_width ); }
    iterator end( ) const;
    bool extend_rect( const coord & c1, const coord c2 );
    bool extend_rect( const coord & c1, const coord c2, const coord & c3, const coord & c4 );
    direction get_dir( );
    pair< bool, path > get_path( const coord & from, const coord & to, size_t min_path_size, size_t max_path_size ) const;
    pair< bool, path > get_path( const coord & from, const coord & to ) const;
    pair< bool, path > get_path( const coord & from, const coord & to, size_t min_path_size, size_t max_path_size, const set< coord > & occupied ) const;
    bool have( const coord & ) const;
    bool need_update( ) const;
    direction operator [ ] ( const coord & ) const;
    void resize( size_t length, size_t width );
    size_t size( ) const;
    void update( const path & p );
    void update( );

  };

}
namespace snake
{

  coord cyclic_route_iterator::operator ++ ( int )
  {
    coord c( * * this );
    ++( * this );
    return c;
  }

  coord cyclic_route_iterator::operator ++ ( )
  {
    assert( ! is_end );
    c = c( cr[ c ] );
    if ( c == head )
    {
      is_end = true;
    }
    return * * this;
  }

  coord cyclic_route_iterator::operator * ( )
  {
    return c;
  }

  cyclic_route_iterator::cyclic_route_iterator ( const cyclic_route & cr, const coord & c, bool is_end ) : c( c ), cr( cr ), head( c ), is_end( is_end ) { }

  bool cyclic_route_iterator::operator == ( const cyclic_route_iterator & i )
  {
    assert( ( & cr ) == ( & i.cr ) );
    return ( c == i.c ) && ( is_end == i.is_end );
  }

  bool cyclic_route_iterator::operator != ( const cyclic_route_iterator & i )
  {
    return ! ( ( * this ) == i );
  }

  bool cyclic_route::have( const coord & c ) const
  {
    return cr[ c.y ][ c.x ].first;
  }

  direction cyclic_route::operator [ ]( const coord & c ) const
  {
    assert( have( c ) );
    return cr[ c.y ][ c.x ].second;
  }

  void cyclic_route::resize( size_t length, size_t width )
  {
    cr.clear( );
    cr.resize( length, vector< pair< bool, direction > >( width ) );
  }

  size_t cyclic_route::size( ) const
  {
    return distance( begin( ), end( ) );
  }

  cyclic_route::iterator cyclic_route::begin( ) const
  {
    if ( have( e.cur_head ) )
    {
      return iterator( * this, e.cur_head, false );
    }
    else
    {
      return end( );
    }
  }

  cyclic_route::iterator cyclic_route::end( ) const
  {
    return iterator( * this, e.cur_head, true );
  }

  void cyclic_route::update( const path & p )
  {
    coord c = p.head;
    for ( auto & i : p.dir )
    {
      auto & square = cr[ c.y ][ c.x ];
      assert( ! square.first );
      square = make_pair( true, i );
      c = c( i );
    }
  }

  void cyclic_route::update( )
  {
    assert( ! e.s_food.empty( ) );
    const coord & f = * e.s_food.begin( );
    if ( ( e.s_snake.size( ) == 1 ) && ( ! have( e.cur_head ) ) )
    {
      auto to = get_path( e.cur_head, f );
      assert( to.first );
      update( to.second );
      size_t two_if_on_same_line = f.is_on_same_line( e.cur_head ) ? 2 : 0;
      auto back = get_path( f, e.cur_head, coord::distance( f, e.cur_head ) + two_if_on_same_line, coord::distance( f, e.cur_head ) + two_if_on_same_line );
      assert( back.first );
      update( back.second );
    }
    else if ( cr[ f.y ][ f.x ].first == false )
    {
      vector< size_t > dis( size( ) );
      vector< coord > coo( size( ) );
      transform( begin( ), end( ), dis.begin( ), bind( coord::distance, f, _1 ) );
      transform( begin( ), end( ), coo.begin( ), move< coord > );
      auto res = min_element( dis.begin( ), dis.end( ) );
      coord closest_coord( coo[ distance( dis.begin( ), res ) ] );
      if ( closest_coord.is_on_same_line( f ) )
      {
        if( ! extend_rect( closest_coord, f ) )
        { extend_rect( closest_coord, f );assert( false ); }
      }
      else
      {
        assert( false );
      }
    }
  }

  bool cyclic_route::extend_rect( const coord & c1, const coord c2 )
  {
    if ( c1.is_on_same_line( c2 ) )
    {
      direction c1_to_c2( c1.get_heading( c2 ) );
      if ( c1_to_c2 != ( * this )[ c1 ] && c1_to_c2 != ( * this )[ c1 ].reverse( ) )
      {
        return extend_rect( c1, c2, c2( ( * this )[ c1 ] ), c1( ( * this )[ c1 ] ) );
      }
      else
      {
        assert( c1_to_c2 == ( * this )[ c1 ].reverse( ) );
        direction prev_dir = c1_to_c2.tilt( );
        if ( ! ( e.is_in_map_after_move( c1, prev_dir ) && have( c1( prev_dir ) ) && ( * this )[ c1( prev_dir ) ] == prev_dir.reverse( ) ) )
        {
          prev_dir = prev_dir.reverse( );
        }
        assert( ( e.is_in_map_after_move( c1, prev_dir ) && have( c1( prev_dir ) ) && ( * this )[ c1( prev_dir ) ] == prev_dir.reverse( ) ) );
        return extend_rect( c1( prev_dir ), c2( prev_dir ), c2, c1 );
      }
    }
    else
    {
      return extend_rect( c1, coord( c1.y, c2.x ), c2, coord( c2.y, c1.x ) ) || extend_rect( c1, coord( c2.y, c1.x ), c2, coord( c1.y, c2.x ) );
    }
  }

  direction cyclic_route::get_dir( )
  {
    if( need_update( ) ) { update( ); }
    assert( ! need_update( ) );
    assert( have( e.cur_head ) );
    return ( * this )[ e.cur_head ];
  }

  bool cyclic_route::extend_rect( const coord & c1, const coord c2, const coord & c3, const coord & c4 )
  {
    assert( c1.is_on_same_line( c2 ) );
    assert( c2.is_on_same_line( c3 ) );
    assert( c3.is_on_same_line( c4 ) );
    assert( c4.is_on_same_line( c1 ) );
    assert( ! c1.is_on_same_line( c3 ) );
    assert( ! c2.is_on_same_line( c4 ) );
    assert( have( c1 ) == true );
    assert( have( c2 ) == false );
    assert( have( c3 ) == false );
    assert( have( c4 ) == true );
    cr[ c1.y ][ c1.x ].first = false;
    pair< bool, path > c1_to_c2( get_path( c1, c2 ) );
    pair< bool, path > c2_to_c3( get_path( c2, c3 ) );
    pair< bool, path > c3_to_c4( get_path( c3, c4 ) );
    if( c1_to_c2.first &&
        c2_to_c3.first &&
        c3_to_c4.first )
    {
      update( c1_to_c2.second );
      update( c2_to_c3.second );
      update( c3_to_c4.second );
      return true;
    }
    cr[ c1.y ][ c1.x ].first = true;
    return false;
  }

  bool is_food( const env & e, const coord & c )
  {
    return dynamic_cast< food * >( e.vec[ c.y ][ c.x ]->get_object_on_top( ).get( ) ) != nullptr;
  }

  bool cyclic_route::need_update( ) const
  {
    return ! any_of( begin( ), end( ), function< bool ( const coord & ) >( bind( is_food, e, placeholders::_1 ) ) );
  }

  pair< bool, path > cyclic_route::get_path(const coord & from, const coord & to, size_t min_path_size, size_t max_path_size ) const
  {
    return get_path( from, to, min_path_size, max_path_size, set< coord >( ) );
  }

  pair< bool, path > cyclic_route::get_path( const coord & from, const coord & to ) const
  {
    auto dis = coord::distance( from, to );
    return get_path( from, to, dis, dis );
  }

  pair< bool, path > cyclic_route::get_path( const coord & from, const coord & to, size_t min_path_size, size_t max_path_size, const set<coord> & occupied ) const
  {
    if ( from == to && min_path_size == 0 )
    {
      return make_pair( true, path( from ) );
    }
    else if ( max_path_size < coord::distance( from, to ) ||
              max_path_size < min_path_size ||
              ( ( ! occupied.empty( ) ) && have( from ) ) ||
              occupied.find( from ) != occupied.end( ) )
    {
      return pair< bool, path >( false, path( coord( ) ) );
    }
    else
    {
      set< coord > new_occupied( occupied );
      new_occupied.insert( from );
      if ( e.is_in_map_after_move( from, up ) )
      {
        pair< bool,  path > up_res( get_path( coord( from( up ) ), to, min_path_size - 1, max_path_size - 1, new_occupied ) );
        if ( up_res.first )
        {
          up_res.second.push_front( up );
          return up_res;
        }
      }
      if ( e.is_in_map_after_move( from, down ) )
      {
        pair< bool,  path > down_res( get_path( coord( from( down ) ), to, min_path_size - 1, max_path_size - 1, new_occupied ) );
        if ( down_res.first )
        {
          down_res.second.push_front( down );
          return down_res;
        }
      }
      if ( e.is_in_map_after_move( from, left ) )
      {
        pair< bool,  path > left_res( get_path( coord( from( left ) ), to, min_path_size - 1, max_path_size - 1, new_occupied ) );
        if ( left_res.first )
        {
          left_res.second.push_front( left );
          return left_res;
        }
      }
      if ( e.is_in_map_after_move( from, right ) )
      {
        pair< bool,  path > right_res( get_path( coord( from( right ) ), to, min_path_size - 1, max_path_size - 1, new_occupied ) );
        if ( right_res.first )
        {
          right_res.second.push_front( right );
          return right_res;
        }
      }
      return make_pair( false, path( coord( ) ) );
    }
  }
}
