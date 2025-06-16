#pragma once

#include "aw/core/filesystem/virtual_file_system.h"

#include <unordered_map>

namespace aw::core
{
	class FilesVFS final : public IVirtualFileSystem
	{
	public:
		void map_path(std::string_view mapping, std::string_view path) override;
		std::string resolve_path(std::string_view path) const override;
		std::vector<std::string> list_files_in_mapped_directory(std::string_view path) const override;

		IFileReader* open_file_for_reading(std::string_view path) override;
		bool file_exists(std::string_view path) const override;

		void try_init_mappings_from_awpk_manifest();

	private:
		std::unordered_map<std::string, std::string> m_Mappings;
	};
	//
}