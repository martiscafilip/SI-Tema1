#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/aes.h>

#define PORT 8040

using namespace std;

void printkey(unsigned char *buf)
{
    for (int i = 0; i < sizeof buf; i++)
    {
        printf("%X", buf[i]);
    }
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread, keysocket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *hello = "Hello from A";

    AES_KEY KK;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    if ((keysocket = accept(server_fd, (struct sockaddr *)&address,
                            (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // valread = read(new_socket, buffer, 1024);
    // printf("%s\n", buffer);

    cout << endl;
    cout << "Introduceti numarul tipului de comunicare dorit:" << endl;
    cout << "1: ECB." << endl;
    cout << "2. CBC." << endl;

    char type;
    cin >> type;
    cout << endl;

    send(new_socket, &type, strlen(&type), 0);
    printf("Type message sent to B.\n");

    char text[1024] = "Send me the encryption key!";
    send(keysocket, text, strlen(text), 0);
    cout << text << endl;

    unsigned char key[1024] = {0};
    valread = read(keysocket, key, 128);

    send(new_socket, key, 128, 0);
    printf("Key sent to B.\n");

    cout << "KEY: " << endl;
    printkey(key);
    cout << endl
         << endl;
    unsigned char keydecrypted[16];

    const unsigned char *ola = (const unsigned char *)"aabbccddeeffgghh";
    AES_set_decrypt_key((const unsigned char *)ola, 128, &KK);

    AES_decrypt((const unsigned char *)key, keydecrypted, &KK);
    cout << "DECRYPTED KEY: " << endl;
    printkey(keydecrypted);
    cout << endl
         << endl;

    send(new_socket, keydecrypted, 128, 0);
    printf("DeciptedKey sent to B.\n");

    unsigned char msg[16] = {0};
    read(new_socket, msg, 16);
    cout << endl;

    unsigned char start[16] = "START";
    if (memcmp(msg, start, 16) == 0)
    {
        int ttype = type - '0';
        if (ttype == 1) //ECB
        {

            FILE *stream;
            stream = fopen("file.txt", "rb");
            fseek(stream, 0, SEEK_END);
            long fsize = ftell(stream);
            fseek(stream, 0, SEEK_SET);
            char buffer[fsize + 1]; // Buffer to store data
            int count = fread(&buffer, 1, fsize, stream);
            fclose(stream);
            // Printing data to check validity
            buffer[count] = 0;
            printf("Data read from file: %s \n", buffer);
            printf("Elements read: %d\n", count);

            char hei[100];
            sprintf(hei, "%zu", strlen(buffer));
            send(new_socket, hei, 100, 0);

            for (int i = 0; i < strlen(buffer); i += 16)
            {
                char sub[17];
                unsigned char sub2[17];
                memcpy(sub, &buffer[i], 16);
                sub[16] = '\0';

                AES_KEY KKK;
               
                AES_set_encrypt_key((const unsigned char *)keydecrypted, 128, &KKK);
                AES_encrypt((unsigned char *)sub, sub2, &KKK);

                send(new_socket, sub2, 16, 0);
                cout << sub << endl;
                // printkey(sub2);
                // cout<<endl;
            }
        }

        if (ttype == 2) //CBC
        {

            FILE *stream;
            stream = fopen("file.txt", "rb");
            fseek(stream, 0, SEEK_END);
            long fsize = ftell(stream);
            fseek(stream, 0, SEEK_SET);
            char buffer[fsize + 1]; // Buffer to store data
            int count = fread(&buffer, 1, fsize, stream);
            fclose(stream);
            // Printing data to check validity
            buffer[count] = 0;
            printf("Data read from file: %s \n", buffer);
            printf("Elements read: %d\n", count);

            char hei[100];
            sprintf(hei, "%zu", strlen(buffer));
            send(new_socket, hei, 100, 0);

            char sub[17]={0};
            unsigned char sub2[17] ={0};
            char initializationArray[17] = "1234567812345678";
            cout<<endl;

            for (int i = 0; i < strlen(buffer); i += 16)
            {
                memcpy(sub, &buffer[i], 16);
                sub[16] = '\0';
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
                        cbc[i] = sub2[i] ^ sub[i];
                        cbc[16] = '\0';
                    }
                }

                AES_KEY KKK;
                // const unsigned char ola2[] = keydecrypted;
                AES_set_encrypt_key((const unsigned char *)keydecrypted, 128, &KKK);
                AES_encrypt((unsigned char *)sub, sub2, &KKK);
                sub2[16]='\0';

                send(new_socket, cbc, 16, 0);
                cout << sub << endl;
                // printkey(sub2);
                // cout<<endl;
            }
        }
    }

    return 0;
}
