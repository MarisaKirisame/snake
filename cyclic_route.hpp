#pragma once
#include <algorithm>
#include "coord.hpp"
#include "direction.hpp"
#include "path.hpp"
#include "environment.hpp"
#include <functional>
#include "point/food.hpp"
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
    bool extend_thin_rect( const coord & c1, const coord c2 );
    bool extend_rect( const coord & c1, const coord c2, const coord & c3, const coord & c4 );
    direction get_dir( );
    pair< bool, path > get_path( const coord & from, const coord & to, size_t min_path_size, size_t max_path_size, size_t time ) const;
    pair< bool, path > get_path( const coord & from, const coord & to, size_t time ) const;
    pair< bool, path > get_path( const coord & from, const coord & to ) const;
    pair< bool, path > get_path( const coord & from, const coord & to, size_t min_path_size, size_t max_path_size, const set< coord > & occupied, size_t time ) const;    bool have( const coord & ) const;
    bool need_update( ) const;
    direction operator [ ] ( const coord & ) const;
    void resize( size_t length, size_t width );
    void update_route( const path & p );
    void update_route( );

  };

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

}
namespace snake
{

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

  void cyclic_route::update_route( const path & p )
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

  void cyclic_route::update_route( )
  {
    assert( ! e.s_food.empty( ) );
    const coord & f = * e.s_food.begin( );
    if ( ( e.s_snake.size( ) == 1 ) && ( ! have( e.cur_head ) ) )
    {
      direction dir;
      bool break_out = false;
      list< direction > ld( { up, down, left, right } );
      for ( auto & i : ld )
      {
        if ( e.is_alive_after_move( i ) )
        {
          dir = i;
          break_out = true;
          break;
        }
      }
      assert( break_out );
      cr[ e.cur_head.y ][ e.cur_head.x ] = make_pair( true, dir );
      cr[ e.cur_head( dir ).y ][ e.cur_head( dir ).x ] = make_pair( true, dir.reverse( ) );
      update_route( );
    }
    else if ( cr[ f.y ][ f.x ].first == false )
    {
      coord i( e.cur_head );
      size_t min_distance = coord::distance( i, f );
      coord closest_coord = i;
      i = i( cr[ e.cur_head.y ][ e.cur_head.x ].second );
      while ( e.cur_head != i )
      {
        size_t dis = coord::distance( i, f );
        if ( dis < min_distance )
        {
          min_distance = dis;
          closest_coord = i;
        }
        assert( cr[ i.y ][ i.x ].first );
        i = i( cr[ i.y ][ i.x ].second );
      }
      if ( closest_coord.is_on_same_line( f ) )
      {
        if( ! extend_thin_rect( closest_coord, f ) ) { assert( false ); }
      }
      else
      {
        assert( false );
      }
    }
  }

  bool cyclic_route::extend_thin_rect(const coord & c1, const coord c2)
  {
    if ( c1.is_on_same_line( c2 ) )
    {
      direction c1_to_c2( c1.get_heading( c2 ) );
      auto & c1_r = cr[ c1.y ][ c1.x ];
      assert( c1_r.first == true );
      if ( c1_r.second == c1_to_c2 || c1_r.second == c1_to_c2.reverse( ) )
      {
        direction t = c1_to_c2.tilt( ), t_r = c1_to_c2.tilt( ).reverse( );
        if ( ( e.is_in_map_after_move( c1, t ) &&
               ( cr[ c1( t ).y ][ c1( t ).x ].first == true ) &&
               ( cr[ c1( t ).y ][ c1( t ).x ].second == t_r ) &&
               extend_rect( c1( t ), c2( t ), c2, c1 ) ) ||
             ( e.is_in_map_after_move( c1, t_r ) &&
               ( cr[ c1( t_r ).y ][ c1( t_r ).x ].first == true ) &&
               ( cr[ c1( t_r ).y ][ c1( t_r ).x ].second == t ) &&
               extend_rect( c1( t_r ), c2( t_r ), c2, c1 ) )
             )
        {
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return extend_rect( c1, c2, c2( c1_r.second ), c1( c1_r.second ) );
      }
    }
    else
    {
      return extend_rect( c1, coord( c1.y, c2.x ), c2, coord( c2.y, c1.x ) ) || extend_rect( c1, coord( c2.y, c1.x ), c2, coord( c1.y, c2.x ) );
    }
  }

  direction cyclic_route::get_dir( )
  {
    if( need_update( ) ) { update_route( ); }
    assert( ! need_update( ) );
    assert( have( e.cur_head ) );
    return ( * this )[ e.cur_head ];
  }

  bool cyclic_route::extend_rect(const coord & c1, const coord c2, const coord & c3, const coord & c4)
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
    cr[ c4.y ][ c4.x ].first = false;
    auto dc12( coord::distance( c1, c2 ) ), dc23( coord::distance( c2, c3 ) ), dc34( coord::distance( c3, c4 ) );
    pair< bool, path > c1_to_c2( get_path( c1, c2, 0 ) );
    pair< bool, path > c2_to_c3( get_path( c2, c3, dc12 ) );
    pair< bool, path > c3_to_c4( get_path( c3, c4, dc12 + dc23 ) );
    pair< bool, path > c4_to_c1( get_path( c4, c1, dc12 + dc23 + dc34 ) );
    if( c1_to_c2.first &&
        c2_to_c3.first &&
        c3_to_c4.first &&
        c4_to_c1.first )
    {
      update_route( c1_to_c2.second );
      update_route( c2_to_c3.second );
      update_route( c3_to_c4.second );
      update_route( c4_to_c1.second );
      return true;
    }
    cr[ c1.y ][ c1.x ].first = true;
    cr[ c4.y ][ c4.x ].first = true;
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

  pair< bool, path > cyclic_route::get_path(const coord & from, const coord & to, size_t min_path_size, size_t max_path_size, size_t time) const
  {
    return get_path( from, to, min_path_size, max_path_size, set< coord >( ), time );
  }

  pair< bool, path > cyclic_route::get_path(const coord & from, const coord & to, size_t time) const
  {
      auto dis = coord::distance( from, to );
      return get_path( from, to, dis, dis, time );
  }

  pair< bool, path > cyclic_route::get_path( const coord & from, const coord & to ) const
  {
    auto dis = coord::distance( from, to );
    return get_path( from, to, dis, dis, 0 );
  }

  pair< bool, path > cyclic_route::get_path(const coord & from, const coord & to, size_t min_path_size, size_t max_path_size, const set<coord> & occupied, size_t time) const
  {
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
        if ( e.can_pass_to( from, up, time ) )
        {
          pair< bool,  path > up_res( get_path( coord( from( up ) ), to, min_path_size - 1, max_path_size - 1, new_occupied, time + 1 ) );
          if ( up_res.first )
          {
            up_res.second.push_front( up );
            return up_res;
          }
        }
        if ( e.can_pass_to( from, down, time ) )
        {
          pair< bool,  path > down_res( get_path( coord( from( down ) ), to, min_path_size - 1, max_path_size - 1, new_occupied, time + 1 ) );
          if ( down_res.first )
          {
            down_res.second.push_front( down );
            return down_res;
          }
        }
        if ( e.can_pass_to( from, left, time ) )
        {
          pair< bool,  path > left_res( get_path( coord( from( left ) ), to, min_path_size - 1, max_path_size - 1, new_occupied, time + 1 ) );
          if ( left_res.first )
          {
            left_res.second.push_front( left );
            return left_res;
          }
        }
        if ( e.can_pass_to( from, right, time ) )
        {
          pair< bool,  path > right_res( get_path( coord( from( right ) ), to, min_path_size - 1, max_path_size - 1, new_occupied, time + 1 ) );
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
}
