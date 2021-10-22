all: A B KEY

A: A.cpp
	g++ A.cpp -o A.out -L/usr/local/ssl/lib -lssl -lcrypto

B: B.cpp
	g++ B.cpp -o B.out -L/usr/local/ssl/lib -lssl -lcrypto

KEY: KeyManager.cpp
	g++ KeyManager.cpp -o KeyManager.out -L/usr/local/ssl/lib -lssl -lcrypto