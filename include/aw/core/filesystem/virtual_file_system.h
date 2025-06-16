#pragma once

#include "aw/core/memory/intrusive_ref_counted.h"
#include <string_view>
#include <string>
#include <vector>

namespace aw::core
{
}
namespace aw::core
{
	inline constexpr std::string_view s_vfs_divider = "://";

	class IFileReader;

	class IVirtualFileSystem : public IntrusiveRefCounted
	{
	public:
		virtual void map_path(std::string_view mapping, std::string_view path) = 0;
		virtual std::string resolve_path(std::string_view path) const = 0;
		virtual std::vector<std::string> list_files_in_mapped_directory(std::string_view path) const = 0;

		virtual IFileReader* open_file_for_reading(std::string_view path) = 0;
		virtual bool file_exists(std::string_view path) const = 0;
	};

	extern IVirtualFileSystem* g_vfs;
}

#define aw_init_global_files_vfs() g_vfs = aw_new aw::core::FilesVFS()
#define aw_init_global_awpk_vfs(awpk_path) g_vfs = aw_new aw::core::AwpkVFS(awpk_path)