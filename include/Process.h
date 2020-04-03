#ifndef PROCESS_H
#define PROCESS_H

#include <unistd.h>
#include <string>

class Process
{
public:

    explicit Process(const std::string &path);

    ~Process();

    ssize_t write(const void *data, size_t len);

    void writeExact(const void *data, size_t len);

    ssize_t read(void *data, size_t len);

    void readExact(void *data, size_t len);

    bool isReadable() const;

    void closeStdin();

    void close();

    void closeStreams();

private:
    int _from_process;
    int _to_process;

    bool _enable_read;
    pid_t _child_pid;
};

#endif //PROCESS_H