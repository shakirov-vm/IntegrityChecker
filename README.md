# Integrity check
In the current implementation, only regular files are monitored for integrity; directory files are not checked for integrity.

# Inotify
In the inotify case, events are monitored for the creation, modification, deletion, and movement of files. Including the creation of a directory triggers integrity check.

It is worth noting that in the case of inotify, events from notify are not analyzed, the integrity check function is simply called.

# Run
When starting, you must specify the directory and timeout.
You can specify the directory either through the command line argument `--dir` or through the environment variable `DAEMON_DIR`.
You can specify the timeout either through the command line argument `--timeout` or through the environment variable `INTERRUPT_TIME`.
Also, through the command line argument `inotify` you can specify the mode - in the case when `inotify` is specified - integrity checking will occur during some inotify events, otherwise - according to a timeout and the USR1 signal.

Example:
```
./daemon --dir path/to/dir --timeout 100 --inotify
```
