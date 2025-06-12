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
		virtual ~IFileStream() = default;

		virtual std::string_view get_path() const = 0;

	protected:
		IFileStream() = default;
	};

	class IFileReader : public IFileStream
	{
	public:
		virtual usize get_size() const = 0;
		virtual std::string read_next_line() const = 0;
		virtual std::string read_all_to_string() const = 0;
		virtual void reset_pointer() const = 0;
		virtual std::vector<std::byte> read_binary() const = 0;
	};

	class DefaultFileReader final : public IFileReader
	{
	public:
		DefaultFileReader(std::string_view path, bool binary = false);
		~DefaultFileReader() override;

		usize get_size() const override { return m_Size; }
		std::string_view get_path() const override { return m_Path; }

		std::string read_all_to_string() const override;
		std::string read_next_line() const override;
		void reset_pointer() const override;

		std::vector<std::byte> read_binary() const override;

	private:
		std::string m_Path;
		mutable std::ifstream m_Stream;

		usize m_Size = 0;
	};

	class AwpkArchive;
	class AwpkFileReader final : public IFileReader
	{
	public:
		AwpkFileReader(AwpkArchive* archive, std::string_view path);
		std::string_view get_path() const override { return m_Path; }
		usize get_size() const override { return m_Bytes.size(); }

		std::string read_next_line() const override;
		std::string read_all_to_string() const override;
		void reset_pointer() const override;

		std::vector<std::byte> read_binary() const override;

	private:
		AwpkArchive* m_Archive;
		std::string_view m_Path;
		std::vector<std::byte> m_Bytes;
		mutable usize m_Offset = 0;
	};

	class IFileWriter : public IFileStream
	{
	public:
		virtual void write_as_string(std::string_view data) const = 0;
		virtual void write_as_binary(const void* data, usize size) const = 0;
	};

	class DefaultFileWriter final : public IFileWriter
	{
	public:
		DefaultFileWriter(std::string_view path, bool binary = false);
		~DefaultFileWriter() override;

		std::string_view get_path() const override { return m_Path; }

		void write_as_string(std::string_view data) const override;
		void write_as_binary(const void* data, usize size) const override;

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
	} // namespace file
} // namespace aw::core