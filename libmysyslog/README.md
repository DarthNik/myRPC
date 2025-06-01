# Библиотека libmysyslog на языке Си
Разработать расширяемую библиотеку libmysyslog на языке Си, которая сможет выводить данные в журнал в разных форматах. В рамках библиотеки необходимо реализовать функцию, через которую другие программы смогут выводить данные в журнал:
```c
int mysyslog(const char* msg, int level, int driver, int format, const char* path);
```
Поддерживаются следующие уровни журналирования: DEBUG, INFO, WARN, ERROR, CRITICAL.

Для библиотеки libmysyslog реализованы в виде подключаемого плагина 2 драйвера (libmysyslog-text, libmysyslog-json): вывод в журнал в виде строки с разделителем и вывод в журнал в виде json строки.

Структура журнала в текстовом формате:
```
timestamp log_level process message
```
Пример журнала в текстовом формате:
```
1439482969 ERROR example-app this is an error
```
Структура журнала в формате JSON:
```json
{"timestamp": ..., "log_level": ..., "process": ..., "message": ...}
```
Пример журнала в формате JSON:
```json
{"timestamp":1439482969,"log_level":"ERROR","process":"example-app","message":"this is an error"}
```