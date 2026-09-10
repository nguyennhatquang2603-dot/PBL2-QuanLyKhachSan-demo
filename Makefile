# Makefile - web server Quan Ly Khach San (PBL2)
# Dung: mingw32-make   hoac   make

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -static -static-libgcc -static-libstdc++
LDLIBS   := -lws2_32 -lwsock32
SRC      := server/main.cpp server/store.cpp
DEPS     := server/store.h server/models.h server/json.h third_party/httplib.h
BIN      := hotelweb.exe

$(BIN): $(SRC) $(DEPS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(LDLIBS)

.PHONY: run clean
run: $(BIN)
	./$(BIN)

clean:
	-rm -f $(BIN)
