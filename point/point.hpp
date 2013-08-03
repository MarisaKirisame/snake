#pragma once
#include <memory>
#include "../coord.hpp"
using namespace std;
namespace snake
{
  class env;
  struct point
  {
    virtual void pass( ) = 0;
    virtual bool can_pass( ) const = 0;
    virtual char representing_char( ) const = 0;
    virtual ~point( ) { }
    virtual void pass( shared_ptr< point > ) { assert( false ); }
    virtual void leave( ) { }
    virtual shared_ptr< point > get_object_on_top( ) const { assert( false ); }
    virtual shared_ptr< point > get_snake( ) const { assert( false ); }
    virtual size_t can_pass_after( ) const = 0;
    bool can_pass_after( size_t time ) const
    { return can_pass_after( ) <= time; }
    point( env & e, coord && p ) : e( e ), p( p ) { }
    point( env & e, const coord & p ) : e( e ), p( p ) { }
    env & e;
    coord p;
  };
}
