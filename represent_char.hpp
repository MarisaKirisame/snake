#pragma once
namespace snake
{
  struct representing_char
  {
    char wall;
    char snake;
    char food;
    char head;
    char tail;
    representing_char( ) { }
    representing_char( char wall, char snake, char food, char head, char tail ) : wall( wall ), snake( snake ), food( food ), head( head ), tail( tail ) { }
  };
}
