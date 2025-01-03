# How to install?

- `libcamera_mon` use to parse log messages
- `libcamera_proc.ko` create a proc vfs for status update

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

