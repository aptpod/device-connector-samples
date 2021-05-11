#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>

const char* JPEG_FILE = "/tmp/small.jpg";

const char* FIFO_LGR2MNG = "/var/run/intdash/logger_100.tx";
const char* FIFO_MNG2LGR = "/var/run/intdash/logger_100.rx";

#define TIMESPEC_CMP(lhs, rhs, cmp)                                            \
    (((lhs).tv_sec == (rhs).tv_sec) ? ((lhs).tv_nsec cmp(rhs).tv_nsec)         \
                                    : ((lhs).tv_sec cmp(rhs).tv_sec))

#define TIMESPEC_ADD(ts, sec, nsec)                                            \
    do {                                                                       \
        (ts).tv_sec += sec;                                                    \
        (ts).tv_nsec += nsec;                                                  \
        if ((ts).tv_nsec >= 1000000000) {                                      \
            (ts).tv_sec++;                                                     \
            (ts).tv_nsec -= 1000000000;                                        \
        }                                                                      \
    } while (0)

static int g_sig_num = 0;

static void
sig_handler(int sig)
{
    g_sig_num = sig;
}

static bool
isTimeUpThenAdd(const struct timespec& ts,
                struct timespec& timeup,
                unsigned int offset_sec,
                unsigned int offset_nsec)
{
    if (TIMESPEC_CMP(ts, timeup, >=)) {
        TIMESPEC_ADD(timeup, offset_sec, offset_nsec);
        return true;
    } else {
        return false;
    }
}

static bool
make_msg(const char* jpeg_file, unsigned char** out_data, size_t* out_data_len)
{
    // Make binary message for String format
    //
    // +---------------+---------------+---------------+---------------+
    // |    Type(01)   |                    Legnth                     |
    // +---------------+---------------+---------------+---------------+
    // |                           Time Sec                            |
    // +---------------+---------------+---------------+---------------+
    // |                           Time NSec                           |
    // +---------------+---------------+---------------+---------------+
    // |     DType     |      Seq      |   Jpeg Data
    // +---------------+---------------+-----------//--+

    // Calc Length

    struct stat st;
    if (stat(jpeg_file, &st) != 0) {
        fprintf(stderr, "unexpected jpeg file path: %s", JPEG_FILE);
        return false;
    }
    size_t data_len = st.st_size;
    size_t msg_len = 4 + 4 + 1 + 1 + data_len;
    size_t msg_size = 4 + msg_len;

    // Create buffer

    unsigned char* binary_msg = new unsigned char[msg_size];

    //
    // Type must be 01
    //

    // Make binary msg
    binary_msg[0] = 0x01;

    //
    // Length from Time Sec to End of Data String
    //

    // Append binary msg
    binary_msg[1] = (msg_len >> 0) & 0xff;
    binary_msg[2] = (msg_len >> 8) & 0xff;
    binary_msg[3] = (msg_len >> 16) & 0xff;

    //
    // Time Sec & Nsec
    //

    // Get CLOCK_MONOTONIC
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // Append binary msg
    binary_msg[4] = (ts.tv_sec >> 0) & 0xff;
    binary_msg[5] = (ts.tv_sec >> 8) & 0xff;
    binary_msg[6] = (ts.tv_sec >> 16) & 0xff;
    binary_msg[7] = (ts.tv_sec >> 24) & 0xff;
    binary_msg[8] = (ts.tv_nsec >> 0) & 0xff;
    binary_msg[9] = (ts.tv_nsec >> 8) & 0xff;
    binary_msg[10] = (ts.tv_nsec >> 16) & 0xff;
    binary_msg[11] = (ts.tv_nsec >> 24) & 0xff;

    //
    // DType of String must be 0x1d
    //

    // Append binary msg
    binary_msg[12] = 0x1d;

    //
    // Seq must be 0
    //

    // Append binary msg
    binary_msg[13] = 0x00;

    //
    // JPEG Data
    //

    FILE* jpeg = fopen(jpeg_file, "r");
    size_t read_len = fread(&binary_msg[14], 1, data_len, jpeg);
    if (read_len != data_len) {
        fprintf(stderr, "unexpected read size: %ld vs %ld", read_len, data_len);
        delete[] binary_msg;
        return false;
    }
    fclose(jpeg);

    *out_data = binary_msg;
    *out_data_len = msg_size;
    return true;
}

static void
update_msg(unsigned char* out_data, timespec& ts, int seq)
{
    out_data[4] = (ts.tv_sec >> 0) & 0xff;
    out_data[5] = (ts.tv_sec >> 8) & 0xff;
    out_data[6] = (ts.tv_sec >> 16) & 0xff;
    out_data[7] = (ts.tv_sec >> 24) & 0xff;
    out_data[8] = (ts.tv_nsec >> 0) & 0xff;
    out_data[9] = (ts.tv_nsec >> 8) & 0xff;
    out_data[10] = (ts.tv_nsec >> 16) & 0xff;
    out_data[11] = (ts.tv_nsec >> 24) & 0xff;

    out_data[13] = static_cast<unsigned char>(seq);
}

int
main(int argc, char* argv[], char* envp[])
{
    int baudrate = 1;
    if (argc > 2) {
        int num = atoi(argv[2]);
        if (num > 0) {
            baudrate = num;
        }
    }
    int freq = 1000 * 1000 * 1000 / baudrate;

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
    if (fd == -1) {
        fprintf(stderr,
                "fopen(%s) failed : %s (%d)\n",
                FIFO_LGR2MNG,
                strerror(errno),
                errno);
        return 1;
    }

    // Load data

    unsigned char* data = NULL;
    size_t data_len = 0;
    if (!make_msg(JPEG_FILE, &data, &data_len)) {
        return 1;
    }
    fprintf(stdout, "FIXME data_len:%ld\n", data_len);

    // Write Data

    struct timespec trigger_data;
    clock_gettime(CLOCK_MONOTONIC_RAW, &trigger_data);
    struct timespec trigger_log = trigger_data;
    size_t ok_count = 0;
    int seq = 0;

    while (g_sig_num == 0) {

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);

        if (isTimeUpThenAdd(now, trigger_data, 0, freq)) {

            update_msg(data, now, seq++);

            ssize_t writesize = write(fd, data, data_len);
            if (writesize == -1) {
                fprintf(
                  stderr, "write() failed : %s (%d)\n", strerror(errno), errno);
                break;
            }
            if (static_cast<size_t>(writesize) != data_len) {
                fprintf(stderr, "write(%ld) return %lu\n", data_len, writesize);
                break;
            }
            ok_count++;
        } else {
            usleep(10);
        }

        if (isTimeUpThenAdd(now, trigger_log, 1, 0)) {
            fprintf(stdout, "Send %ld [msg]\n", ok_count);
            ok_count = 0;
        }
    }

    // Destroy

    delete[] data;

    // Close FIFO

    close(fd);

    return 0;
}




