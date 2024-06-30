# Integrity check
In the current implementation, only regular files are monitored for integrity; directory files are not checked for integrity.
# Inotify
In the inotify case, events are monitored for the creation, modification, deletion, and movement of files. Including the creation of a directory triggers integrity check.

It is worth noting that in the case of inotify, events from notify are not analyzed, the integrity check function is simply called.
