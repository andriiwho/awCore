#include "aw/core/filesystem/awpk.h"

#include "aw/core/filesystem/virtual_file_system.h"
#include "aw/core/memory/paged_memory_pool.h"
#include "aw/core/primitive/numbers.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <mutex>

namespace aw::core
{
	struct AwpkHeader
	{
		char magic[4] = { 'A', 'W', 'P', 'K' };
		u32 version = 1;
		u64 num_files = 0;
		u64 index_offset = 0;
	};

	struct AwpkFileEntry
	{
		char filename[256];
		u64 offset = 0;
		u64 size = 0;
	};

	class AwpkArchive
	{
	public:
		AwpkArchive()
			: m_IsWriting(true)
		{
		}

		explicit AwpkArchive(const std::string_view path)
			: m_Path(path)
			, m_IsWriting(false)
		{
			std::ifstream in(path.data(), std::ios::binary);
			if (!in.is_open())
			{
				throw std::runtime_error("Failed to open .awpk file for reading.");
			}

			AwpkHeader header{};
			in.read(reinterpret_cast<char*>(&header), sizeof(AwpkHeader));
			in.seekg(header.index_offset);

			m_ReadFiles.resize(header.num_files);
			in.read(reinterpret_cast<char*>(m_ReadFiles.data()), header.num_files * sizeof(AwpkFileEntry));

			m_AwpkStream = std::move(in);
			m_AwpkStream.seekg(0, std::ios::beg);

			for (const auto& entry : m_ReadFiles)
			{
				m_ReadMappings[entry.filename] = &entry;

				std::string_view filename = entry.filename;
				if (const auto pos = path.find(s_vfs_divider); pos != std::string_view::npos)
				{
					const std::string_view mapping = filename.substr(0, pos + s_vfs_divider.length());
					m_FilesPerDirectory[std::string(mapping)].push_back(&entry);
				}
			}
		}

		void add_file_for_write(const std::string_view mapping, const std::string_view path)
		{
			m_WriteFileMappings[std::string(mapping)] = std::string(path);
		}

		std::vector<std::byte> extract_file(const std::string_view name)
		{
			if (m_IsWriting)
			{
				throw std::runtime_error("Cannot extract file. This archive is not open for reading.");
			}

			if (const auto it = m_ReadMappings.find(name); it != m_ReadMappings.end())
			{
				const auto& entry = *it->second;

				std::lock_guard lock(m_Mutex);
				m_AwpkStream.seekg(entry.offset);
				std::vector<std::byte> result(entry.size);
				m_AwpkStream.read(reinterpret_cast<char*>(result.data()), entry.size);
				return result;
			}

			throw std::runtime_error("File not found.");
		}

		void write_to_disk(const std::string_view path)
		{
			if (!m_IsWriting)
			{
				throw std::runtime_error("Cannot write to disk. This archive is not open for writing.");
			}

			std::ofstream out(path.data(), std::ios::binary);
			if (!out.is_open())
			{
				throw std::runtime_error("Failed to open .awpk file for writing.");
			}

			AwpkHeader header{};
			header.num_files = m_WriteFileMappings.size();

			std::vector<AwpkFileEntry> index{};
			index.reserve(m_WriteFileMappings.size());

			out.seekp(sizeof(AwpkHeader));
			std::ostringstream data_stream{};

			usize current_offset = sizeof(AwpkHeader);
			for (auto& [mapping, read_path] : m_WriteFileMappings)
			{
				std::ifstream in(read_path, std::ios::binary | std::ios::ate);
				if (!in.is_open())
				{
					throw std::runtime_error("Failed to open file for writing.");
				}

				const auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				std::vector<char> buffer(size);
				in.read(buffer.data(), size);

				AwpkFileEntry entry{};
				memcpy(entry.filename, mapping.data(), mapping.size());
				memset(entry.filename + mapping.size(), 0, sizeof(entry.filename) - mapping.size());
				entry.offset = current_offset;
				entry.size = size;
				index.push_back(entry);

				data_stream.write(buffer.data(), size);
				current_offset += size;
			}

			header.index_offset = out.tellp() + data_stream.tellp();
			out << data_stream.str();

			for (const auto& entry : index)
			{
				out.write(reinterpret_cast<const char*>(&entry), sizeof(AwpkFileEntry));
			}

			// Write header at the beginning
			out.seekp(0);
			out.write(reinterpret_cast<const char*>(&header), sizeof(AwpkHeader));

			out.close();
		}

		std::vector<std::string> list_files() const
		{
			if (m_IsWriting)
			{
				throw std::runtime_error("Failed to list files. Archive is not open for reading.");
			}

			std::vector<std::string> out;
			out.reserve(m_ReadFiles.size());

			for (const auto& entry : m_ReadFiles)
			{
				out.push_back(entry.filename);
			}

			return out;
		}

		std::vector<std::string> list_files(const std::string_view directory) const
		{
			if (m_IsWriting)
			{
				throw std::runtime_error("Failed to list files. Archive is not open for reading.");
			}

			std::vector<std::string> out;
			out.reserve(m_ReadFiles.size());

			if (const auto iter = m_FilesPerDirectory.find(std::string(directory)); iter != m_FilesPerDirectory.end())
			{
				for (const auto& entry : iter->second)
				{
					out.push_back(entry->filename);
				}
			}

			return out;
		}

		bool exists(const std::string_view path) const
		{
			if (m_IsWriting)
			{
				throw std::runtime_error("Failed to check if file exists. Archive is not open for reading.");
			}

			return m_ReadMappings.contains(path);
		}

	private:
		std::string m_Path{};
		std::ifstream m_AwpkStream{};

		std::unordered_map<std::string, std::string> m_WriteFileMappings{};
		std::vector<AwpkFileEntry> m_ReadFiles{};
		std::unordered_map<std::string_view, const AwpkFileEntry*> m_ReadMappings{};
		bool m_IsWriting = false;

		std::unordered_map<std::string, std::vector<const AwpkFileEntry*>> m_FilesPerDirectory{};

		std::mutex m_Mutex{};
	};

	AwpkArchive* awpk::open_for_writing()
	{
		return aw_new AwpkArchive();
	}

	AwpkArchive* awpk::open_for_reading(const std::string_view path)
	{
		return aw_new AwpkArchive(path);
	}


	void awpk::add_file_to_awpk(AwpkArchive* archive, const std::string_view mapping, const std::string_view path)
	{
		archive->add_file_for_write(mapping, path);
	}

	void awpk::write_to_disk(AwpkArchive* archive, const std::string_view path)
	{
		archive->write_to_disk(path);
	}

	void awpk::close_archive(AwpkArchive* archive)
	{
		aw_delete(archive);
	}

	std::vector<std::byte> awpk::extract_file(AwpkArchive* archive, const std::string_view name)
	{
		return archive->extract_file(name);
	}

	std::vector<std::string> awpk::list_files_in_archive(const AwpkArchive* archive)
	{
		return archive->list_files();
	}

	std::vector<std::string> awpk::list_files_in_directory(const AwpkArchive* archive, const std::string_view directory)
	{
		return archive->list_files(directory);
	}

	bool awpk::file_exists(const AwpkArchive* archive, const std::string_view name)
	{
		return archive->exists(name);
	}
} // namespace aw::core