#include <filesystem>
#include <unordered_map>

#pragma once

namespace fs = std::filesystem;

uint32_t crc32_ieee(const uint8_t *data, size_t len);
uint32_t get_crc32_file(const fs::directory_entry& dir_entry);

class crc_files_info {

	std::unordered_map<std::string, uint32_t> files_info;
	fs::path directory;
public:
	crc_files_info(fs::path directory_);
	void update();
	fs::path get_dir() {
		return directory;
	}
};
