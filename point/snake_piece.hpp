#pragma once
#include "point.hpp"
namespace snake
{

  struct snake_piece : point
  {
    size_t life;
    snake_piece( size_t life, env & e, coord && p );
    snake_piece( size_t life, env & e, const coord & p );
    ~snake_piece( ) noexcept( true ) { point::e.s_snake.erase( point::p ); }
    bool can_pass( ) const { return false; }
    char representing_char( ) const;
    size_t can_pass_after( ) const { return life; }
    void leave( ) { }
    void pass( )
    {
      --life;
      if ( life == 0 )
      {
        point::e.vec[ point::p.y ][ point::p.x ]->leave( );
      }
    }
  };

  snake_piece::snake_piece( size_t life, env & e, coord && p ) : point( e, move( p ) ), life( life )
  { assert( life > 0 );
    e.s_snake.insert( p ); }

  snake_piece::snake_piece( size_t life, env & e, const coord & p ) : point( e, p ), life( life )
  { assert( life > 0 );
    e.s_snake.insert( p ); }

  char snake_piece::representing_char() const
  {
    assert( life > 0 );
    if ( point::p == point::e.cur_head )
    { return point::e.rc.head; }
    else if ( life == 1 ) { return point::e.rc.tail; }
    else { return e.rc.snake; }
  }

}
