#pragma once

#include "aw/core/async/thread_pool.h"
#include "aw/core/primitive/numbers.h"

#include <fstream>
#include <future>
#include <string>
#include <string_view>
#include <vector>

namespace aw::core
{
	class IFileStream
	{
	public:
		virtual ~IFileStream();

		virtual std::string_view get_path() const = 0;

	protected:
		IFileStream() = default;
	};

	class FileReader final : public IFileStream
	{
	public:
		FileReader(std::string_view path, bool binary = false);
		~FileReader() override;

		usize get_size() const { return m_Size; }
		std::string_view get_path() const override { return m_Path; }

		std::string read_all_to_string() const;
		std::string read_next_line() const;
		void reset_pointer() const;

		std::vector<std::byte> read_binary() const;

	private:
		std::string m_Path;
		mutable std::ifstream m_Stream;

		usize m_Size = 0;
	};

	class FileWriter final : public IFileStream
	{
	public:
		FileWriter(std::string_view path, bool binary = false);
		~FileWriter() override;

		std::string_view get_path() const override { return m_Path; }

		void write_as_string(std::string_view data) const;
		void write_as_binary(const void* data, usize size) const;

	private:
		std::string m_Path;
		mutable std::ofstream m_Stream;
	};

	namespace file
	{
		std::string read_file_to_string(std::string_view path);
		std::future<std::string> read_file_to_string_async(std::string_view path, ThreadPool* thread_pool = nullptr);

		void write_file_from_string(std::string_view path, std::string_view data);
		std::future<void> write_file_from_string_async(std::string_view path, std::string_view data, ThreadPool* thread_pool = nullptr);

		std::vector<std::byte> read_file_to_binary(std::string_view path);
		std::future<std::vector<std::byte>> read_file_to_binary_async(std::string_view path, ThreadPool* thread_pool = nullptr);

		void write_file_from_binary(std::string_view path, const void* data, usize size);
		std::future<void> write_file_from_binary_async(std::string_view path, const void* data, usize size, ThreadPool* thread_pool = nullptr);
	}
}