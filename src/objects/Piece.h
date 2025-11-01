#ifndef PIECE_HPP
#define PIECE_HPP

#include <stdint.h>
#include <iostream>

using namespace std;

class Piece
{
public:
  string id;
  string name;
  int amount;

  Piece() {};

  Piece(string id, string name, int amount) : id(id), name(name), amount(amount) {}
};

#endif // PIECE_HPP