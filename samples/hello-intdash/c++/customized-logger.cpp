#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <vector>

const char *DATA = "Hello intdash!";
const char *ID = "abc";

const char *FIFO_LGR2MNG = "/var/run/intdash/logger_100.tx";
const char *FIFO_MNG2LGR = "/var/run/intdash/logger_100.rx";

static int g_sig_num = 0;

static void
sig_handler(int sig)
{
    g_sig_num = sig;
}

static std::vector<unsigned char> make_msg(const char *id, const char *str)
{
    // Make binary message for String format
    // See: https://docs.intdash.jp/manual/intdash-agent-developer-guide/latest/ja/intdash-agent-developer-guide-ja.pdf
    //
    // +---------------+---------------+---------------+---------------+
    // |    Type(01)   |                    Legnth                     |
    // +---------------+---------------+---------------+---------------+
    // |                           Time Sec                            |
    // +---------------+---------------+---------------+---------------+
    // |                           Time NSec                           |
    // +---------------+---------------+---------------+---------------+
    // |     DType     |      Seq      |   ID Length   |      ID
    // +---------------+---------------+---------------+-----------//--+
    // |   Data String
    // +-----------//--+

    std::vector<unsigned char> binary_msg;

    //
    // Type must be 1 (0x01)
    //

    // Make binary msg
    binary_msg.push_back(0x01);

    //
    // Length from Time Sec to end of Data String
    //

    // Get Data String length
    size_t data_len = strlen(str);

    // Get ID length
    unsigned char id_len = strlen(id);

    // Calc Length
    // 4(Time Sec) + 4(Time NSec) + 1(DType) + 1(Seq) + 1(ID Length) + id_len + data_len
    size_t msg_len = 4 + 4 + 1 + 1 + 1 + id_len + data_len;

    // Append binary msg
    binary_msg.push_back((msg_len >> 0) & 0xff);
    binary_msg.push_back((msg_len >> 8) & 0xff);
    binary_msg.push_back((msg_len >> 16) & 0xff);

    //
    // Time Sec & Time Nsec
    //

    // Get CLOCK_MONOTONIC
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // Append binary msg
    binary_msg.push_back((ts.tv_sec >> 0) & 0xff);
    binary_msg.push_back((ts.tv_sec >> 8) & 0xff);
    binary_msg.push_back((ts.tv_sec >> 16) & 0xff);
    binary_msg.push_back((ts.tv_sec >> 24) & 0xff);
    binary_msg.push_back((ts.tv_nsec >> 0) & 0xff);
    binary_msg.push_back((ts.tv_nsec >> 8) & 0xff);
    binary_msg.push_back((ts.tv_nsec >> 16) & 0xff);
    binary_msg.push_back((ts.tv_nsec >> 24) & 0xff);

    //
    // DType of String must be 29 (0x1D)
    // NOTE:
    //   Please note that intdash Edge Agent's DType of String (29, 0x1D)
    //   and intdash's Data Type of String (10, 0x0A) are different.
    //

    // Append binary msg
    binary_msg.push_back(0x1d);

    //
    // Seq must be 0
    //

    // Append binary msg
    binary_msg.push_back(0x00);

    //
    // ID Length
    //

    //  Change Time to hex string
    binary_msg.push_back(id_len);

    //
    // ID
    //

    // Append binary msg
    for (size_t i = 0; i < id_len; i++)
    {
        binary_msg.push_back(static_cast<unsigned char>(id[i]));
    }

    //
    // Data String
    //

    // Append binary msg
    for (size_t i = 0; i < data_len; i++)
    {
        binary_msg.push_back(static_cast<unsigned char>(str[i]));
    }

    return binary_msg;
}

int main(int argc, char *argv[], char *envp[])
{
    // Setup signal handler

    struct sigaction sa;
    sa.sa_handler = sig_handler;
    sigemptyset(&(sa.sa_mask));
    sa.sa_flags = 0;

    signal(SIGPIPE, SIG_IGN);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Open FIFO

    int fd = open(FIFO_LGR2MNG, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "fopen(%s) failed : %s (%d)\n", FIFO_LGR2MNG, strerror(errno), errno);
        return 1;
    }

    // Write data

    for (int count = 0; count < 200 && g_sig_num == 0; count++)
    {
        std::vector<unsigned char> msg = make_msg(ID, DATA);

        ssize_t writesize = write(fd, &msg[0], msg.size());
        if (writesize == -1)
        {
            fprintf(stderr, "write() failed : %s (%d)\n", strerror(errno), errno);
            break;
        }
        if (static_cast<size_t>(writesize) != msg.size())
        {
            fprintf(stderr, "write(%ld) return %lu\n", msg.size(), writesize);
            break;
        }

        usleep(500 * 1000);
    }

    // Close FIFO

    close(fd);

    // Waiting for signal

    while (g_sig_num == 0)
    {
        usleep(500 * 1000);
    }

    return 0;
}