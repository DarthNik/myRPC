#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <libconfig.h>
#include "../libmysyslog/libmysyslog.h"
#include <sys/socket.h>
#include <string.h>

int is_allowed_user(const char *name){
    FILE *file = fopen("/etc/myRPC/users.conf", "r");
    if (file == 0){
	mysyslog("Ошибка открытия файла разрешённых пользователей", 1, 0, 0, "");
	return 0;
    }
    
    int allow = 0;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), file) != NULL){
	buffer[strcspn(buffer, "\r\n")] = 0;
	if (strcmp(buffer, name) == 0){
	    allow = 1;
	    break;
	}
    }

    fclose(file);
    return allow;
}

void execution(const char *command, int fdo, int fde){
    if (dup2(fdo, 1) == -1)
        mysyslog("Ошибка перенаправления вывода", 1, 0, 0, "");
        
    if (dup2(fde, 2) == -1)
        mysyslog("Ошибка перенаправления вывода ошибок", 1, 0, 0, "");

    if (system(command) < 0)
        mysyslog("Ошибка выполнения команды", 1, 0, 0, "");

}

int main(){
    //Чтение конфигурационного файла
    config_t conf;
    config_init(&conf);
    if (config_read_file(&conf, "/etc/myRPC/myRPC.conf") == 0){
	mysyslog("Ошибка чтения конфигурационного файла%d", 1, 0, 0, "");
	config_destroy(&conf);
	exit(1);
    }

    int port; 
    if (config_lookup_int(&conf, "port", &port) == 0){
	mysyslog("Ошибка получения порта", 1, 0, 0, "");
	exit(1);
    }

    int socket_type;
    if (config_lookup_int(&conf, "socket_type", &socket_type) == 0){
	mysyslog("Ошибка получения типа сокета", 1, 0, 0, "");
        exit(1);
    }

    config_destroy(&conf);

    //Создание сокета
    int sock;
    if (socket_type == 1)
	sock = socket(AF_INET, SOCK_STREAM, 0);
    else
	sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
	mysyslog("Ошибка создания сокета", 1, 0, 0, "");

    //Настройка механизма сокетов
    struct sockaddr_in serv_addr, client_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t addr_len = sizeof(client_addr); 
    
    if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
	mysyslog("Ошибка привязки сокета", 1, 0, 0, "");
	close(sock);
	exit(1);
    }

    if (listen(sock, 5) < 0 && socket_type){
	mysyslog("Ошибка прослушивания сокета", 1, 0, 0, "");
	exit(1);
    }

    int sd;
    while (1){
	if ((sd = accept(sock, (struct sockaddr*)&client_addr, &addr_len)) < 0){
	    mysyslog("Ошибка принятия соединения", 1, 0, 0, "");
	    continue;
	}
        
	int recv_bytes;
	char buf[1024];
	if ((recv_bytes = recv(sd, buf, sizeof(buf) - 1, 0)) <= 0){
	    mysyslog("Ошибка получения данных", 1, 0, 0, "");
	    close(sd);
	}

 	//Разбор полученной строки
	char user[128] = {0};
	char command[512] = {0};
	sscanf(buf, "\"%[^\"]\": \"%[^\"]\"", user, command);

	//Проверка допущен ли пользователь
	if (!is_allowed_user(user)){
	    const char *allow_response = {"1: \"Пользователь не допущен\""};
	    send(sd, allow_response, strlen(allow_response), 0);
	    close(sd);
	}

	char out_temp[] = "/tmp/myRPC_stdout_XXXXXX";
        char err_temp[] = "/tmp/myRPC_stderr_XXXXXX";
        int fdo = mkstemp(out_temp);
        int fde = mkstemp(err_temp);

        if (fdo < 0 || fde < 0){
            mysyslog("Ошибка создания временного файла", 1, 0, 0, "");
            break;
        }

	execution(command, fdo, fde);

	char result[1024];
	lseek(fdo, 0, SEEK_SET);
	lseek(fde, 0, SEEK_SET);
	read(fdo, result, sizeof(result));
	close(fdo);
	char errors[1024] = {0};
	read(fde, errors, sizeof(errors));
	close(fde);
	errors[strlen(errors) - 1] = 0;
	result[strlen(result) - 1] = 0;
	char response[2048];
	if (strlen(errors) > 0){
	    snprintf(response, sizeof(response), "1: \"%s\"", errors);
	    send(sd, response, strlen(response), 0);
	    close(sd);
	}
	
	snprintf(response, sizeof(response), "0: \"%s\"", result);
	send(sd, response, strlen(response), 0);
	memset(result, 0, sizeof(result));
	close(sd);
    }

    close(sock);
    return 0;
}
