#pragma once
#include <cassert>
#include <list>
using namespace std;
namespace snake
{

  struct direction
  {
    enum dir
    {
      up,down,left,right
    }d;
    direction( dir d ) : d( d ) { }
    direction( const direction & d ) : d( d.d ) { }
    direction( ) { }
    operator dir ( ) const { return d; }
    direction tilt( ) const
    {
      switch ( d )
      {
      case up:
        return left;
      case down:
        return right;
      case left:
        return down;
      default:
        assert( d == right );
        return up;
      }
    }
    direction reverse( ) const
    {
      switch ( d )
      {
      case up:
        return down;
      case down:
        return up;
      case left:
        return right;
      default:
        assert( d == right );
        return left;
      }
    }
  };
  constexpr direction::dir up = direction::up;
  constexpr direction::dir down = direction::down;
  constexpr direction::dir left = direction::left;
  constexpr direction::dir right = direction::right;

}

