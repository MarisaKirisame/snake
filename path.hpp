#pragma once
#include <list>
#include "coord.hpp"
#include "direction.hpp"
using namespace std;
namespace snake
{
  struct path
  {
    path( coord && head ) : head( head ) { }
    path( const coord & head ) : head( head ) { }
    path( path && p ) : head( move( p.head ) ),
      dir( move( p.dir ) ) { }
    path( ) { }
    path( const path & ) = default;
    coord head;
    list< direction > dir;
    size_t size( ) const { return dir.size( ); }
    void push_front( const direction & _dir );
    void push_back( const direction & _dir ) { dir.push_back( _dir ); }
  };

  void path::push_front( const direction & _dir )
  { dir.push_front( _dir );
    head = head( _dir.reverse( ) ); }

}
