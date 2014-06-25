BOOST_DIR=/usr/local/include/boost
SRC_DIR=src/
LIBRARIES=-lboost_program_options -lboost_system -lboost_filesystem -lboost_serialization
BUILD_DIR=obj
EXECUTABLES=locate updatedb
FLAGS=-Wl,-rpath=/usr/local/lib

all: $(BUILD_DIR) $(EXECUTABLES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/locate.o: $(SRC_DIR)locate.cpp
	g++ $(FLAGS) -c $< -o $@

$(BUILD_DIR)/update_db.o: $(SRC_DIR)updatedb.cpp
	g++ $(FLAGS) -c $< -o $@

locate: $(BUILD_DIR)/locate.o
	g++ $(FLAGS) -I $(BOOST_DIR) $(BUILD_DIR)/locate.o -o $@ $(LIBRARIES)
updatedb: $(BUILD_DIR)/update_db.o
	g++ $(FLAGS) -I $(BOOST_DIR) $(BUILD_DIR)/update_db.o -o $@ $(LIBRARIES)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(EXECUTABLES)
