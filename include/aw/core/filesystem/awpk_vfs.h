#pragma once

#include "virtual_file_system.h"

#include <unordered_map>

namespace aw::core
{
	class AwpkArchive;
	class AwpkVFS final : public IVirtualFileSystem
	{
	public:
		AwpkVFS(std::string_view awpk_path);
		~AwpkVFS() override;

		void map_path(std::string_view mapping, std::string_view path) override;
		std::string resolve_path(std::string_view path) const override;
		std::vector<std::string> list_files_in_mapped_directory(std::string_view path) const override;
		IFileReader* open_file_for_reading(std::string_view path) override;

	private:
		AwpkArchive* m_Archive{};
		std::unordered_map<std::string, std::string> m_Mappings;
	};
}