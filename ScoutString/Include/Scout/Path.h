#pragma once

#include <filesystem>

namespace Scout
{
	class Path
	{
	public:
		Path(const std::string_view absolutePath, const std::string_view rootDir);

		inline std::string_view GetAbsolute() const
		{
			return std::string_view(str_);
		}

		inline std::string_view GetRelative() const
		{
			return relative_;
		}

		inline std::string_view GetStem() const
		{
			return stem_;
		}

		inline std::string_view GetExtension() const
		{
			return extension_;
		}

		inline bool GetIsDirectory() const
		{
			return isDirectory_;
		}

		inline bool Exists() const
		{
			return std::filesystem::exists(str_);
		}

	private:
		std::string str_ = "";
		std::string rootDir_ = "";
		std::string relative_ = "";
		std::string stem_ = "";
		std::string extension_ = "";
		bool isDirectory_ = false;
	};
}