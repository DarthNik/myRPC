#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libmysyslog.h>

void usage(const char *name){
    printf("Использование: %s -s|--stream или -d|--dgram -h|--host <ip_addr> -p|--port <port> -c|--command \"bash команда\"\n", name);
} 

int main(int argc, char *argv[]){
    int stream = 1; //По умолчанию TCP
    char host[256];
    int port;
    char command[1024] = {0};
    int opt;

    //Парсинг аргументов
    struct option long_option[] = {
	{"stream", no_argument, NULL, 's'},
	{"dgram", no_argument, NULL, 'd'},
	{"host", required_argument, NULL, 'h'},
	{"port", required_argument, NULL, 'p' },
	{"command", required_argumant, NULL, 'c'},
	{0, 0, 0, 0}
    };
  
    for (int i = 0; i < argc; i++){
	if (strcmp(argv[i], "--help") == 0){
	     usage(argv[0]);
	}

	while ((opt = getopt_long(argc, argv, "sdh:p:c:", long_option, NULL)) != -1){
            switch (opt){
                case 's': stream = 1; break;
                case 'd': stream = 0; break;
                case 'h': strncpy(host, argv[i], sizeof(host)); break;
                case 'p': port = atoi(argv[i]); break;
                case 'c': strcnpy(command, argv[i], sizeof(command)); break;
                default:
                    fprintf(stderr, "Unknown option. Use -h for help\n");
                    exit(1);
            }
	}
    }

    char *user = getlogin();
    if (user == NULL){
	perror("Ошибка");
	exit(1);
    }

    //Формирование запроса текстового протокола
    char request[1024];
    snprintf(request, sizeof(request), "\"%s\": \"%s\"", user, command);

    //Создание сокета
    int sock;
    if (stream)
	sock = socket(AF_INET, SOCK_STREAM, 0);
    else
	sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0){
	perror("Ошибка");
	exit(1);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    struct in_addr serv;
    serv.s_addr = inet_addr(host);
    serv_addr.sin_addr = serv;

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
	perror("Ошибка подключения к серверу");
	close(sock);
	exit(1);
    }


    if (send(sock, request, strlen(request), 0) < 0){
	perror("Ошибка передачи данных");
	break;
    }

    int n;
    char buf[1024];
    if ((n = recv(sock, buf, sizeof(buf) - 1, 0)) <= 0){
	printf("Соединение разорвано\n");
	break;
    }

    printf("%s", buf);
    close(sock);
    return 0;
}
