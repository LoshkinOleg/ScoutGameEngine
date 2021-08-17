#include <gli/gli.hpp>

int main(int argv, char** args)
{
	auto texture = gli::load("../data/ktx/complexScene/Cube_albedoMap.ktx");
	gli::gl translator(gli::gl::PROFILE_KTX);

	int i = 5;

	return 0;
}