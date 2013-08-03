#pragma once
#include "point.hpp"
#include "snake_piece.hpp"
namespace snake
{
  struct space : point
  {
    shared_ptr< point > obj;
    bool can_pass( ) const;
    size_t can_pass_after( ) const;
    char representing_char( ) const;
    space( env & e, coord && p ) : point( e, move( p ) ) { e.s_space.insert( p ); }
    ~space( ) noexcept( true ) { }
    void pass( shared_ptr< point > sp );
    void leave( );
    shared_ptr< point > get_object_on_top( ) const { return obj; }
    shared_ptr< point > get_snake( ) const;
  };

  bool space::can_pass( ) const
  {
    if ( obj.get( ) == nullptr ) { return true; }
    else { return obj->can_pass( ); }
  }

  size_t space::can_pass_after( ) const
  {
    if ( obj.get( ) == nullptr ) { return 0; }
    else { return obj->can_pass_after( ); }
  }

  char space::representing_char( ) const
  {
    if ( obj.get( ) == nullptr ) { return ' '; }
    else { return obj->representing_char( ); }
  }

  void space::pass( shared_ptr< point > sp )
  {
    assert( can_pass( ) );
    if ( obj.get( ) == nullptr )
    { obj = sp;
      point::e.s_space.erase( point::p ); }
    else { obj->pass( sp ); }
  }

  void space::leave( )
  {
      obj->leave( );
      e.s_space.insert( p );
      obj.reset( );
  }

  shared_ptr< point > space::get_snake( ) const
  {
    if ( obj.get( ) == nullptr ) { return nullptr; }
    else if ( dynamic_cast< snake_piece * >( obj.get( ) ) != nullptr ) { return obj; }
    else { return obj->get_snake( ); }
  }

}
