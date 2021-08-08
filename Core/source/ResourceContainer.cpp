#include "ResourceContainer.h"

namespace sge
{
	bool A_Handle::operator==(const A_Handle& other) const
	{
		return hash == other.hash;
	}
	bool A_Handle::operator==(const A_Resource& resource) const
	{
		return hash == resource.hash;
	}
	bool A_Resource::operator==(const A_Resource& other) const
	{
		hash == other.hash;
	}
	bool A_Resource::operator==(const A_Handle& handle) const
	{
		return hash == handle.hash;
	}
}//!sge