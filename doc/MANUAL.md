# Manual

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