# libcamera_mon

libcamera-vid monitor tool, which parses log message to get FPS/bitrate etc on raspberry pi system.


`libcamera-vid`  -- pipe --> `libcamera_mon`  -- proc --> `/proc/libcamera_proc`


# How to build

```
$ git clone https://github.com/SnapDragonfly/libcamera_mon.git
$ cd libcamera_mon
$ make
$ tree ./build/
./build/
├── libcamera_mon
├── libcamera_proc.ko
├── libcamera_proc.mod
├── libcamera_proc.mod.c
├── libcamera_proc.mod.o
├── libcamera_proc.o
├── modules.order
└── Module.symvers

1 directory, 8 files
```

# How to run

There will be an example for simple setup.

```
$ ./build/libcamera_mon --help
Usage: ./build/libcamera_mon [options]
Options:
  --buffer-size <value>        Set the buffer size (default: 64)
  --required-data-size <value> Set the required data size (default: 256)
  --fifo-path <path>           Set the FIFO file path (default: /tmp/libcamera/logs)
  --sampling-method <value>    Set the sampling method (default: 0)
                               0: arithmetic+filter
                               1: raw+filter
                               2: arithmetic
                               3: raw
  --sample-frequency <value>   Set the sampling frequency in Hz (default: 30)
  --cutoff-frequency <value>   Set the cutoff frequency in Hz (default: 5)
  --help                       Show this help message

```


# Example

1. check `libcamera_proc` running

If Not sure what exactly module is running, then just `rmmod`.

```
$ lsmod | grep libcamera_proc
libcamera_proc         12288  0

$ sudo rmmod libcamera_proc
```

2. run `libcamera_proc` for data exchange with other program

```
$ sudo insmod ./build/libcamera_proc.ko
```
3. run `libcamera_mon`

Make sure libcamera-vid is NOT running.

```
$ ./build/libcamera_mon
Configuration:
  Buffer size: 64
  Required data size: 256
  FIFO path: /tmp/libcamera/logs
  Sampling frequency: 30.00 Hz
  Cutoff frequency: 5.00 Hz
  Sampling method: 0
    (0: arithmetic+filter, 1: raw+filter, 2: arithmetic, 3: raw)
Calculated alpha: 0.1429
Path '/tmp/libcamera/' already exists.
Path '/tmp/libcamera/logs' already exists.
FIFO file '/tmp/libcamera/logs' opened successfully. Waiting for input...
PROC file '/proc/libcamera_proc' opened successfully. Waiting for write...
```

4. execute `libcamera-vid`, make sure to use `--verbose`, then stream the log to pip `/tmp/libcamera/logs`

```
$ libcamera-vid \
  --verbose \
  --inline \
  --width 1920 \
  --height 1080 \
  --bitrate 4000000 \
  --framerate 30 \
  --hflip \
  --vflip \
  --timeout 0 \
  -o - 2> /tmp/libcamera/logs | \
  /usr/bin/gst-launch-1.0 \
    -v fdsrc ! h264parse ! rtph264pay config-interval=1 pt=35 ! udpsink sync=false host=127.0.0.1 port=5602
```

5. `libcamera_mon` will get data as follows:

```
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
No more data. Waiting for libcamera log message...
....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
.....Failed(-2) to parse data. Discarding all data in the ring buffer.
........68: frame=127, fps=-0.00, size=127, bitrate=filtered(-0.00, -0.00) Mbps
...........68: frame=127, fps=-0.00, size=127, bitrate=filtered(-0.00, -0.00) Mbps
...........67: frame=-2, fps=30.01, size=20904, bitrate=filtered(0.72, 5.02) Mbps
............67: frame=-2, fps=30.01, size=17023, bitrate=filtered(1.20, 4.09) Mbps
.........67: frame=-2, fps=30.01, size=18952, bitrate=filtered(1.68, 4.55) Mbps
.............66: frame=-2, fps=30.01, size=8984, bitrate=filtered(1.75, 2.16) Mbps
..........67: frame=-2, fps=30.01, size=15932, bitrate=filtered(2.05, 3.82) Mbps
..........67: frame=-2, fps=30.01, size=15932, bitrate=filtered(2.30, 3.82) Mbps
........67: frame=-2, fps=30.01, size=15932, bitrate=filtered(2.52, 3.82) Mbps
............67: frame=-2, fps=30.01, size=15932, bitrate=filtered(2.70, 3.82) Mbps
...........67: frame=-2, fps=30.01, size=15932, bitrate=filtered(2.86, 3.82) Mbps
..........67: frame=-2, fps=30.01, size=19233, bitrate=filtered(3.12, 4.62) Mbps
........67: frame=-2, fps=30.01, size=19233, bitrate=filtered(3.33, 4.62) Mbps
........67: frame=-2, fps=30.01, size=16876, bitrate=filtered(3.43, 4.05) Mbps
........67: frame=-2, fps=30.01, size=16876, bitrate=filtered(3.52, 4.05) Mbps
........67: frame=-2, fps=30.01, size=16876, bitrate=filtered(3.60, 4.05) Mbps
........67: frame=-2, fps=29.99, size=16491, bitrate=filtered(3.65, 3.96) Mbps
...........67: frame=-2, fps=29.99, size=16491, bitrate=filtered(3.69, 3.96) Mbps
.........67: frame=-2, fps=30.01, size=15898, bitrate=filtered(3.71, 3.82) Mbps
.........67: frame=-2, fps=30.01, size=15584, bitrate=filtered(3.72, 3.74) Mbps
.............67: frame=-2, fps=30.01, size=15584, bitrate=filtered(3.72, 3.74) Mbps
............67: frame=-2, fps=30.01, size=15485, bitrate=filtered(3.72, 3.72) Mbps
...........67: frame=-2, fps=30.01, size=15814, bitrate=filtered(3.73, 3.80) Mbps
........67: frame=-2, fps=30.01, size=26604, bitrate=filtered(4.11, 6.39) Mbps
```

6. we can get result from proc system

```
$ cat /proc/libcamera_proc
frame=-2, fps=30.01, size=15932, bitrate=filtered(2.05, 3.82) Mbps
```

*Note: There is an `update.sh` loop `cat` messages.*
