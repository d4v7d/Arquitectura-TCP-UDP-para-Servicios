#include "JSONDecoder.h"

bool JSONDecoder::decode() {
  // Abrir el archivo JSON
  ifstream file(filepath);
  if (!file.is_open()) {
    cout << "El archivo solicitado no existe o no se pudo abrir." << endl;
    return false;
  }

  // Leer el contenido del archivo JSON en un objeto Json::Value
  file >> root;

  // Cerrar el archivo después de leer
  file.close();

  return true;
}

Lego JSONDecoder::getLegoFromJson() const {
  Lego lego;

  lego.id = root["id"].asString();
  lego.name = root["name"].asString();
  lego.pieces_count = root["total-pieces"].asInt();

  // Iterar sobre los elementos del objeto
  for (const auto& pieceHalf : root["pieces"]) {
    lego.add_piece(pieceHalf["m1"].asString(), 1);
    lego.add_piece(pieceHalf["m2"].asString(), 2);
  }

  return lego;
}

string JSONDecoder::getString(const string key) const {
  return root[key].asString();
}

int JSONDecoder::getInt(const string key) const { return root[key].asInt(); }