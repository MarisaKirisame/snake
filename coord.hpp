#pragma once
#include <cassert>
#include <limits>
#include <tuple>
#include "direction.hpp"
using namespace std;
namespace snake
{
  struct coord
  {
    size_t y;
    size_t x;
    bool operator == ( const coord & comp_to ) const { return ( y == comp_to.y ) && ( x == comp_to.x ); }
    bool operator != ( const coord & comp_to ) const { return ( y != comp_to.y ) || ( x != comp_to.x ); }
    bool operator < ( const coord & comp_to ) const { return ( y < comp_to.y ) || ( ( y == comp_to.y ) && ( x < comp_to.x ) ); }
    coord( size_t y, size_t x ) : y( y ), x( x ) { }
    coord( const coord & ) = default;
    coord( ) { }
    coord operator ( ) ( direction dir ) const;
    bool is_on_same_line( const coord & c ) const { return ( c.x == x ) || ( c.y == y ); }
    direction get_heading( const coord & ) const;
    static size_t distance( const coord & x, const coord & y )
    { return max( x.y, y.y ) - min( x.y, y.y ) + max( x.x, y.x ) - min( x.x, y.x ); }
  };

  coord coord::operator ( )( direction dir ) const
  {
    switch ( dir.d )
    {
    case up:
      assert( y > 0 );
      return coord( y - 1, x );
    case down:
      assert( y < numeric_limits< size_t >::max( ) );
      return coord( y + 1, x );
    case left:
      assert( x > 0 );
      return coord( y, x -1 );
    default:
      assert( dir == right );
      assert( x < numeric_limits< size_t >::max( ) );
      return coord( y, x + 1 );
    }
  }

  direction coord::get_heading( const coord & c ) const
  {
    if ( y == c.y && x > c.x )
    {
      return left;
    }
    else if ( y == c.y && x < c.x )
    {
      return right;
    }
    else if ( x == c.x && y > c.y )
    {
      return up;
    }
    else
    {
      assert( x == c.x );
      assert( y < c.y );
      return down;
    }
  }

}
