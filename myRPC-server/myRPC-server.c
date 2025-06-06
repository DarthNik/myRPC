#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <libconfig.h>
#include "libmysyslog.h"
#include <sys/socket.h>
#include <string.h>

#define LOG_PATH "/var/log/myRPC.log"

//Проверка, допущен ли пользователь
int is_allowed_user(const char *name){
    FILE *file = fopen("/etc/myRPC/users.conf", "r");
    if (file == 0){
	mysyslog("Ошибка открытия файла разрешённых пользователей", 3, 0, 0, LOG_PATH);
	return 0;
    }
    //сравнение имён из файла с переданным в функцию
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


//Функция выполнения команды пользователя и перенаправления вывода
void execution(const char *command, int fdo, int fde){
    if (dup2(fdo, 1) == -1)
        mysyslog("Ошибка перенаправления вывода", 3, 0, 0, LOG_PATH);
        
    if (dup2(fde, 2) == -1)
        mysyslog("Ошибка перенаправления вывода ошибок", 3, 0, 0, LOG_PATH);

    if (system(command) < 0)
        mysyslog("Ошибка выполнения команды", 3, 0, 0, LOG_PATH);

}

int main(){
    //Чтение конфигурационного файла
    config_t conf;
    config_init(&conf);
    if (config_read_file(&conf, "/etc/myRPC/myRPC.conf") == 0){
	mysyslog("Ошибка чтения конфигурационного файла", 3, 0, 0, LOG_PATH);
	config_destroy(&conf);
	exit(1);
    }
    //считывание порта из файла
    int port; 
    if (config_lookup_int(&conf, "port", &port) == 0){
	mysyslog("Ошибка получения порта", 3, 0, 0, LOG_PATH);
	exit(1);
    }
    //считывание типа сокета
    int socket_type;
    if (config_lookup_int(&conf, "socket_type", &socket_type) == 0){
	mysyslog("Ошибка получения типа сокета", 3, 0, 0, LOG_PATH);
        exit(1);
    }

    config_destroy(&conf);

    mysyslog("Сервер запущен", 1, 0, 0, LOG_PATH);
    //Создание сокета
    int sock;
    if (socket_type == 1)
	sock = socket(AF_INET, SOCK_STREAM, 0);
    else
	sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
	mysyslog("Ошибка создания сокета", 3, 0, 0, LOG_PATH);

    //Настройка механизма сокетов
    struct sockaddr_in serv_addr, client_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t addr_len = sizeof(client_addr); 
    
    //привязка сокета
    if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
	mysyslog("Ошибка привязки сокета", 3, 0, 0, LOG_PATH);
	close(sock);
	exit(1);
    }

    //прослушивание соединений
    if (listen(sock, 5) < 0 && socket_type){
	mysyslog("Ошибка прослушивания сокета", 3, 0, 0, LOG_PATH);
	exit(1);
    }

    int sd = 0;
    while (1){
        //принятие соединения
        if (socket_type){
	    if ((sd = accept(sock, (struct sockaddr*)&client_addr, &addr_len)) < 0){
	        mysyslog("Ошибка принятия соединения", 3, 0, 0, LOG_PATH);
	        continue;
	    }
	}
	
	mysyslog("Пользователь подключился", 1, 0, 0, LOG_PATH);
	
        //получение сообщения от пользователя
	int recv_bytes;
	char buf[1024];
	if ((recv_bytes = recvfrom(sd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&client_addr, &addr_len)) <= 0){
	    mysyslog("Ошибка получения данных от пользователя", 3, 0, 0, LOG_PATH);
	    close(sd);
	}
        
        mysyslog("Получено сообщение от пользователя", 1, 0, 0, LOG_PATH);
        
 	//Разбор полученной строки
	char user[128] = {0};
	char command[512] = {0};
	sscanf(buf, "\"%[^\"]\": \"%[^\"]\"", user, command);

	//Проверка допущен ли пользователь
	if (!is_allowed_user(user)){
	    const char *allow_response = {"1: \"Пользователь не допущен\""};
	    sendto(sd, allow_response, strlen(allow_response), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
	    close(sd);
	}

        //создание временных файлов
	char out_temp[] = "/tmp/myRPC_stdout_XXXXXX";
        char err_temp[] = "/tmp/myRPC_stderr_XXXXXX";
        int fdo = mkstemp(out_temp);
        int fde = mkstemp(err_temp);

        if (fdo < 0 || fde < 0){
            mysyslog("Ошибка создания временного файла", 3, 0, 0, LOG_PATH);
            break;
        }

        //выполнение полученной команды
	execution(command, fdo, fde);

        //считывание временных файлов
	char result[1024];
	lseek(fdo, 0, SEEK_SET);
	lseek(fde, 0, SEEK_SET);
	if (read(fdo, result, sizeof(result)) < 0){
	    mysyslog("Ошибка чтения файла вывода", 3, 0, 0, LOG_PATH);
	}
	
	close(fdo);
	char errors[1024] = {0};
	if (read(fde, errors, sizeof(errors)) < 0 ){
	    mysyslog("Ошибка чтения файла вывода", 3, 0, 0, LOG_PATH);
	}
	
	close(fde);
	errors[strlen(errors) - 1] = 0;
	result[strlen(result) - 1] = 0;
	
	//создание ответа для пользователя
	char response[2048];
	mysyslog("Отправка ответа пользователю", 1, 0, 0, LOG_PATH);
	if (strlen(errors) > 0){
	    //ответ в случае ошибки
	    snprintf(response, sizeof(response), "1: \"%s\"", errors);
	    sendto(sd, response, strlen(response), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
	    close(sd);
	    unlink(out_temp);
	    unlink(err_temp);
	}
	
	//ответ в случае успеха
	snprintf(response, sizeof(response), "0: \"%s\"", result);
	sendto(sd, response, strlen(response), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
	memset(result, 0, sizeof(result));
	close(sd);
	unlink(out_temp);
	unlink(err_temp);
    }

    close(sock);
    return 0;
}
