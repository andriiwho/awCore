#include "aw/core/filesystem/virtual_file_system.h"

#include "../../../../include/aw/core/filesystem/files_vfs.h"

namespace aw::core
{
	IVirtualFileSystem* g_vfs = nullptr;
} // namespace aw::core