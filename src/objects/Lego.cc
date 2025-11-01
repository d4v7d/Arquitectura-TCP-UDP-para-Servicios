#include "Lego.h"

// Constructor
Lego::Lego() {
  // Inicializa los vectores
  pieces = vector<string>();
  half_01 = vector<string>();
  half_02 = vector<string>();
}

Lego::Lego(string id, string name, int pieces_count) : id(id), name(name), pieces_count(pieces_count) {
  // Inicializa los vectores
  pieces = vector<string>();
  half_01 = vector<string>();
  half_02 = vector<string>();
};

Lego::Lego(string id, string name, int pieces_count, vector<string> pieces) : id(id), name(name), pieces_count(pieces_count), pieces(pieces) {
  // Inicializa los vectores
  half_01 = vector<string>();
  half_02 = vector<string>();
};

// Destructor
Lego::~Lego() {
  pieces.clear();
  half_01.clear();
  half_02.clear();
};

/**
 * @brief Almacena la informacion de las piezas en una lista.
 *
 * @param data La data en texto de la pieza a agregar
 * @param half mitad a la cual pertenece
 */
void Lego::add_piece(string data, int half) {
  // Store piece in pieces list
  this->pieces.push_back(data);

  // Decide the current half in which the piece will be sorted
  if (half == 1) {
    this->half_01.push_back(data);
  } else {
    this->half_02.push_back(data);
  }
}

/**
 * @brief Se fija si una pieza está en la lista de piezas.
 *
 * @param id El numero de id de pieza a buscar.
 */
bool Lego::contains(string piece_data) {
  for (uint64_t i = 0; i < this->pieces.size(); i++) {
    if (this->pieces[i] == piece_data) {
      return true;
    }
  }

  return false;
}

/**
 * @brief Imprime el lego y las listas de piezas.
 */
void Lego::print() {
  cout << "Nombre: " << this->name << endl;
  cout << "Cantidad de Piezas: " << this->pieces_count << endl;

  cout << "Piezas: " << endl;
  for (uint64_t i = 0; i < this->pieces.size(); i++) {
    cout << this->pieces[i] << endl;
  }

  cout << endl;

  cout << "Mitad 01: " << endl;
  for (uint64_t i = 0; i < this->half_01.size(); i++) {
    cout << this->half_01[i] << endl;
  }

  cout << endl;
  cout << "Mitad 02: " << endl;

  for (uint64_t i = 0; i < this->half_02.size(); i++) {
    cout << this->half_02[i] << endl;
  }
}

string Lego::generateHTML() {
  stringstream ss;
  ss << "HTTP/1.1 200 OK\n";
  ss << "Content-Type: text/html\r\n\r\n";

  ss << "<html>\n";
  ss << "<head>\n";
  ss << "<title>" << this->name << "</title>\n";
  ss << "</head>\n";
  ss << "<body>\n";
  ss << "<h1>" << this->name << "</h1>\n";
  ss << "<h2>" << "Piezas: " << this->pieces_count << "</h2>\n";
  
  ss << "<ul>\n";
  for (const auto& piece : this->pieces) {
    ss << "<li>" << piece << "</li>\n";
  }
  ss << "</ul>\n";

  ss << "</body>\n";
  ss << "</html>\n";
  
  return ss.str();
}