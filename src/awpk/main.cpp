#include <aw/core/all.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include <format>
#include <iostream>

using namespace aw::core;

int main(const int argc, char** argv)
{
	for (i32 i = 0; i < argc; ++i)
	{
		if (argv[i] == std::string_view("-w"))
		{
			std::filesystem::current_path(argv[i + 1]);
		}
	}

	if (!std::filesystem::exists("awpk_manifest.json"))
	{
		return 0;
	}

	const auto vfs = aw_new FilesVFS();
	defer[vfs]
	{
		vfs->release();
	};

	AwpkArchive* archive = awpk::open_for_writing();
	defer[archive]
	{
		awpk::close_archive(archive);
	};

	std::string str = file::read_file_to_string("awpk_manifest.json");
	auto document = nlohmann::json::parse(str);
	if (!document.contains("awpk_path"))
	{
		std::cerr << std::format("Failed to load awpk path. 'awpk_path' is not set in the manifest.\n");
		return -1;
	}

	bool any_files_written = false;

	const auto awpk_path = document["awpk_path"];
	for (const auto directory_mappings = document["directory_mappings"];
		const auto& [key, value] : directory_mappings.items())
	{
		vfs->map_path(key, value.get<std::string>());
		for (const auto& file : vfs->list_files_in_mapped_directory(key))
		{
			awpk::add_file_to_awpk(archive, file, vfs->resolve_path(file));
			any_files_written = true;
		}
	}

	const std::string awpk_path_str = awpk_path.get<std::string>();
	if (std::filesystem::path(awpk_path_str).has_parent_path() && !std::filesystem::exists(std::filesystem::path(awpk_path_str).parent_path()))
	{
		std::filesystem::create_directory(std::filesystem::path(awpk_path_str).parent_path());
	}

	if (any_files_written)
	{
		std::cout << "Writing awpk to " << awpk_path_str << std::endl;
		awpk::write_to_disk(archive, awpk_path.get<std::string>());
	}

	return 0;
}