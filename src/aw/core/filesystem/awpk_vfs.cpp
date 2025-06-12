#include "aw/core/filesystem/awpk_vfs.h"

#include "aw/core/filesystem/awpk.h"
#include "aw/core/filesystem/file.h"

namespace aw::core
{

	AwpkVFS::AwpkVFS(const std::string_view awpk_path)
		: m_Archive(awpk::open_for_reading(awpk_path))
	{
	}

	AwpkVFS::~AwpkVFS()
	{
		awpk::close_archive(m_Archive);
	}

	void AwpkVFS::map_path(std::string_view mapping, std::string_view path)
	{
		m_Mappings.emplace(mapping, path);
	}

	std::string AwpkVFS::resolve_path(const std::string_view path) const
	{
		return std::string(path);
	}

	std::vector<std::string> AwpkVFS::list_files_in_mapped_directory(std::string_view path) const
	{
		return awpk::list_files_in_archive(m_Archive);
	}

	IFileReader* AwpkVFS::open_file_for_reading(const std::string_view path)
	{
		return aw_new AwpkFileReader(m_Archive, path);
	}
} // namespace aw::core