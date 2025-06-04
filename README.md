# myRPC

## Описание

Проект **myRPC** представляет собой механизм вызова команд myRPC . Проект включает клиентское приложение `myRPC-client` и серверное приложение-демон `myRPC-server`, которые взаимодействуют через сокеты.

## Требования

- gcc
- libconfig (`sudo apt-get install libconfig-dev`)
- dpkg-deb
- make

## Сборка проекта
```
make all
```

Сборка производится в директории соответствующих подпроектам дирректориях. Там же будут размещены исполняемые файлы:

- `myRPC-client/myRPC-client`
- `myRPC-server/myRPC-server`

### Подлючение библиотеки проекта

1. Необходимо подключить библиотеку `libmysyslog` для работоспособности проекта.
2. Выполните следующую команду:
```
export LD_LIBRARY_PATH=/home/user/myRPC/libmysyslog:$LD_LIBRARY_PATH
```

## Использование

### Настройка сервера

Создайте конфигурационный файл `/etc/myRPC/myRPC.conf` со следующим содержимым:
```conf
# Порт, на котором будет слушать сервер
port = 1234;
# Тип сокета : 1(TCP) или 0(UDP)
socket_type = 1;
```

Создайте файл со списком разрешенных пользователей `/etc/myRPC/users.conf`:
```
# Список разрешённых пользователей
username1
username2
username3
```

Запустите сервер:

```
cd myRPC-server
./myRPC-server
```

### Использование клиента
```
cd myRPC-client
./myRPC-client -s | -d -h <адрес_сервера> -p <порт> -c "команда bash"
```

Пример:
```
./myRPC-client -s -h 127.0.0.1 -p 1234 -c "ls -la"
```

## Создание deb-пакетов
Для сборки deb-пакетов выполните:
```
make deb
```

## Создание локального репозитория
Для создания локального репозитория APT выполните:
```
make repo
```

После этого вы сможете устанавливать пакеты через apt:
```
sudo apt-get install myrpc-client myrpc-server
```

## systemd unit
```
make systemd_install
```

## Очистка
Очистка временных файлов и папок:
```
make clean
```
