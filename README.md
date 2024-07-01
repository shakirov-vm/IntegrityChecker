# Build
```
mkdir build
cd build
cmake ..
cmake --build .
```

# Run
When starting, you must specify the directory and timeout.
You can specify the directory either through the command line argument `--dir` or through the environment variable `DAEMON_DIR`.
You can specify the timeout either through the command line argument `--timeout` or through the environment variable `INTERRUPT_TIME`.
Also, through the command line argument `inotify` you can specify the mode - in the case when `inotify` is specified - integrity checking will occur during some inotify events, otherwise - according to a timeout and the USR1 signal.

Example:
```
./daemon --dir ./path/to/dir --timeout 100 --inotify
```
Stop daemon (only SIGTEM; SIGQUIT, SIGINT, SIGHUP, SIGSTOP, SIGCONT is ignoring):
```
killall -15 daemon
```

# Test with pytest
From ./tests directory:
```
pytest --bin ../build/daemon .
```

# Integrity check
In the current implementation, only regular files are monitored for integrity; directory files are not checked for integrity.

# Inotify
In the inotify case, events are monitored for the creation, modification, deletion, and movement of files. Including the creation of a directory triggers integrity check.

It is worth noting that in the case of inotify, events from notify are not analyzed, the integrity check function is simply called.

# Json dump
In the current implementation, with each integrity check, json is generated in /tmp/crc32_daemon/dump.json