#include <Scout/Path.h>

#include <Scout/String.h>

namespace Scout
{
	Path::Path(const std::string_view absolutePath, const std::string_view rootDir)
	{
		str_ = absolutePath;
		SanitizeAbsolutePath(str_);

		isDirectory_ = absolutePath.find('.') == std::string::npos;

		relative_ = RelativePathFromAbsolutePath(str_, rootDir);
		stem_ = StemFromAbsolutePath(str_);
		extension_ = ExtensionFromAbsolutePath(str_);
		rootDir_ = rootDir;
	}
}