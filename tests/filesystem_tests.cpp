#include <gtest/gtest.h>

#include "aw/core/all.h"
#include <filesystem>
#include <ranges>

using namespace aw::core;

TEST(FilesystemTests, FilesVFSTests)
{
	IVirtualFileSystem* vfs = aw_new FilesVFS();
	defer[vfs] { vfs->release(); };

	vfs->map_path("test_assets://", "assets");

	const std::string path = vfs->resolve_path("test_assets://test.txt");
	EXPECT_EQ(path, "assets/test.txt");

	EXPECT_TRUE(std::filesystem::exists(path));

	const std::string file_contents = file::read_file_to_string(path);
	EXPECT_EQ(file_contents, "Hello, this is a test text.");

	const std::vector files = vfs->list_files_in_mapped_directory("test_assets://");
	EXPECT_FALSE(files.empty());
	EXPECT_TRUE(std::ranges::find(files, "test_assets://test.txt") != files.end());
}

TEST(FilesystemTests, TestAWPK)
{
	{
		IVirtualFileSystem* vfs = aw_new FilesVFS();
		defer[vfs] { vfs->release(); };

		vfs->map_path("test_assets://", "assets");

		AwpkArchive* write_archive = awpk::open_for_writing();
		for (const std::string& path : vfs->list_files_in_mapped_directory("test_assets://"))
		{
			awpk::add_file_to_awpk(write_archive, path, vfs->resolve_path(path));
		}

		awpk::write_to_disk(write_archive, "test.awpk");

		AwpkArchive* read_archive = awpk::open_for_reading("test.awpk");
		const auto data = awpk::extract_file_to_string(read_archive, "test_assets://test.txt");
		const auto data2 = awpk::extract_file_to_string(read_archive, "test_assets://test2.txt");

		EXPECT_EQ(data, "Hello, this is a test text.");
		EXPECT_EQ(data2, "This is other test.");
		awpk::close_archive(read_archive);
		awpk::close_archive(write_archive);
	}

	{
		IVirtualFileSystem* vfs = aw_new AwpkVFS("test.awpk");
		defer[vfs] { vfs->release(); };

		IFileReader* reader = vfs->open_file_for_reading("test_assets://test.txt");
		defer[reader] { aw_delete(reader); };

		const std::string string = reader->read_all_to_string();
		EXPECT_EQ(string, "Hello, this is a test text.");
	}

	std::filesystem::remove("test.awpk");
}