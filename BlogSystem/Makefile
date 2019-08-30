FLAG=-std=c++11 -L/usr/lib64/mysql -lmysqlclient -ljsoncpp -lpthread
all:db_test blog_server 

db_test:db_test.cpp db.hpp 
	g++ db_test.cpp -o db_test $(FLAG)

blog_server:blog_server.cpp 
	g++ blog_server.cpp -o blog_server $(FLAG)

.PHONY:clean
clean:
	rm db_test blog_server 

