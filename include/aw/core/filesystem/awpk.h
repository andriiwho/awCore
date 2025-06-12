#pragma once

#include <string_view>
#include <vector>

namespace aw::core
{
	class IVirtualFileSystem;
	class AwpkArchive;

	namespace awpk
	{
		AwpkArchive* open_for_reading(std::string_view path);
		AwpkArchive* open_for_writing();
		void close_archive(AwpkArchive* archive);

		void add_file_to_awpk(AwpkArchive* archive, std::string_view mapping, std::string_view path);

		std::vector<std::byte> extract_file(AwpkArchive* archive, std::string_view name);

		inline std::string extract_file_to_string(AwpkArchive* archive, const std::string_view name)
		{
			const auto bytes = extract_file(archive, name);
			return std::string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
		}

		void write_to_disk(AwpkArchive* archive, std::string_view path);
		std::vector<std::string> list_files_in_archive(const AwpkArchive* archive);
		std::vector<std::string> list_files_in_directory(const AwpkArchive* archive, std::string_view directory);

	} // namespace awpk
} // namespace aw::core