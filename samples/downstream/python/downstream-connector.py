#!/usr/bin/env python3
# coding: utf-8

import os
import select
import struct
import signal

FIFO_LGR2MNG = "/var/run/intdash2/logger_100.tx"
FIFO_MNG2LGR = "/var/run/intdash2/logger_100.rx"

SIGNUM = 0

def signalHandler(signum, frame):
  global SIGNUM
  SIGNUM = signum

# Packat format (type: String)
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

class StringPacket():
  def __init__(self, pkt) -> None:
    offset = 0

    # Type
    type_size = 1
    (self.__type,) = struct.unpack_from('B', pkt, offset)
    offset += type_size

    # Length
    pkt_len_size = 3
    (pkt_len, ) = struct.unpack_from('<3s', pkt, offset)
    pkt_len += bytes([0])
    (self.__pkt_len,) = struct.unpack('<L', pkt_len)
    offset += pkt_len_size

    # Time Sec & Nsec
    sec_size = 4
    (self.__sec,) = struct.unpack_from('<L', pkt, offset)
    offset += sec_size
    nsec_size = 4
    (self.__nsec,) = struct.unpack_from('<L', pkt, offset)
    offset += nsec_size

    # DType
    dtype_size = 1
    (self.__dtype,) = struct.unpack_from('B', pkt, offset)
    offset += dtype_size

    # Seq
    seq_size = 1
    (self.__seq,) = struct.unpack_from('B', pkt, offset)
    offset += seq_size

    # ID Length & ID
    id_len_size = 1
    (self.__id_len,) = struct.unpack_from('B', pkt, offset)
    offset += id_len_size
    (self.__id,) = struct.unpack_from(str(self.__id_len) + 's', pkt, offset)
    offset += self.__id_len

    # Calc Header Length
    header_size = sec_size + nsec_size + dtype_size + seq_size

    # Data String
    data_len = self.__pkt_len - header_size - id_len_size - self.__id_len
    (self.__data,) = struct.unpack_from(str(data_len) + 's', pkt, offset)
    offset += data_len

    self.__pkt_size = offset

  @property
  def size(self):
    return self.__pkt_size

  @property
  def type(self):
    return self.__type

  @property
  def sec(self):
    return self.__sec

  @property
  def nsec(self):
    return self.__nsec

  @property
  def seq(self):
    return self.__seq

  @property
  def dtype(self):
    return self.__dtype

  @property
  def id(self):
    return self.__id.decode()

  @property
  def data(self):
    return self.__data


if __name__ == '__main__':
  # Setup signal handler
  signal.signal(signal.SIGINT, signalHandler)
  signal.signal(signal.SIGTERM, signalHandler)

  while SIGNUM == 0:
    # Open FIFO
    with os.fdopen(os.open(FIFO_MNG2LGR, os.O_RDONLY | os.O_NONBLOCK), 'rb') as fifo:
      # Main Loop
      while SIGNUM == 0:
        # Wait Data
        rready, _, _ = select.select([fifo], [], [], 1)
        if SIGNUM != 0:
          break
        if not rready:
          continue

        # Read Data
        data = fifo.read()
        while len(data) != 0:
          # Parse Data
          pkt = StringPacket(data)
          print('Timestamp: {0}.{1:0=9}, id: {2}, data: {3}'.format(pkt.sec, pkt.nsec, pkt.id, pkt.data))

          # Next Data
          tempdat = bytearray(data)
          del tempdat[:pkt.size]
          data = bytes(tempdat)

