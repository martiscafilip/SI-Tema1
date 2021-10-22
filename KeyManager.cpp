#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/aes.h>

#define PORT 8040

using namespace std;

void printkey(unsigned char* buf)
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
    const char *hello = "Hello from KeyManager";
    char buffer[1024] = {0};

    char KECB[16] = "aabbccddeeffggh";
    char KCBC[16] = "aabbccddeeffggh";
    AES_KEY KK;

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
    printf("%s\n", buffer);

    if (strcmp(buffer, "Send me the encryption key!") == 0)
    {
        unsigned char key[16];
        unsigned char key2[16];

        RAND_bytes(key, sizeof(key));
        const unsigned char *ola = (const unsigned char *)"aabbccddeeffgghh";
        AES_set_encrypt_key((const unsigned char *)ola, 128, &KK);

        AES_encrypt(key, key2, &KK);

        cout << "Initial key: " << endl;
        printkey(key);
        cout << endl;

        cout << "Encryption Key sent to A." << endl;
        cout << "ENCRYPTED KEY: " << endl;
        printkey(key2);
        cout<<endl;
        send(sock, key2, 128, 0);
    }

    return 0;
}
