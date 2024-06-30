#include <iostream>
#include <atomic>
#include <sstream>
#include <csignal>
#include <sys/time.h>
#include <unistd.h>
#include <syslog.h>

#include <sys/inotify.h>

#include "handler.h"

#define EXPECTED_NUM_ARGS 3
#define ENV_TARGET_DIRECTORY "DAEMON_DIR"
#define ENV_INTERRUPT_TIME "INTERRUPT_TIME"
#define INOTIFY_BUFFER_SIZE 4096
#define INOTIFY_FLAGS (IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO)

// maybe create pthread_sigmask + sigwait?
// FIXME Global values? need std::atomic
std::atomic_int queue = 0;

void signal_usr1_handler(int signum) {

	++queue;
}

void signal_term_handler(int signum) {

	std::exit(0);
}

void signal_timer_handler(int signum) {

	++queue;
}

void register_signals(int seconds) {

	std::signal(SIGUSR1, signal_usr1_handler);
	std::signal(SIGQUIT, SIG_IGN);
	std::signal(SIGINT, SIG_IGN);
	std::signal(SIGHUP, SIG_IGN);
	std::signal(SIGSTOP, SIG_IGN);
	std::signal(SIGCONT, SIG_IGN);
	std::signal(SIGTERM, signal_term_handler);

	std::signal(SIGALRM, signal_timer_handler);
	struct itimerval it_val;

	it_val.it_value.tv_sec = seconds;
	it_val.it_value.tv_usec = 0;   
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		syslog(LOG_INFO, "Error when calling setitimer()!");
		std::exit(1);
	}
}

// Rewrite with common string and parameters
fs::path get_path_to_dir(int argc, char *argv[]) {

	fs::path path_to_dir;

	if (argc != EXPECTED_NUM_ARGS) {
		const char* env_path_to_dir = std::getenv(ENV_TARGET_DIRECTORY);

		if (env_path_to_dir != nullptr)
	      	path_to_dir = fs::path(env_path_to_dir);
	    else {
			syslog(LOG_INFO, "The path to the directory was not entered!");
			std::exit(1);
	    }
	} else 
		path_to_dir = fs::path(argv[1]);

	if (!fs::exists(path_to_dir)) {		
		syslog(LOG_INFO, "Directory <%s> does not exist!", path_to_dir.string().c_str());
		std::exit(1);
	}

	return path_to_dir;
}

int get_interrupt_time(int argc, char *argv[]) {

	int seconds = 0;

	if (argc != EXPECTED_NUM_ARGS) {
		const char* env_interrupt_time = std::getenv(ENV_INTERRUPT_TIME);

		if (env_interrupt_time != nullptr) {

			std::stringstream s(env_interrupt_time);
			s >> seconds;
		}
	    else {
			syslog(LOG_INFO, "Interrupt timeout was not entered!");
			std::exit(1);
	    }
	} else {
		std::stringstream s(argv[2]);
		s >> seconds;
	}

	if (!seconds) {
		syslog(LOG_INFO, "Interrupt timeout was parsed as 0, this is an incorrect timeout");
		std::exit(1);
	}

	return seconds;
}

void daemon(crc_files_info& info) {

	while (true) {
		while(queue) {
			info.update();
			--queue;
		}
		pause();
	}
}

void inotify_daemon(crc_files_info& info) {

	int inotify_fd = inotify_init();
	if (inotify_fd < 0) {
		syslog(LOG_INFO, "Inotify initialization failed");
		std::exit(1);
	}

	int wd = inotify_add_watch(inotify_fd, info.get_dir().string().c_str(), INOTIFY_FLAGS);
	if (wd < 0) {
		syslog(LOG_INFO, "Inotify watch create failed");
		std::exit(1);
	}

	while (true) {

		char buffer[INOTIFY_BUFFER_SIZE];

		ssize_t r = read(inotify_fd, buffer, INOTIFY_BUFFER_SIZE);
		if (r < 0) {
			break;
		} 
		info.update();
	}

	inotify_rm_watch(inotify_fd, wd);
	close(inotify_fd);
}