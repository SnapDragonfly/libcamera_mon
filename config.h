#ifndef CONFIG_H
#define CONFIG_H

#define RING_BUFFER_SIZE        1024                // Buffer size for the ring

#define FIFO_BUFFER_SIZE        64                  // Buffer size for reading
#define PROC_BUFFER_SIZE        128                 // Buffer size for writing
#define REQUIRED_DATA_SIZE      256                 // The minimum data size needed to parse data

#define DEFAULT_PATH_LENGTH     256                         // Path length for dir/file
#define DIR_PATH               "/tmp/libcamera/"            // libcamera-mon dir
#define FIFO_PATH              "/tmp/libcamera/logs"        // Path to the FIFO file
#define PROC_PATH              "/proc/libcamera_proc"       // Path to the proc fps file

#define SAMPLING_METHOD         0                   // 0: arithmetic+filter; 1: raw+filter; 2 arithmetic; 3 raw
#define SAMPLING_FREQUENCY      30                  // sampling frequency (Hz)
#define CUTOFF_FREQUENCY        5                   // cutoff frequency (Hz)   

#endif // CONFIG_H