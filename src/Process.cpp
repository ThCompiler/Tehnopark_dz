#include "Process.h"

Process::Process(const std::string &path)
{
    if (pipe(_from_process) == -1)
    {
        fprintf(stderr, "can't create pipe for reading from process: %s\n",
                strerror(errno));
        return;
    }

    if (pipe(_to_process) == -1)
    {
        fprintf(stderr, "can't create pipe for writing to process: %s\n",
                strerror(errno));
        ::close(_from_process[0]);
        ::close(_from_process[1]);
        return;
    }

    _child_pid = fork();
    if (_child_pid == -1)
    {
        fprintf(stderr, "can't create a new process: %s\n", strerror(errno));
        closeStreams();
        return;
    }

    if (!_child_pid)
    {
        if (dup2(_to_process[0], STDIN_FILENO) == -1)
        {
            fprintf(stderr, "can't set stdin of process: %s\n",
                    strerror(errno));
            exit(-1);
        }

        if (dup2(_from_process[1], STDOUT_FILENO) == -1)
        {
            fprintf(stderr, "can't set stdout of process: %s\n",
                    strerror(errno));
            exit(-1);
        }

        ::close(_to_process[1]);
        ::close(_from_process[0]);

        prctl(PR_SET_PDEATHSIG, SIGTERM);

        char *arg[2] = {(char *) path.c_str(), nullptr};
        if (execv(arg[0], arg) == -1)
        {
            fprintf(stderr, "can't create process: %s\n", strerror(errno));
            exit(-1);
        }
    } else
    {
        ::close(_to_process[0]);
        ::close(_from_process[1]);
    }

}

Process::~Process()
{
    close();
}

void Process::closeStdin()
{
    if (_to_process[0] == -1)
    {
        return;
    }

    ::close(_to_process[0]);
    ::close(_to_process[1]);
    _to_process[0] = -1;
    _to_process[1] = -1;
}

size_t Process::write(const void *data, size_t len)
{
    if (_to_process[1] == -1)
    {
        perror("stdin of process close\n");
        return -1;
    }

    size_t res = ::write(_to_process[1], data, len);

    if (res < 0)
    {
        fprintf(stderr, "can't write to process: %s\n", strerror(errno));
    }

    return res;
}

void Process::writeExact(const void *data, size_t len)
{
    if (_to_process[1] == -1)
    {
        perror("stdin of process close\n");
        return;
    }

    size_t res = 0;
    while (res != len)
    {
        size_t len_write = ::write(_to_process[1], data, len - res);

        if (len_write < 0)
        {
            fprintf(stderr, "can't write to process: %s\n", strerror(errno));
        }

        res += len_write;
        wait(NULL);
    }
}

size_t Process::read(void *data, size_t len)
{
    size_t res = ::read(_from_process[0], data, len);

    if (res < 0)
    {
        fprintf(stderr, "can't read from process: %s\n", strerror(errno));
    }

    return res;
}

void Process::readExact(void *data, size_t len)
{
    size_t res = 0;
    while (res != len)
    {
        size_t len_read = ::read(_from_process[0], data, len - res);

        if (len_read < 0)
        {
            fprintf(stderr, "can't read from process: %s\n", strerror(errno));
            return;
        }

        res += len_read;
        wait(NULL);
    }

}

bool Process::isReadable() const
{
    if (fcntl(_from_process[0], F_GETFL) < 0)
    {
        if (errno != EBADF)
        {
            fprintf(stderr, "Error with stdin: %s\n", strerror(errno));
        }
        return false;
    }
    return true;
}

void Process::close()
{
    closeStreams();

    if (_child_pid)
    {
        int status = 0;
        waitpid(_child_pid, &status, WNOHANG);

        if (!WIFEXITED(status) && !WIFSIGNALED(status))
        {
            if (kill(_child_pid, SIGKILL) == -1)
            {
                fprintf(stderr, "Can't kill process: %s\n", strerror(errno));
            }
        }
    }
    _child_pid = 0;
}

void Process::closeStreams()
{
    ::close(_to_process[0]);
    ::close(_from_process[1]);
    ::close(_to_process[1]);
    ::close(_from_process[0]);
}
