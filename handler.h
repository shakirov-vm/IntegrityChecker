
#include "daemon.h"

void register_signals(int seconds);
fs::path get_path_to_dir(int argc, char *argv[]);
int get_interrupt_time(int argc, char *argv[]);
void daemon(crc_files_info& info);