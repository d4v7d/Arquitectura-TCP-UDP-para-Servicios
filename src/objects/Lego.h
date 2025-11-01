#ifndef LEGO_HPP
#define LEGO_HPP

#include <stdint.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

#include "Piece.h"

using namespace std;

class Lego {
  private:
  size_t current_list = 1;

  public:
  string id;
  string name;
  int pieces_count;
  vector<string> pieces;
  vector<string> half_01;
  vector<string> half_02;

  // Constructor
  Lego();
  Lego(string id, string name, int pieces_count);
  Lego(string id, string name, int pieces_count, vector<string> pieces);

  // Destructor
  ~Lego();

  void add_piece(string data, int half);
  bool contains(string piece_data);
  void print();
  string generateHTML();
};

#endif
