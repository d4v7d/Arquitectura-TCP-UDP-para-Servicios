# Directorios de origen
SRC_DIR := ./src
INCLUDE_DIR := ./include
MISC_DIR := $(SRC_DIR)/miscellaneous
NETWORK_DIR := $(SRC_DIR)/network
OBJECTS_DIR := $(SRC_DIR)/objects

# Directorio de salida para binarios
BIN_DIR := ./bin

# Compilador y flags
CXX := g++
CFLAGS := -g -c -I$(INCLUDE_DIR)/json -I$(INCLUDE_DIR)
LDFLAGS := -lssl -lcrypto

# Archivos de origen
SRC_CC := $(wildcard $(SRC_DIR)/*.cc)
MISC_CC := $(wildcard $(MISC_DIR)/*.cc)
NETWORK_CC := $(wildcard $(NETWORK_DIR)/*.cc)
OBJECTS_CC := $(wildcard $(OBJECTS_DIR)/*.cc)

# Objetos generados
OBJ := $(patsubst $(SRC_DIR)/%.cc,$(BIN_DIR)/%.o,$(SRC_CC))
OBJ += $(patsubst $(MISC_DIR)/%.cc,$(BIN_DIR)/%.o,$(MISC_CC))
OBJ += $(patsubst $(NETWORK_DIR)/%.cc,$(BIN_DIR)/%.o,$(NETWORK_CC))
OBJ += $(patsubst $(OBJECTS_DIR)/%.cc,$(BIN_DIR)/%.o,$(OBJECTS_CC))

# Objetos específicos
SERVER_OBJ := $(BIN_DIR)/server.o
CLIENT_OBJ := $(BIN_DIR)/client.o
INTERMEDIATE_OBJ := $(BIN_DIR)/intermediate.o

# Bibliotecas
LIBS := $(INCLUDE_DIR)/libjsoncpp.a

# Targets
TARGETS := $(BIN_DIR)/ipv4-ssl-cli.out $(BIN_DIR)/ipv6-ssl-cli.out $(BIN_DIR)/simulacion-protocolo.out $(BIN_DIR)/server.out $(BIN_DIR)/client.out $(BIN_DIR)/intermediate.out

# Dependencias
DEPS := $(NETWORK_DIR)/Socket.h $(OBJECTS_DIR)/JSONDecoder.h

# Crea la carpeta ./bin
BIN_DIR_GUARD = @mkdir -p $(@D)

# Regla principal
all: $(OBJ) $(SERVER_OBJ) $(CLIENT_OBJ) $(INTERMEDIATE_OBJ) $(TARGETS)

# Regla para compilar los archivos .cc en objetos
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cc $(DEPS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/%.o: $(MISC_DIR)/%.cc $(DEPS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/%.o: $(NETWORK_DIR)/%.cc $(DEPS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/%.o: $(OBJECTS_DIR)/%.cc $(DEPS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(SERVER_OBJ): $(SRC_DIR)/server.cc $(DEPS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(CLIENT_OBJ): $(SRC_DIR)/client.cc $(DEPS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(INTERMEDIATE_OBJ): $(SRC_DIR)/intermediate.cc $(DEPS)
	$(BIN_DIR_GUARD)
	$(CXX) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Reglas para generar los .out de los targets
$(BIN_DIR)/ipv4-ssl-cli.out: $(BIN_DIR)/ipv4-ssl-cli.o $(BIN_DIR)/VSocket.o $(BIN_DIR)/Socket.o $(BIN_DIR)/SSLSocket.o $(LIBS)
	$(BIN_DIR_GUARD)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/ipv6-ssl-cli.out: $(BIN_DIR)/ipv6-ssl-cli.o $(BIN_DIR)/VSocket.o $(BIN_DIR)/Socket.o $(BIN_DIR)/SSLSocket.o $(LIBS)
	$(BIN_DIR_GUARD)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/simulacion-protocolo.out: $(BIN_DIR)/simulacion-protocolo.o $(BIN_DIR)/VSocket.o $(BIN_DIR)/Socket.o $(BIN_DIR)/SSLSocket.o $(LIBS)
	$(BIN_DIR_GUARD)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/server.out: $(BIN_DIR)/server.o $(BIN_DIR)/JSONDecoder.o $(BIN_DIR)/Lego.o $(BIN_DIR)/VSocket.o $(BIN_DIR)/Socket.o $(BIN_DIR)/SSLSocket.o $(LIBS)
	$(BIN_DIR_GUARD)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/client.out: $(BIN_DIR)/client.o $(BIN_DIR)/VSocket.o $(BIN_DIR)/Socket.o $(BIN_DIR)/SSLSocket.o $(LIBS)
	$(BIN_DIR_GUARD)
	$(CXX) $^ $(LDFLAGS) -o $@

$(BIN_DIR)/intermediate.out: $(BIN_DIR)/intermediate.o $(BIN_DIR)/VSocket.o $(BIN_DIR)/Socket.o $(BIN_DIR)/SSLSocket.o $(LIBS)
	$(BIN_DIR_GUARD)
	$(CXX) $^ $(LDFLAGS) -o $@

# Limpieza de archivos generados
clean:
	rm -rf $(BIN_DIR)

compile:
	make clean; make all;

run-ipv6:
	make compile; ./bin/ipv6-ssl-cli.out 1

run-ipv4:
	make compile; ./bin/ipv4-ssl-cli.out 1

run-simulacion:
	make compile; ./bin/ipv4-ssl-cli.out; ./bin/simulacion-protocolo.out

run-simulacion-ssl:
	make compile; ./bin/ipv4-ssl-cli.out 1; ./bin/simulacion-protocolo.out

run-server:
	make compile; ./bin/server.out

run-client:
	make compile; ./bin/client.out

run-intermediate:
	make compile; ./bin/intermediate.out

run-intermediate-test-case-1:
	make compile; ./bin/intermediate.out 1
