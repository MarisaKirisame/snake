#pragma once
//#include "point.hpp"
//#include "snake_piece.hpp"
namespace snake
{
  struct space// : point
  {
    /*shared_ptr< point > obj;
    bool can_pass( ) const
    {
      if ( obj.get( ) == nullptr ) { return true; }
      else { return obj->can_pass( ); }
    }
    size_t can_pass_after( ) const
    {
      if ( obj.get( ) == nullptr ) { return 0; }
      else { return obj->can_pass_after( ); }
    }
    char representing_char( ) const
    {
      if ( obj.get( ) == nullptr ) { return ' '; }
      else { return obj->representing_char( ); }
    }
    space( env & e, coord && p ) : point( e, move( p ) )
    {
      ++e.space_left;
      e.s_space.insert( p );
    }
    ~space( ) noexcept( true ) { --e.space_left; }
    void pass( shared_ptr< point > sp )
    {
      assert( can_pass( ) );
      if ( obj.get( ) == nullptr )
      {
        obj = sp;
        --e.space_left;
        point::e.s_space.erase( point::p );
      }
      else { obj->pass( sp ); }
    }
    void leave( )
    {
      obj->leave( );
      ++e.space_left;
      e.s_space.insert( p );
      obj.reset( );
    }
    shared_ptr< point > get_object_on_top( ) const { return obj; }
    shared_ptr< point > get_snake( ) const
    {
      if ( dynamic_cast< snake_piece * >( obj.get( ) ) == nullptr ) { return obj->get_snake( ); }
      else { return obj; }
    }*/
  };
}
