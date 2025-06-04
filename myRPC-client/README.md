# Консольная утилита для отправки команд на удалённый myRPC-сервер.

## Сборка
```
cd myRPC-client
make all
```

## Использование

```
./myRPC-client -s|--stream | -d|--dgram 
               -h <IP_сервера> 
               -p <порт> 
               -c|--command "bash команда"
```

Пример:
```
./myRPC-client -s -h 192.168.0.10 -p 1234 -c "ls"
./myRPC-client --dgram --host 10.0.0.5 --port 4321 --command "df -h"
```

Для помощи введите аргумент `--help`.
