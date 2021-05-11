#!/usr/bin/env python3
# coding: utf-8

import io
import signal
import time

DATA = "Hello intdash!"
ID = "abc"

FIFO_LGR2MNG = "/var/run/intdash/logger_100.tx"
FIFO_MNG2LGR = "/var/run/intdash/logger_100.rx"

SIGNUM = 0


class Fifo:
    def __init__(self, path):
        self.fifo = io.open(path, 'wb')

    def __del__(self):
        self.fifo.close

    def SendString(self, id, data):

        # Make binary message for String format
        #
        # +---------------+---------------+---------------+---------------+
        # |    Type(01)   |                    Legnth                     |
        # +---------------+---------------+---------------+---------------+
        # |                           Time Sec                            |
        # +---------------+---------------+---------------+---------------+
        # |                           Time NSec                           |
        # +---------------+---------------+---------------+---------------+
        # |     DType     |      Seq      |   ID Length   |      ID 
        # +---------------+---------------+---------------+-----------//--+
        # |   Data String
        # +-----------//--+

        #
        # Type must be 01
        #

        # Make binary msg
        self.fifo.write(bytearray([1]))

        #
        # Length from Time Sec to End of Data String
        #

        # Get Data String length
        data_bytes = data.encode('utf-8')
        data_len = len(data_bytes)

        # Get ID length
        id_bytes = id.encode('utf-8')
        id_len = len(id_bytes)

        # Calc Length
        msg_len = 4 + 4 + 1 + 1 + 1 + id_len + data_len

        # Append binary msg
        self.fifo.write(msg_len.to_bytes(4, 'little')[0:3])

        #
        # Time Sec & Nsec
        #

        # Get CLOCK_MONOTONIC
        now = int(time.clock_gettime(time.CLOCK_MONOTONIC_RAW) * 1_000_000_000)
        sec = int(now / 1_000_000_000)
        nsec = int(now - sec * 1_000_000_000)

        # Append binary msg
        self.fifo.write(sec.to_bytes(4, 'little'))
        self.fifo.write(nsec.to_bytes(4, 'little'))

        #
        # DType of String must be 29
        #

        # Append binary msg
        self.fifo.write(bytearray([29]))

        #
        # Seq must be 0
        #

        # Append binary msg
        self.fifo.write(bytearray([0]))

        #
        # ID Length
        #

        # Append binary msg
        self.fifo.write(bytearray([id_len]))

        #
        # ID
        #

        # Append binary msg
        self.fifo.write(id_bytes)

        #
        # Data String
        #

        # Append binary msg
        self.fifo.write(data_bytes)

        #
        # Flush FIFO
        #
        self.fifo.flush()


if __name__ == '__main__':

    # Setup signal handler

    def signalHandler(signum, frame):
        global SIGNUM
        SIGNUM = signum
    signal.signal(signal.SIGINT, signalHandler)
    signal.signal(signal.SIGTERM, signalHandler)

    # Open FIFO

    fifo = Fifo(FIFO_LGR2MNG)

    # Write Data

    for i in range(20):
        fifo.SendString(ID, DATA)
        time.sleep(0.5)
        if SIGNUM != 0:
            break

    # Waiting for signal

    while SIGNUM == 0:
        time.sleep(0.5)
