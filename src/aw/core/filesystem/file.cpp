#include "aw/core/filesystem/file.h"

#include "aw/core/primitive/defer.h"

#include <format>

namespace aw::core
{

	FileReader::FileReader(const std::string_view path, const bool binary)
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

	FileReader::~FileReader()
	{
	}

	std::string FileReader::read_all_to_string() const
	{
		std::string result;
		result.resize(m_Size);
		m_Stream.read(&result[0], m_Size);
		reset_pointer();
		return result;
	}

	std::string FileReader::read_next_line() const
	{
		std::string out;
		std::getline(m_Stream, out);
		return out;
	}

	void FileReader::reset_pointer() const
	{
		m_Stream.seekg(0, std::ios::beg);
	}

	std::vector<std::byte> FileReader::read_binary() const
	{
		std::vector<std::byte> result(m_Size);
		m_Stream.read(reinterpret_cast<char*>(result.data()), m_Size);
		return result;
	}

	FileWriter::FileWriter(const std::string_view path, const bool binary)
		: m_Path(path)
		, m_Stream(m_Path, binary ? std::ios::binary : std::ios::out)
	{
		if (!m_Stream.is_open())
		{
			throw std::runtime_error(std::format("Failed to load file: {}", path));
		}
	}

	FileWriter::~FileWriter()
	{
		m_Stream.flush();
	}

	void FileWriter::write_as_string(const std::string_view data) const
	{
		m_Stream << data;
	}

	void FileWriter::write_as_binary(const void* data, const usize size) const
	{
		m_Stream.write(static_cast<const char*>(data), size);
	}

	std::string file::read_file_to_string(const std::string_view path)
	{
		return FileReader(path).read_all_to_string();
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
		FileWriter(path).write_as_string(data);
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
		return FileReader(path, true).read_binary();
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
		FileWriter(path, true).write_as_binary(data, size);
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