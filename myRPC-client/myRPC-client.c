#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "libmysyslog.h"

#define LOG_PATH "/var/log/myRPC.log"

//функция вывода помощи
void usage(const char *name){
    printf("Использование: %s -s|--stream или -d|--dgram -h|--host <ip_addr> -p|--port <port> -c|--command \"bash команда\"\n", name);
    exit(0);
} 

int main(int argc, char *argv[]){
    int stream = 1; //По умолчанию TCP
    char host[256];
    int port = 1234;
    char command[1024] = {0};
    int opt;

    //Обработка длинных аргументов
    struct option long_option[] = {
	{"stream", no_argument, NULL, 's'},
	{"dgram", no_argument, NULL, 'd'},
	{"host", required_argument, NULL, 'h'},
	{"port", required_argument, NULL, 'p' },
	{"command", required_argument, NULL, 'c'},
	{"help", no_argument, NULL, 0},
	{0, 0, 0, 0}
    };
  
    //обработка аргументов
    for (int i = 0; i < argc; i++){
	while ((opt = getopt_long(argc, argv, "sdh:p:c:", long_option, NULL)) != -1){
            switch (opt){
                case 's': stream = 1; break;
                case 'd': stream = 0; break;
                case 'h': strcpy(host, optarg); break;
                case 'p': port = atoi(optarg); break;
                case 'c': strcpy(command, optarg); break;
		case 0: usage(argv[0]); return 0;
                default:
                    usage(argv[0]);
                    exit(1);
            }
	}
    }

    //получение имени пользователя
    char *user = getlogin();
    if (user == NULL){
        mysyslog("Ошибка получения имени пользователя", 3, 0, 0, LOG_PATH);
	perror("Ошибка получения имени пользователя");
	exit(1);
    }

    //Формирование запроса текстового протокола
    char request[2048];
    snprintf(request, sizeof(request), "\"%s\": \"%s\"", user, command);

    //Создание сокета
    int sock;
    if (stream)
	sock = socket(AF_INET, SOCK_STREAM, 0);
    else
	sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0){
        mysyslog("Ошибка создания сокета", 3, 0, 0, LOG_PATH);
	perror("Ошибка создания сокета");
	exit(1);
    }

    //настройка сокета
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    struct in_addr serv;
    serv.s_addr = inet_addr(host);
    serv_addr.sin_addr = serv;
    socklen_t len = sizeof(serv_addr);

    //подключение к серверу
    mysyslog("Подключение с серверу...", 1, 0, 0, LOG_PATH);
    if (stream){
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
            mysyslog("Ошибка подключения к серверу", 3, 0, 0, LOG_PATH);
	    perror("Ошибка подключения к серверу");
	    close(sock);
	    exit(1);
        }
    }
        //отправка сообщения серверу
        if (sendto(sock, request, strlen(request), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
            mysyslog("Ошибка передачи данных серверу", 3, 0, 0, LOG_PATH);
	    perror("Ошибка передачи данных серверу");
	    exit(1);
        }

        //получения ответа от сервера
        int n;
        char buf[2048] = {0};
        if ((n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&serv_addr, &len)) <= 0){
	    printf("Соединение разорвано\n");
	    exit(1);
        }
    
    mysyslog("Получено сообщение от сервера", 1, 0, 0, LOG_PATH);
    printf("%s\n", buf);
    close(sock);
    return 0;
}
