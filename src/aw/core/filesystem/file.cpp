#include "aw/core/filesystem/file.h"

#include "aw/core/filesystem/awpk.h"
#include "aw/core/primitive/defer.h"

#include <format>
#include <sstream>

namespace aw::core
{
	DefaultFileReader::DefaultFileReader(const std::string_view path, const bool binary)
		: m_Path(path)
		, m_Stream(m_Path, (binary ? std::ios::binary : std::ios::in) | std::ios::ate)
	{
		if (!m_Stream.is_open())
		{
			throw std::runtime_error(std::format("Failed to load file: {}", path));
		}

		const auto pos = m_Stream.tellg();
		m_Stream.seekg(0, std::ios::beg);
		m_Size = static_cast<usize>(pos);
	}

	DefaultFileReader::~DefaultFileReader()
	{
	}

	std::string DefaultFileReader::read_all_to_string() const
	{
		std::stringstream stream;
		stream << m_Stream.rdbuf();
		reset_pointer();
		return stream.str();
	}

	std::string DefaultFileReader::read_next_line() const
	{
		std::string out;
		std::getline(m_Stream, out);
		return out;
	}

	void DefaultFileReader::reset_pointer() const
	{
		m_Stream.seekg(0, std::ios::beg);
	}

	std::vector<std::byte> DefaultFileReader::read_binary() const
	{
		std::vector<std::byte> result(m_Size);
		m_Stream.read(reinterpret_cast<char*>(result.data()), m_Size);
		return result;
	}

	AwpkFileReader::AwpkFileReader(AwpkArchive* archive, std::string_view path)
		: m_Archive(archive)
		, m_Path(path)
		, m_Bytes(awpk::extract_file(archive, path))
	{
	}

	std::string AwpkFileReader::read_next_line() const
	{
		std::stringstream line{};
		for (usize i = m_Offset; i < m_Bytes.size(); ++i)
		{
			while (static_cast<char>(m_Bytes[i]) != '\n')
			{
				line << static_cast<char>(m_Bytes[i]);
				m_Offset++;
			}
		}
		return line.str();
	}

	std::string AwpkFileReader::read_all_to_string() const
	{
		return std::string(reinterpret_cast<const char*>(m_Bytes.data()), m_Bytes.size());
	}

	void AwpkFileReader::reset_pointer() const
	{
		m_Offset = 0;
	}

	std::vector<std::byte> AwpkFileReader::read_binary() const
	{
		return m_Bytes;
	}

	DefaultFileWriter::DefaultFileWriter(const std::string_view path, const bool binary)
		: m_Path(path)
		, m_Stream(m_Path, binary ? std::ios::binary : std::ios::out)
	{
		if (!m_Stream.is_open())
		{
			throw std::runtime_error(std::format("Failed to load file: {}", path));
		}
	}

	DefaultFileWriter::~DefaultFileWriter()
	{
		m_Stream.flush();
	}

	void DefaultFileWriter::write_as_string(const std::string_view data) const
	{
		m_Stream << data;
	}

	void DefaultFileWriter::write_as_binary(const void* data, const usize size) const
	{
		m_Stream.write(static_cast<const char*>(data), size);
	}

	std::string file::read_file_to_string(const std::string_view path)
	{
		return DefaultFileReader(path).read_all_to_string();
	}

	std::future<std::string> file::read_file_to_string_async(std::string_view path, ThreadPool* thread_pool)
	{
		if (!thread_pool)
		{
			thread_pool = g_global_thread_pool;
			if (!thread_pool)
			{
				throw std::runtime_error("g_global_thread_pool is not initialized. Use aw::core::initialize_global_thread_pool() to initialize it.");
			}
		}

		return thread_pool->submit_task([path] {
			return read_file_to_string(path);
		});
	}

	void file::write_file_from_string(const std::string_view path, const std::string_view data)
	{
		DefaultFileWriter(path).write_as_string(data);
	}

	std::future<void> file::write_file_from_string_async(const std::string_view path, std::string_view data, ThreadPool* thread_pool)
	{
		if (!thread_pool)
		{
			thread_pool = g_global_thread_pool;
			if (!thread_pool)
			{
				throw std::runtime_error("g_global_thread_pool is not initialized. Use aw::core::initialize_global_thread_pool() to initialize it.");
			}
		}

		return thread_pool->submit_task([path_str = std::string(path), data] {
			write_file_from_string(path_str, data);
		});
	}

	std::vector<std::byte> file::read_file_to_binary(const std::string_view path)
	{
		return DefaultFileReader(path, true).read_binary();
	}

	std::future<std::vector<std::byte>> file::read_file_to_binary_async(const std::string_view path, ThreadPool* thread_pool)
	{
		if (!thread_pool)
		{
			thread_pool = g_global_thread_pool;
			if (!thread_pool)
			{
				throw std::runtime_error("g_global_thread_pool is not initialized. Use aw::core::initialize_global_thread_pool() to initialize it.");
			}
		}

		return thread_pool->submit_task([path_str = std::string(path)] {
			return read_file_to_binary(path_str);
		});
	}

	void file::write_file_from_binary(const std::string_view path, const void* data, const usize size)
	{
		DefaultFileWriter(path, true).write_as_binary(data, size);
	}

	std::future<void> file::write_file_from_binary_async(std::string_view path, const void* data, usize size, ThreadPool* thread_pool)
	{
		if (!thread_pool)
		{
			thread_pool = g_global_thread_pool;
			if (!thread_pool)
			{
				throw std::runtime_error("g_global_thread_pool is not initialized. Use aw::core::initialize_global_thread_pool() to initialize it.");
			}
		}

		// Copy data for memory safety
		void* out_data = allocate_memory(size);
		memcpy(out_data, data, size);
		return thread_pool->submit_task([path_str = std::string(path), out_data, size] {
			defer[out_data]
			{
				free_memory(out_data);
			};
			write_file_from_binary(path_str, out_data, size);
		});
	}
} // namespace aw::core