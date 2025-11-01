#ifndef JSONDECODER_H
#define JSONDECODER_H

#include <fstream>
#include <iostream>

#include "../../include/json/json.h"
#include "Lego.h"

using namespace std;

class JSONDecoder {
 private:
  string filepath;
  Json::Value root;

 public:
  JSONDecoder(const string filepath) : filepath(filepath) {}

  bool decode();
  Lego getLegoFromJson() const;
  string getString(const string key) const;
  int getInt(const string key) const;
};

#endif  // JSONDecoder.h
