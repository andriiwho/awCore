#include "../../../../include/aw/core/filesystem/files_vfs.h"

#include "aw/core/filesystem/file.h"

#include <filesystem>
#include <format>

namespace aw::core
{
	void FilesVFS::map_path(const std::string_view mapping, const std::string_view path)
	{
		if (mapping.empty())
		{
			throw std::runtime_error("Invalid mapping name.");
		}

		if (path.empty())
		{
			throw std::runtime_error("Invalid path.");
		}

		if (!mapping.ends_with(s_vfs_divider))
		{
			throw std::runtime_error("Invalid mapping name. Must end with '://'.");
		}

		m_Mappings[std::string(mapping)] = path;
	}

	std::string FilesVFS::resolve_path(const std::string_view path) const
	{
		if (const auto pos = path.find(s_vfs_divider); pos != std::string_view::npos)
		{
			const std::string_view mapping = path.substr(0, pos + s_vfs_divider.length());
			if (const auto iter = m_Mappings.find(std::string(mapping)); iter != m_Mappings.end())
			{
				std::string final_path = std::format("{}/{}", iter->second, path.substr(pos + s_vfs_divider.length()));
				return final_path;
			}
		}

		throw std::runtime_error("Invalid path. Must contain '://'.");
	}

	std::vector<std::string> FilesVFS::list_files_in_mapped_directory(const std::string_view path) const
	{
		const auto resolved_path = resolve_path(path);

		std::vector<std::string> result;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(resolved_path))
		{
			const auto relative_path = std::filesystem::relative(entry.path(), resolved_path);
			result.push_back(std::format("{}{}", path, relative_path.string()));
		}

		return result;
	}

	IFileReader* FilesVFS::open_file_for_reading(const std::string_view path)
	{
		return aw_new DefaultFileReader(resolve_path(path));
	}
} // namespace aw::core