#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <cassert>
#include <vector>
#include <memory>
#include <set>
#include <boost/cast.hpp>
#include <boost/concept_check.hpp>
#include <random>
using namespace std;
namespace snake
{
  struct env
  {
    typedef pair< size_t, size_t > coord;

    struct representing_char
    {
      char wall;
      char snake;
      char food;
      representing_char( ) { }
      representing_char( char wall, char snake, char food ) : wall( wall ), snake( snake ), food( food ) { }
    };

    enum direction
    {
      up, down, left, right
    };

    direction get_dir( ) const
    {
      if ( s_food.empty( ) )
      {
        if ( is_safe_after_move( down ) )
        {
          return down;
        }
        else if ( is_safe_after_move( up ) )
        {
          return up;
        }
        else if ( is_safe_after_move( left ) )
        {
          return left;
        }
        else if ( is_safe_after_move( right ) )
        {
          return right;
        }
        if ( is_alive_after_move( down ) )
        {
          return down;
        }
        else if ( is_alive_after_move( up ) )
        {
          return up;
        }
        else if ( is_alive_after_move( left ) )
        {
          return left;
        }
        else
        {
          return right;
        }
      }
      else
      {
        auto f_pos = * s_food.begin( );
        if ( f_pos.first > cur_head.first )
        {
          if ( is_safe_after_move( down ) )
          {
            return down;
          }
        }
        else if ( f_pos.first < cur_head.first )
        {
          if ( is_safe_after_move( up ) )
          {
            return up;
          }
        }
        if ( f_pos.second > cur_head.second )
        {
          if ( is_safe_after_move( right ) )
          {
            return right;
          }
        }
        else if ( f_pos.second < cur_head.second )
        {
          if ( is_safe_after_move( left ) )
          {
            return left;
          }
        }
        if ( is_safe_after_move( down ) )
        {
          return down;
        }
        else if ( is_safe_after_move( up ) )
        {
          return up;
        }
        else if ( is_safe_after_move( left ) )
        {
          return left;
        }
        else if ( is_safe_after_move( right ) )
        {
          return right;
        }
        if ( f_pos.first > cur_head.first )
        {
          if ( is_alive_after_move( down ) )
          {
            return down;
          }
        }
        else if ( f_pos.first < cur_head.first )
        {
          if ( is_alive_after_move( up ) )
          {
            return up;
          }
        }
        if ( f_pos.second > cur_head.second )
        {
          if ( is_alive_after_move( right ) )
          {
            return right;
          }
        }
        else if ( f_pos.second < cur_head.second )
        {
          if ( is_alive_after_move( left ) )
          {
            return left;
          }
        }
        if ( is_alive_after_move( down ) )
        {
          return down;
        }
        else if ( is_alive_after_move( up ) )
        {
          return up;
        }
        else if ( is_alive_after_move( left ) )
        {
          return left;
        }
        else
        {
          return right;
        }
      }
    }

    size_t score( )
    {
      return s_snake.size( ) - 1;
    }

    env( ) : is_snake_alive( false ) { }
    env( size_t length, size_t width, representing_char && rc, size_t food_num, list< coord >&& snake )
    { reset( length, width, move( rc ), food_num, move( snake ) ); }

    void reset( size_t length, size_t width, representing_char && rc, size_t food_num, list< coord >&& snake )
    {
      this->rc = rc;
      srand( time( 0 ) );
      vec.clear( );
      is_snake_alive = true;
      assert( length > 0 );
      assert( width > 0 );
      space_left = 0;
      cur_map_length = length;
      cur_map_width = width;
      vec.resize( length );
      size_t cur_length = 0;
      for ( auto & i : vec )
      {
        i.resize( width );
        size_t cur_width = 0;
        for ( auto & ii : i )
        {
          ii = shared_ptr< point >( new space( * this, move( make_pair( cur_length, cur_width ) ) ) );
          ++cur_width;
        }
        ++cur_length;
      }
      cur_head = snake.front( );
      size_t life = snake.size( );
      assert( life > 0 );
      for ( auto & i : snake )
      {
        vec[ i.first ][ i.second ]->pass( shared_ptr< point >( new snake_piece( life, * this, move( i ) ) ) );
        --life;
      }
      assert( life == 0 );
      gen_food( food_num );
    }

    bool is_game_over( ) const { return ! is_snake_alive; }
    bool is_in_map_after_move( direction dir ) const
    {
      switch ( dir )
      {
      case up:
        return cur_head.first != 0;
      case down:
        return cur_head.first + 1 < cur_map_length;
      case left:
        return cur_head.second != 0;
      case right:
        return cur_head.second + 1 < cur_map_width;
      default:
        assert( false );
      }
    }

    coord get_coord( direction dir ) const
    {
      assert( is_in_map_after_move( dir ) );
      coord next = cur_head;
      switch ( dir )
      {
      case up:
        --next.first;
        break;
      case down:
        ++next.first;
        break;
      case left:
        --next.second;
        break;
      case right:
        ++next.second;
        break;
      default:
        assert( false );
      }
      return next;
    }

    enum point_status
    {
      connected_to_others, not_coonected_to_others_yet, wall
    };
    bool is_alive_after_move( direction dir ) const
    {
      if ( is_in_map_after_move( dir ) )
      {
        auto next = get_coord( dir );
        return vec[ next.first ][ next.second ]->can_pass( );
      }
      else
      {
        return false;
      }
    }

    static size_t distance( const coord & x, const coord & y )
    {
      return max( x.first, y.first ) - min( x.first, y.first ) + max( x.second, y.second ) - min( x.second, y.second );
    }

    bool is_safe_after_move( direction dir ) const
    {
      if ( is_alive_after_move( dir ) )
      {
        auto next = get_coord( dir );
        vector< vector< point_status > > map( cur_map_length, vector< point_status >( cur_map_width ) );
        for ( size_t i = 0; i < cur_map_length; ++i )
        {
          for ( size_t ii = 0; ii < cur_map_width; ++ii )
          {
            if ( next == make_pair( i, ii ) ) { map[ i ][ ii ] = wall; }
            else if ( vec[ i ][ ii ]->can_pass_after( distance( make_pair( i, ii ), next ) - 1 ) )
            {
              if ( distance( make_pair( i, ii ), next ) == 1 )
              {
                map[ i ][ ii ] = connected_to_others;
              }
              else { map[ i ][ ii ] = not_coonected_to_others_yet; }
            }
            else { map[ i ][ ii ] = wall; }
          }
        }
        bool still_calculating = true;
        while ( still_calculating )
        {
          still_calculating = false;
          for ( size_t i = 0; i < cur_map_length; ++i )
          {
            for ( size_t ii = 0; ii < cur_map_width; ++ii )
            {

              if ( ( ! vec[ i ][ ii ]->can_pass( ) && vec[ i ][ ii ]->can_pass_after( distance( make_pair( i, ii ), next ) - 1 ) ) &&
                        ( ( i > 0 && map[ i - 1 ][ ii ] == connected_to_others ) ||
                          ( i + 1 < cur_map_length && map[ i +1 ][ ii ] == connected_to_others ) ||
                          ( ii > 0 && map[ i ][ ii - 1 ] == connected_to_others ) ||
                          ( ii + 1 < cur_map_width && map[ i ][ ii + 1 ] == connected_to_others ) ) )
              {
                return true;
              }
              else if ( ( map[ i ][ ii ] == not_coonected_to_others_yet ) &&
                        ( ( i > 0 && map[ i - 1 ][ ii ] == connected_to_others ) ||
                          ( i + 1 < cur_map_length && map[ i +1 ][ ii ] == connected_to_others ) ||
                          ( ii > 0 && map[ i ][ ii - 1 ] == connected_to_others ) ||
                          ( ii + 1 < cur_map_width && map[ i ][ ii + 1 ] == connected_to_others ) ) )
              {
                map[ i ][ ii ] = connected_to_others;
                still_calculating = true;
              }
            }
          }
        }
        for ( size_t i = 0; i < cur_map_length; ++i )
        {
          for ( size_t ii = 0; ii < cur_map_width; ++ii )
          {
            if ( map[ i ][ ii ] == not_coonected_to_others_yet )
            {
              return false;
            }
          }
        }
        return true;
      }
      else
      {
        return false;
      }
    }

    void move_snake( direction dir )
    {
      assert( is_snake_alive );
      if ( ! is_alive_after_move( dir ) )
      {
        is_snake_alive = false;
      }
      else
      {
        coord next = get_coord( dir );
        auto next_square = vec[ next.first ][ next.second ];
        auto cur_square = vec[ cur_head.first ][ cur_head.second ];
        size_t previous_space = space_left;
        next_square->pass( shared_ptr< snake_piece >( new snake_piece
                                                      ( boost::polymorphic_cast< snake_piece * >( cur_square->get_snake( ).get( ) )->life + 1, * this, move( next ) ) ) );
        list< coord > zombie_snake_piece;
        for ( auto & i : s_snake )
        {
          size_t & life = boost::polymorphic_cast< snake_piece * >( vec[ i.first ][ i.second ]->get_snake( ).get( ) )->life;
          --life;
          if ( life == 0 ) { zombie_snake_piece.push_back( i ); }
        }
        for ( auto & i : zombie_snake_piece )
        {
          assert( boost::polymorphic_cast< snake_piece * >( vec[ i.first ][ i.second ]->get_snake( ).get( ) )->life == 0 );
          vec[ i.first ][ i.second ]->leave( );
        }
        cur_head = next;
        assert( previous_space >= space_left );
        assert( space_left +1 >= previous_space );
      }
    }
  private:
    size_t space_left;
    bool is_snake_alive;
    coord cur_head;
    size_t cur_map_length;
    size_t cur_map_width;
    representing_char rc;
    struct point
    {
      virtual bool can_pass( ) const = 0;
      virtual char representing_char( ) const = 0;
      virtual ~point( ) { }
      virtual void pass( shared_ptr< point > ) { assert( false ); }
      virtual void leave( ) { assert( false ); }
      virtual shared_ptr< point > get_object_on_top( ) const { assert( false ); }
      virtual shared_ptr< point > get_snake( ) const { assert( false ); }
      virtual bool can_pass_after( size_t time ) const = 0;
      point( env & e, coord && p ) : e( e ), p( p ) { }
      env & e;
      coord p;
    };

    struct space : point
    {
      shared_ptr< point > obj;
      bool can_pass( ) const
      {
        if ( obj.get( ) == nullptr ) { return true; }
        else { return obj->can_pass( ); }
      }
      bool can_pass_after( size_t time ) const
      {
        if ( obj.get( ) == nullptr ) { return true; }
        else { return obj->can_pass_after( time ); }
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
        if ( obj->can_pass( ) ) { obj->leave( ); }
        ++e.space_left;
        e.s_space.insert( p );
        obj.reset( );
      }
      shared_ptr< point > get_object_on_top( ) const { return obj; }
      shared_ptr< point > get_snake( ) const
      {
        if ( dynamic_cast< snake_piece * >( obj.get( ) ) == nullptr ) { return obj->get_snake( ); }
        else { return obj; }
      }
    };

    struct snake_piece : point
    {
      size_t life;

      snake_piece( unsigned int life, env & e, coord && p ) : point( e, move( p ) ), life( life )
      {
        assert( life > 0 );
        e.s_snake.insert( p );
      }

      ~snake_piece( ) noexcept( true ) { point::e.s_snake.erase( point::p ); }

      bool can_pass( ) const { return false; }
      char representing_char( ) const { return e.rc.snake; }
      bool can_pass_after( size_t time ) const
      {
        return time >= life;
      }
    };

    struct food : point
    {
      shared_ptr< point > ss;
      food( env & e, coord && p ) : point( e, move( p ) ) { e.s_food.insert( p ); }
      ~food( ) { }
      bool can_pass( ) const
      {
        if ( ss.get( ) == nullptr ) { return true; }
        else { return ss->can_pass( ); }
      }
      bool can_pass_after( size_t time ) const
      {
        if ( ss.get( ) == nullptr ) { return true; }
        else { return ss->can_pass_after( time ); }
      }
      char representing_char( ) const
      {
        if ( ss.get( ) == nullptr ) { return e.rc.food; }
        else { return ss->representing_char( ); }
      }
      void pass( shared_ptr< point > sp )
      {
        assert( ss.get( ) == nullptr );
        ss = sp;
        ++boost::polymorphic_cast< snake_piece * >( sp.get( ) )->life;
        point::e.s_food.erase( point::p );
        e.gen_food( 1 );
      }
      void leave( ) { ss.reset( ); }
      shared_ptr< point > get_object_on_top( ) const { return ss; }
      shared_ptr< point > get_snake( ) const
      {
        if ( dynamic_cast< snake_piece * >( ss.get( ) ) == nullptr ) { return ss->get_snake( ); }
        else { return ss; }
      }
    };

    void gen_food( size_t num )
    {
      assert( ! s_space.empty( ) );
      for ( size_t i = 0; i < num; ++i )
      {
        size_t dis = rand( ) % s_space.size( );
        auto it = s_space.begin( );
        advance( it, dis );
        assert( it != s_space.end( ) );
        auto pos = * it;
        vec[ pos.first ][ pos.second ]->pass( shared_ptr< point >( new food( * this, move( pos ) ) ) );
      }
    }

    set< coord > s_snake;
    set< coord > s_space;
    set< coord > s_food;
    vector< vector< shared_ptr< point > > > vec;

    friend ostream & operator << ( ostream & os, env & e )
    {
      assert( e.is_snake_alive );
      os << e.space_left << endl;
      os << string( e.cur_map_width + 2, e.rc.wall ) << endl;
      for( auto & i : e.vec )
      {
        assert( i.size( ) == size_t( e.cur_map_width ) );
        os << e.rc.wall;
        for ( auto & ii : i ) { os << ii->representing_char( ); }
        os << e.rc.wall;
        os << endl;
      }
      os << string( e.cur_map_width + 2, e.rc.wall ) << endl;
      return os;
    }
  };
}
