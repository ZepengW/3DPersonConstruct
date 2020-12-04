#include "camera.h"
#include <key_handler.h>
#include <time.h>
#define MAX_TIME_FOR_NO_LICENSE 18000	//30min
std::string resInfo[3] =
{
	"SUCCESS",
	"Initialize failed",
	"NO LICENSE: So Reboot every 30 mins",
};

int process(const char* licenseString)
{
	if (ASTRA_STATUS_SUCCESS != astra::initialize())
		return 1;
	// license for skeleton
	bool isActivated = orbbec_body_tracking_set_license(licenseString) == ASTRA_STATUS_SUCCESS ? true : false;
	int res = 2;
	bool isLicenseTimeout = false;
	//30min if no license
	clock_t start = isActivated ? LONG_MAX : clock();

	astra::StreamSet streamSet;
	astra::StreamReader reader = streamSet.create_reader();

	MultiFrameListener listener(640, 480);


	reader.stream<astra::ColorStream>().start();
	reader.stream<astra::PointStream>().start();
	auto mode = reader.stream<astra::DepthStream>().mode();
	mode.set_width(640);
	mode.set_height(480);
	reader.stream<astra::DepthStream>().set_mode(mode);
	reader.stream<astra::DepthStream>().start();
	reader.stream<astra::BodyStream>().set_skeleton_profile(astra::SkeletonProfile::Full);
	reader.stream<astra::BodyStream>().start();

	reader.add_listener(listener);

	while (!isLicenseTimeout)
	{
		isLicenseTimeout = (clock() - start >= MAX_TIME_FOR_NO_LICENSE) ? true : false;
		astra_update();
	}
	reader.remove_listener(listener);

	astra::terminate();
	return res;
}

int main(int argc, char** argv)
{
	bool isContinue=true;
	while (isContinue)
	{
		std::cout << "capture begin"<<std::endl;
		int res = process("<INPUT LICENSE>");
		std::cout << "capture finish because of [" << resInfo[res] << "]"<<std::endl;
		isContinue = (res == 100) ? false : true;
	}
	return 0;
}