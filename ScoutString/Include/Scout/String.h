/*
	For both platforms, a clean absolute path is one that fulfills the following conditions:
	- Does not contain backslashes
	- Does not contain ../ nor /.. nor ./ nor /.
	- Does not contain more than one sequential /
	- Contains only ASCII characters
	- Contains no more than one . which case it is interpreted as a folder, or as a directory if there is no .
	- Contains no identically named directories

	A clean absolute path for windows is one that fulfills the following conditions:
	- Starts with an uppercase drive letter followed by :/

	A clean absolute path for linux is one that fulfills the following conditions:
	- Starts with a leading /
*/

#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>
#include <stdexcept>
#include <algorithm>

namespace Scout
{
	inline std::int32_t StringToS32(const std::string_view str)
	{
		// Taken from: https://stackoverflow.com/questions/194465/how-to-parse-a-string-to-an-int-in-c
		char* end;
		int l;
		errno = 0;
		l = l = strtol(str.data(), &end, 10);
		if (((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) ||
			((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) ||
			(*str.data() == '\0' || *end != '\0'))
		{
			return false;
		}
		return l;
	}

	inline float StringToF32(const std::string_view str)
	{
		// Taken from: https://codereview.stackexchange.com/questions/199775/convert-string-to-double-and-check-for-errors

		char* endptr;

		const int errno_original = errno;
		errno = 0;

		float f = strtof(str.data(), &endptr);
		int errno_my_strtof = errno;
		if (errno == 0) {
			errno = errno_original;
		}

		if (str == endptr) {
			return false;
		}

		while (isspace((unsigned char)*endptr)) {
			endptr++;
		}
		if (*endptr) {
			return false;
		}

		if (errno_my_strtof == ERANGE && fabs(f) == HUGE_VALF) {
			return false;
		}

		if (errno_my_strtof == ERANGE && fabs(f) <= FLT_MIN) {
			return false;
		}

		errno = errno_original;

		return f;
	}

	inline void SanitizeAbsolutePath(std::string& path)
	{
#ifdef WIN32
		// TODO: handle non ASCII characters
		// If there are non ASCII characters, set error and return
		for (const auto& c : path)
		{
			if (c > 127)
			{
				throw std::runtime_error("Non ASCII characters detected in path. The program does not handle such cases.");
			}
		}

		// TODO: shouldn't expect user to adjust their file structure, program should be able to handle paths with multiple .'s
		// If there's more than one . set error and return
		{
			const auto nrOfDots = std::count(path.begin(), path.end(), '.');
			if (nrOfDots > 1) throw std::runtime_error("Path contains more than one dot. Please sanitize your file structure.");
		}

		// Replace all \\ with /
		std::replace(path.begin(), path.end(), '\\', '/');

		// TODO: surely you can handle this case automatically...
		// If the pattern // is detected, set error and return
		if (path.find("//") != std::string::npos) throw std::runtime_error("Cannot sanitize path automatically. Remove any /// sequences from input path.");

		// If the pattern /. or ./ is detected, set error and return
		if (path.find("./") != std::string::npos) throw std::runtime_error("Invalid input path. Remove any /. or ./ sequences from input path. Absolute paths should not have any relative path syntax in them.");
		if (path.find("/.") != std::string::npos) throw std::runtime_error("Invalid input path. Remove any /. or ./ sequences from input path. Absolute paths should not have any relative path syntax in them.");

		// If there is no drive letter, set error and return
		if (!((path[0] >= 'A' && path[0] <= 'Z') ||
			(path[0] >= 'a' && path[0] <= 'z')))
		{
			throw std::runtime_error("Absolute path does not contain a drive letter, path is ambiguous.");
		}

		// Make lowercase drive letter uppercase
		if (path[0] >= 'a' && path[0] <= 'z')
		{
			path[0] += 'a' - 'A';
		}

		// TODO: set error if there's identically named directories and return

#elif __linux__
		throw std::runtime_error("Implement this.");
#endif
	}

	inline void SanitizeDirectory(std::string& path)
	{
		throw std::runtime_error("Implement this.");
	}

	inline void SanitizeFileName(std::string& path)
	{
		throw std::runtime_error("Implement this.");
	}

	constexpr inline std::string RelativePathFromAbsolutePath(const std::string& path, const std::string_view rootDir)
	{
		const auto begin = path.find(rootDir);
		if (begin == std::string::npos) throw std::runtime_error("Variable path does not contain rootDir.");

		const auto end = path.size();
		return std::string(path.begin() + begin, path.begin() + end);
	}

	constexpr inline std::string StemFromAbsolutePath(const std::string& path)
	{
		const auto dotMatch = std::find(path.begin(), path.end(), '.');
		const auto reverseLastSlashMatch = std::find(path.rbegin(), path.rend(), '/');
		const auto lastSlashMatch = (reverseLastSlashMatch + 1).base(); // https://stackoverflow.com/questions/2037867/can-i-convert-a-reverse-iterator-to-a-forward-iterator
		if (dotMatch == path.end() || reverseLastSlashMatch == path.rend()) throw std::runtime_error("Cannot extract stem from given path.");

		const auto begin = (lastSlashMatch + 1) - path.begin(); // Compute with offsets rather than ptrs.
		const auto end = dotMatch - path.begin();
		return std::string(path.begin() + begin, path.begin() + end);
	}

	constexpr inline std::string ExtensionFromAbsolutePath(const std::string& path)
	{
		const auto dotMatch = std::find(path.begin(), path.end(), '.');
		if (dotMatch == path.end()) throw std::runtime_error("Path passed does not contains an extension.");

		const auto begin = dotMatch - path.begin(); // Compute with offsets rather than ptrs.
		const auto end = path.size();
		return std::string(path.begin() + begin, path.begin() + end);
	}
}