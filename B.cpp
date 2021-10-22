#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

using namespace std;

#define PORT 8040

void printkey(unsigned char *buf)
{
	for (int i = 0; i < sizeof buf; i++)
	{
		printf("%X", buf[i]);
	}
}

int main(int argc, char const *argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};

	AES_KEY KK;
	char *comunicare = {0};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

	valread = read(sock, buffer, 1024);
	comunicare = buffer;
	printf("%s\n", comunicare);

	// send(sock, hello, strlen(hello), 0);
	// printf("Hello message sent\n");

	unsigned char key[1024] = {0};
	unsigned char key2[1024] = {0};

	unsigned char keydecrypted[16];

	read(sock, key, 128);
	read(sock, key2, 128);

	cout << "KEY: " << endl;
	printkey(key);
	cout << endl;
	const unsigned char *ola = (const unsigned char *)"aabbccddeeffgghh";
	AES_set_decrypt_key((const unsigned char *)ola, 128, &KK);

	AES_decrypt((const unsigned char *)key, keydecrypted, &KK);
	cout << "DECRYPTED KEY: " << endl;
	printkey(keydecrypted);
	cout << endl;

	if (memcmp(key2, keydecrypted, 16) == 0)
	{
		unsigned char hello[16] = "START";
		printf("Start message sent\n");
		send(sock, hello, 16, 0);
		printkey(key2);
		cout << endl;
	}

	int com = atoi(comunicare);
	if (com == 1)
	{
		char sizee[100];
		read(sock, sizee, 100);
		cout << sizee << endl;

		char sub[17];
		unsigned char sub2[17];

		for (int i = 0; i < atoi(sizee); i += 16)
		{
			read(sock, sub, 16);
			AES_KEY KKK;

			AES_set_decrypt_key((const unsigned char *)keydecrypted, 128, &KKK);
			AES_decrypt((const unsigned char *)sub, sub2, &KKK);
			cout << sub2 << endl;
			// printkey((unsigned char*)sub);
			//     cout<<endl;
		}
	}
	if (com == 2)
	{
		char sizee[100];
		read(sock, sizee, 100);
		cout << sizee << endl;

		char sub[17]={0};
		unsigned char sub2[17]={0};
		cout << endl;

		for (int i = 0; i < atoi(sizee); i += 16)
		{
			char initializationArray[17] = "1234567812345678";
			read(sock, sub, 16);
			char cbc[17];
			if (i == 0)
			{

				for (int i = 0; i < 17; i++)
				{
					cbc[i] = initializationArray[i] ^ sub[i];
					cbc[16] = '\0';
				}
			}
			else 
			{
				for (int i = 0; i < 17; i++)
				{
					cbc[i] = sub[i] ^ sub2[i];
					cbc[16] = '\0';
				}
			}
			AES_KEY KKK;
			AES_set_decrypt_key((const unsigned char *)keydecrypted, 128, &KKK);
			AES_decrypt((const unsigned char *)sub, sub2, &KKK);

			cout << cbc << endl;
			// printkey((unsigned char*)sub);
			//     cout<<endl;
		}
	}

	return 0;
}
