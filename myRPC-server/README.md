# Служба-демон, выполняющая команды по удалённому доступу с помощью протокла RPC
## Зависимости
- gcc
- libconfig (`libconfig-dev`)
- libmysyslog (собирается в подпроекте `libmysyslog`)
- pkg-config
- make

Данные зависимости можно установить командой:
```
sudo apt update
sudo apt install gcc libconfig-dev make pkg-config
```

## Сборка
```
cd myRPC-server
make all
```

Но перед сбокрой необходимо убедиться, что собрана библиотека `libmysyslog.a` или `libmysyslog.so` 

## Подготовка к запуску
1. Создайте каталог:
```
sudo mkdir /etc/myRPC
```

2. Перейдите в него и создайте файл:
```
sudo touch myRPC.conf
```

В нём пропишите:
```
port = 1234;
socket_type = 1; #TCP(1) или UDP(0)
```

3. Создайте ещё файл `users.conf`. В нём будет храниться список разрешённых пользователей

Пример:
```
nik
alice
```

## Запуск
```
./myRPC-server
```

## DEB-пакет
```
make deb
```
Данная команда собирает deb-пакет `MyRPC_server_1.0-0_all.deb`.
Чтобы его установить, введите команду:
```
sudo dpkg -i MyRPC_server_1.0-0_all.deb
```
