#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/fcntl.h>
#include <cstring>

class Process
{
public:

    explicit Process(const std::string &path);

    ~Process();

    size_t write(const void *data, size_t len);

    void writeExact(const void *data, size_t len);

    size_t read(void *data, size_t len);

    void readExact(void *data, size_t len);

    bool isReadable() const;

    void closeStdin();

    void close();

    void closeStreams();

private:
    int _from_process[2];
    int _to_process[2];

    pid_t _child_pid;
};

#endif //PROCESS_H