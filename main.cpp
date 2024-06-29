#include <syslog.h>

#include "daemon.h"
#include "handler.h"

int main(int argc, char *argv[]) {

	openlog("crc32_daemon", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);

	crc_files_info info(get_path_to_dir(argc, argv));
	register_signals(get_interrupt_time(argc, argv));

	daemon(info);

	closelog();
	return 0;
}

