#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <vector>

const char *FIFO_LGR2MNG = "/var/run/intdash2/logger_100.tx";
const char *FIFO_MNG2LGR = "/var/run/intdash2/logger_100.rx";

static int g_sig_num = 0;

static void
sig_handler(int sig)
{
    g_sig_num = sig;
}

struct StringMessage
{
    unsigned int sec;
    unsigned int nsec;
    std::string id;
    std::string data;
};

static bool
parse_msg(std::vector<unsigned char> &pool, StringMessage &msg, std::string &err)
{
    // Parse message
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

    // Initialize

    err.erase();

    // 1(Type) + 3(Length) + 4(Time Sec) + 4(Time NSec) + 1(DType) + 1(Seq)
    const size_t minimum_msg_size = 1 + 3 + 4 + 4 + 1 + 1;
    // minimum_msg_size + 1(ID Length)
    const size_t offset_to_id = minimum_msg_size + 1;

    // Check size of pool

    if (pool.size() < minimum_msg_size)
    {
        return false;
    }

    //
    // Type must be 1 (0x01)
    //

    if (pool[0] != 1)
    {
        pool.erase(pool.begin(), pool.begin() + 1);
        err = "unexpected data: type must be 1";
        return false;
    }

    //
    // Length from Time Sec to end of Data String
    //

    size_t length_from_time_to_end = 0;
    length_from_time_to_end |= pool[1];
    length_from_time_to_end |= pool[2] << 8;
    length_from_time_to_end |= pool[3] << 16;

    size_t msg_len = 1 + 3 + length_from_time_to_end;

    // Check data size
    if (pool.size() < msg_len)
    {
        return false;
    }

    //
    // Time Sec & Time Nsec
    //

    msg.sec = le32toh(*(unsigned int *)(&pool[4]));
    msg.nsec = le32toh(*(unsigned int *)(&pool[8]));

    //
    // DType of String must be 29 (0x1D)
    // NOTE:
    //   Please note that intdash Edge Agent's DType of String (29, 0x1D)
    //   and intdash's Data Type of String (10, 0x0A) are different.
    //

    if (pool[12] != 0x1d)
    {
        pool.erase(pool.begin(), pool.begin() + msg_len);
        err = "dtype is not string";
        return false;
    }

    //
    // ID Length
    //

    size_t id_len = pool[14];

    //
    // ID
    //

    msg.id.assign(pool.begin() + offset_to_id, pool.begin() + offset_to_id + id_len);

    //
    // Data String
    //

    size_t offset_to_data = offset_to_id + id_len;
    size_t data_len = msg_len - offset_to_data;

    msg.data.assign(pool.begin() + offset_to_data, pool.begin() + offset_to_data + data_len);

    pool.erase(pool.begin(), pool.begin() + msg_len);
    return true;
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

    int fd = open(FIFO_MNG2LGR, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "fopen(%s) failed : %s (%d)\n", FIFO_MNG2LGR, strerror(errno), errno);
        return 1;
    }

    // Read data

    std::vector<unsigned char> pool;
    while (g_sig_num == 0)
    {
        // Parse data

        StringMessage msg;
        std::string err;
        if (parse_msg(pool, msg, err))
        {
            // Print message
            fprintf(stdout, "Timestamp: %u.%09u, id: %s, data: %s\n", msg.sec, msg.nsec, msg.id.c_str(), msg.data.c_str());
            continue;
        }
        if (err.length() > 0)
        {
            fprintf(stderr, "parse failed: %s\n", err.c_str());
            continue;
        }

        // Read data from FIFO

        unsigned char buf[minimum_msg_size];
        ssize_t readsize = read(fd, buf, sizeof(buf));
        if (readsize == -1)
        {
            fprintf(stderr, "read() failed: %s (%d)\n", strerror(errno), errno);
            break;
        }

        // Store data to pool

        pool.insert(pool.end(), buf, buf + readsize);
    }

    // Close FIFO

    close(fd);

    return 0;
}