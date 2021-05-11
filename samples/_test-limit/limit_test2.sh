#!/bin/bash

RUNDIR=/var/run/intdash

mkdir -p $RUNDIR

cat <<MANAGER_CONF >"$RUNDIR/manager.conf.in"
{
  "manager": {
    "ntpserver": "ntp.intdash.jp",
    "raw_autodelete_th": 85,
    "workdirs": [
      "/var/lib/intdash/meas",
      "/var/run/intdash"
    ],
    "rawdir": "/var/lib/intdash/raw",
    "meas_root": "/var/lib/intdash/meas",
    "basetime": "/var/run/intdash/basetime",
    "hooks": [
      {
        "event": "basetime",
        "script": "/opt/vm2m/bin/apt_usbtrx_timesync.sh"
      },
      {
        "event": "busreset",
        "script": "/opt/vm2m/bin/intdash-edge-busreset.sh"
      }
    ],
    "required_space": 90,
    "required_space_raw": 90,
    "stat": "/var/run/intdash/manager.stat",
    "system_stat": "/var/run/intdash/system.stat",
    "process_stat": "/var/run/intdash/process.stat",
    "wwan_stat": "/var/run/intdash/wwan.stat"
  },
  "clients": [
    {
      "connection": {
        "host": "dev.intdash.jp",
        "path": "/api/v1/ws/measurements",
        "port": 443,
        "ssl": "secure",
        "opts": []
      },
      "mode": 1,
      "my_id": "80fdb1fc-ef52-4cf6-ab5a-ec18da6b0302",
      "my_token": "QiQtWE-NdwnD5wZ3CDTg8-rHLJK4bUJLV8tKfNZUPH2Xantp",
      "protocol": "mod_websocket.v2",
      "type": "realtime",
      "user_agent": "",
      "path": "/opt/vm2m/sbin/intdash-edge-client",
      "fifo_rx": "/var/run/intdash/client_realtime.rx",
      "fifo_tx": "/var/run/intdash/client_realtime.tx",
      "stat": "/var/run/intdash/client_realtime.stat"
    },
    {
      "connection": {
        "host": "dev.intdash.jp",
        "path": "/api/v1/measurements",
        "port": 443,
        "ssl": "secure",
        "opts": []
      },
      "fast_net_check_cmd": "/opt/vm2m/bin/intdash-edge-networkd.sh",
      "fast_net_only": false,
      "mode": 1,
      "my_id": "80fdb1fc-ef52-4cf6-ab5a-ec18da6b0302",
      "my_token": "QiQtWE-NdwnD5wZ3CDTg8-rHLJK4bUJLV8tKfNZUPH2Xantp",
      "protocol": "mod_http",
      "resend_cycle": 250,
      "type": "resend",
      "user_agent": "",
      "path": "/opt/vm2m/sbin/intdash-edge-client",
      "fifo_rx": "/var/run/intdash/client_resend.rx",
      "fifo_tx": "/var/run/intdash/client_resend.tx",
      "stat": "/var/run/intdash/client_resend.stat"
    }
  ],
  "loggers": [
    {
      "path": "/opt/vm2m/sbin/intdash-edge-logger",
      "conf": "/var/run/intdash/logger_100.conf",
      "status": "/var/run/intdash/logger_100.stat",
      "connections": [
        {
          "fifo_tx": "/var/run/intdash/logger_100.tx",
          "fifo_rx": "/var/run/intdash/logger_100.rx",
          "channel": 100
        }
      ]
    }
MANAGER_CONF

manager_logger=$(cat <<MANAGER_LOGGER_CONF
    , {
      "path": "/opt/vm2m/sbin/intdash-edge-logger",
      "conf": "/var/run/intdash/logger_CHANNEL.conf",
      "status": "/var/run/intdash/logger_CHANNEL.stat",
      "connections": [
        {
          "fifo_tx": "/var/run/intdash/logger_CHANNEL.tx",
          "fifo_rx": "/var/run/intdash/logger_CHANNEL.rx",
          "channel": CHANNEL
        }
      ]
    }
MANAGER_LOGGER_CONF
)

cat <<LOGGER_CONF >"$RUNDIR/logger.conf.in"
{
  "type": "mjpeg",
  "data_handler": {
    "baudrate": BAUDRATE,
    "camera_height": 1080,
    "camera_hwencodedelay_msec": 100,
    "camera_width": 1920,
    "path": "FILEPATH"
  },
  "manager_client": {
    "tx_path": "/var/run/intdash/logger_CHANNEL.tx",
    "rx_path": "/var/run/intdash/logger_CHANNEL.rx"
  },
  "status": "/var/run/intdash/logger_CHANNEL.stat",
  "ntpserver": "ntp.intdash.jp",
  "basetime": "/var/run/intdash/basetime"
}
LOGGER_CONF


for dev_count in 0 1 2; do
  cp "$RUNDIR/manager.conf.in" "$RUNDIR/manager.conf"
  for channel in $(seq 101 $((100+${dev_count}))); do
    echo $manager_logger | sed -e "s/CHANNEL/${channel}/g" >>"$RUNDIR/manager.conf"
  done
  echo "]}" >>"$RUNDIR/manager.conf"
  cp "$RUNDIR/manager.conf" "$RUNDIR/manager${dev_count}.conf"
  for channel in $(seq 100 $((100+${dev_count}))); do
    sed -e "s/BAUDRATE/4000/" -e "s/FILEPATH/\/opt\/vm2m\/etc\/small.jpg/" -e "s/CHANNEL/${channel}/" "$RUNDIR/logger.conf.in" > "$RUNDIR/logger_${channel}.conf"
  done
  vmstat 1 >small-${dev_count}.vmstat &
  vmstat_pid=$!
  LD_LIBRARY_PATH=/opt/vm2m/lib /opt/vm2m/sbin/intdash-edge-manager -C $RUNDIR/manager.conf >small-${dev_count}.log 2>&1 &
  sleep 25
  /opt/vm2m/sbin/intdash-edge-manager -k
  kill $vmstat_pid
done

#for baudrate in 1 2 4 8 16 32; do
#  sed -e "s/BAUDRATE/$baudrate/" -e "s/FILEPATH/\/opt\/vm2m\/etc\/large.jpg/" "$RUNDIR/logger.conf" > "$RUNDIR/logger_101.conf"
#  LD_LIBRARY_PATH=/opt/vm2m/lib /opt/vm2m/sbin/intdash-edge-manager -C $RUNDIR/manager.conf >large-$baudrate.log 2>&1 &
#  sleep 10
#  /opt/vm2m/sbin/intdash-edge-manager -k
#  CHANNEL=$((CHANNEL+1))
#done

