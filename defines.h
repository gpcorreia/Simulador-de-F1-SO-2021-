#ifndef DEFINES_H
#define DEFINES_H

#define CONFIG_FILE "./config.txt"
#define LOG_FILE "./log.txt"

#define MAX_LINES_CONFIG 8
#define MAX_COLUMS_CONFIG 10
#define MAX_CHAR_NAMES 20

//Named Semaphores

#define MUTEX_UP "MT WR LOG"          //mutex for write log
#define MUTEX_WR_LOG "MT UnnamedPipe" //mutex for Unnamed Pipe
#define MUTEX_SH "MT SharedMem"       //mutex for shared memory
#define PIPE_NAME "namedpipe"

#define KEY 2212

#endif