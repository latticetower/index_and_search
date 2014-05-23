boost_dir=../../boost_1_55_0

all:
	g++ -I $(boost_dir) main_test.cpp -o main_test
clean:
	rm -rd main_test
