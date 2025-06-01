#ifndef LIBMYSYSLOG_H
#define LIBMYSYSLOG_H

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

enum LogDriver {
    TEXT_DRIVER,
    JSON_DRIVER
};

int mysyslog(const char* msg, int level, int driver, int format, const char* path);

#endif // LIBMYSYSLOG_H