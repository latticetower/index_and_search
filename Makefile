BOOST_DIR=/usr/local/include/boost
SRC_DIR=src/

all: locate updatedb

locate:
	g++ -I $(BOOST_DIR) -Wl,-rpath=/usr/local/lib $(SRC_DIR)locate.cpp -o locate -lboost_program_options -lboost_system -lboost_filesystem

updatedb:
	g++ -I $(BOOST_DIR) -Wl,-rpath=/usr/local/lib $(SRC_DIR)updatedb.cpp -o updatedb -lboost_program_options -lboost_system -lboost_filesystem
clean:
	rm -rd locate updatedb
