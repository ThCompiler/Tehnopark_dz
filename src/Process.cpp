#include "Process.h"
#include "Exception.h"
#include <string>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/fcntl.h>
#include <cstring>

Process::Process(const std::string &path)
{
    int from[2];
    int to[2];

    if (pipe(from) == -1)
    {
        throw CreateError("can't create pipe for reading from process: " +
                          std::string(strerror(errno)));
    }

    if (pipe(to) == -1)
    {
        ::close(from[0]);
        ::close(from[1]);
        throw CreateError("can't create pipe for reading from process: " +
                          std::string(strerror(errno)));
    }

    _child_pid = fork();
    if (_child_pid == -1)
    {
        ::close(to[0]);
        ::close(to[1]);
        ::close(from[0]);
        ::close(from[1]);

        throw CreateError(
                "can't create a new process: " + std::string(strerror(errno)));
    }

    if (_child_pid == 0)
    {
        if (dup2(to[0], STDIN_FILENO) == -1)
        {
            fprintf(stderr, "can't set stdin of process: %s\n",
                    strerror(errno));
            exit(-1);
        }

        if (dup2(from[1], STDOUT_FILENO) == -1)
        {
            fprintf(stderr, "can't set stdout of process: %s\n",
                    strerror(errno));
            exit(-1);
        }
        ::close(to[0]);
        ::close(from[1]);
        ::close(to[1]);
        ::close(from[0]);

        prctl(PR_SET_PDEATHSIG, SIGTERM);

        char *arg[2] = {(char *) path.c_str(), nullptr};
        if (execv(arg[0], arg) == -1)
        {
            fprintf(stderr, "can't create process: %s\n", strerror(errno));
            exit(-1);
        }
    } else
    {
        ::close(to[0]);
        ::close(from[1]);

        _to_process = to[1];
        _from_process = from[0];
    }
    _enable_read = true;
}

Process::~Process()
{
    close();
}

void Process::closeStdin()
{
    if (_to_process == -1)
    {
        return;
    }

    ::close(_to_process);
    _to_process = -1;
}

ssize_t Process::write(const void *data, size_t len)
{
    ssize_t res = ::write(_to_process, data, len);
    if(res == 0)
    {
        _enable_read = false;
    }
    return res;
}

void Process::writeExact(const void *data, size_t len)
{
    size_t res = 0;
    while (res != len)
    {
        size_t len_write = write((const char*)data + res, len - res);

        if (len_write < 0)
        {
            throw StreamError(
                    "can't write to process: " + std::string(strerror(errno)));
        }

        res += len_write;
    }
}

ssize_t Process::read(void *data, size_t len)
{
    return ::read(_from_process, data, len);
}

void Process::readExact(void *data, size_t len)
{
    size_t res = 0;
    while (res != len)
    {
        size_t len_read = read((char*)data + res, len - res);

        if (len_read < 0)
        {
            throw StreamError(
                    "can't read from process: " + std::string(strerror(errno)));
        }

        res += len_read;
    }

}

bool Process::isReadable() const
{
    return _enable_read;
}

void Process::close()
{
    closeStreams();

    if (_child_pid != 0)
    {
        int status = 0;
        if (kill(_child_pid, SIGKILL) == -1)
        {
            fprintf(stderr, "Can't kill process: %s\n", strerror(errno));
        }

        waitpid(_child_pid, &status, WNOHANG);
    }
    _child_pid = 0;
}

void Process::closeStreams()
{
    ::close(_from_process);
    ::close(_to_process);
}

