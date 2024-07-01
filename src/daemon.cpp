#include <unordered_set>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <syslog.h>
#include <utility>
#include <vector>
#include <sstream>

#include "daemon.h"

uint32_t get_crc32_file(const fs::directory_entry& dir_entry) {

	std::ifstream file(dir_entry.path().string(), std::ifstream::binary);

    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);

	if (length < 0)
		return 0;

    uint8_t *buffer = new uint8_t[length];

    file.read(reinterpret_cast<char *>(buffer), length);
    file.close();

	uint32_t crc32 = crc32_ieee(buffer, length);

	delete[]buffer;

	return crc32;
}

crc_files_info::crc_files_info(fs::path directory_) {

	syslog(LOG_INFO, "Daemon started!");

	directory = directory_;

	for (const auto& dir_entry : fs::recursive_directory_iterator(directory)) {

		if (fs::is_directory(dir_entry.path()))
			continue;

		files_info[dir_entry.path().string()] = get_crc32_file(dir_entry);
	}
}

void crc_files_info::update() {

	std::vector<std::pair<std::string, std::string>> failed_cases;
	json all_cases({"", ""});
	bool ok = true;

	for (auto& it: files_info) {

		if (fs::is_directory(fs::path(it.first)))
			continue;

  		fs::directory_entry dir_entry(fs::path(it.first));

  		if (!fs::exists(fs::path(it.first))) {
			failed_cases.push_back(std::make_pair(dir_entry.path().string(), "ABSENT"));
			ok = false;

			all_cases.push_back({
				{"path", dir_entry.path().string()},
				{"status", "ABSENT"},
			});
			continue;
  		}

		uint32_t new_crc32 = get_crc32_file(dir_entry);
		if (it.second != new_crc32) {
			std::ostringstream out;
			out << "FAIL: " << "old: " << it.second << ", new: " << new_crc32;
			failed_cases.push_back(std::make_pair(dir_entry.path().string(), out.str()));
			ok = false;

			all_cases.push_back({
				{"path", dir_entry.path().string()},
				{"etalon_crc32", it.second},
				{"result_crc32", new_crc32},
				{"status", "FAIL"},
			});
			continue;
		}
		all_cases.push_back({
			{"path", dir_entry.path().string()},
			{"etalon_crc32", it.second},
			{"result_crc32", new_crc32},
			{"status", "OK"},
		});
	}
	for (const auto& dir_entry : fs::recursive_directory_iterator(directory)) {

		if (fs::is_directory(dir_entry.path()))
			continue;

		if (files_info.find(dir_entry.path().string()) == files_info.end()) {
			failed_cases.push_back(std::make_pair(dir_entry.path().string(), "NEW"));
			ok = false;

			all_cases.push_back({
				{"path", dir_entry.path().string()},
				{"status", "NEW"},
			});
		}
	}
	if (ok)
		syslog(LOG_INFO, "Integrity check: OK");
	else {
		syslog(LOG_INFO, "Integrity check: FAIL");
		for (auto& it: failed_cases) {
			syslog(LOG_INFO, "    (%s - %s)", it.first.c_str(), it.second.c_str());
		}
	}
	
  	std::filesystem::create_directory("/tmp/crc32_daemon");

	std::ofstream json_file("/tmp/crc32_daemon/dump.json", std::fstream::out | std::fstream::trunc);
	json_file << all_cases.dump(4);
	json_file.close();
}


