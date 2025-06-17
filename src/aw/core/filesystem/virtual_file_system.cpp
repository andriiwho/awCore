#include "aw/core/filesystem/virtual_file_system.h"

#include "../../../../include/aw/core/filesystem/files_vfs.h"

namespace aw::core
{
	IVirtualFileSystem* g_vfs = nullptr;

	std::string IVirtualFileSystem::get_virtual_parent_path(std::string_view path)
	{
		if (const auto pos = path.find(s_vfs_divider); pos != std::string_view::npos)
		{
			const std::string_view mapping = path.substr(0, pos + s_vfs_divider.length());
			return std::string(mapping);
		}

		throw std::runtime_error("Invalid path. Must contain '://'.");
	}
} // namespace aw::core