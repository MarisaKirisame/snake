#include "point.hpp"
#include "snake_piece.hpp"
namespace snake
{
  struct food : point
  {
    shared_ptr< point > ss;
    food( env & e, coord && p ) : point( e, move( p ) ) { e.s_food.insert( p ); }
    ~food( ) { }
    bool can_pass( ) const;
    size_t can_pass_after( ) const;
    char representing_char( ) const;
    void pass( shared_ptr< point > sp );
    void leave( );
    shared_ptr< point > get_object_on_top( ) const { return ss; }
    shared_ptr< point > get_snake( ) const;
    void pass( ) { if ( ss.get( ) != nullptr ) { ss->pass( ); } }
  };

  bool food::can_pass() const
  { if ( ss.get( ) == nullptr ) { return true; }
    else { return ss->can_pass( ); }
  }

  size_t food::can_pass_after() const
  { if ( ss.get( ) == nullptr ) { return 0; }
    else { return ss->can_pass_after( ); } }

  char food::representing_char() const
  { if ( ss.get( ) == nullptr ) { return e.rc.food; }
    else { return ss->representing_char( ); } }

  void food::pass( shared_ptr< point > sp )
  {
    assert( ss.get( ) == nullptr );
    ss = sp;
    ++boost::polymorphic_cast< snake_piece * >( sp.get( ) )->life;
    ++point::e.food_in_snake;
    point::e.s_food.erase( point::p );
    e.gen_food( 1 );
  }

  void food::leave( )
  {
      --point::e.food_in_snake;
      ss->leave( );
      ss.reset( );
  }

  shared_ptr< point > food::get_snake( ) const
  { if ( dynamic_cast< snake_piece * >( ss.get( ) ) == nullptr ) { return ss->get_snake( ); }
    else { return ss; } }

}
