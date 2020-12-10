#include "camera.h"
#include <time.h>
#define MAX_TIME_FOR_NO_LICENSE 1200	//20min

#define WIDTH 640
#define HEIGHT 480
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
	orbbec_body_tracking_set_license(licenseString);
	bool isActivated =  false;
	int res = 2;
	bool isLicenseTimeout = false;
	//30min if no license
	clock_t start = isActivated ? LONG_MAX : clock();

	astra::StreamSet streamSet;
	astra::StreamReader reader = streamSet.create_reader();
	MultiFrameListener listener(WIDTH, HEIGHT, true);

	if (reader.stream<astra::DepthStream>().is_available() == false)
		return -1;

	auto mode = reader.stream<astra::DepthStream>().mode();
	mode.set_width(WIDTH);
	mode.set_height(HEIGHT);
	reader.stream<astra::DepthStream>().set_mode(mode);
	reader.stream<astra::DepthStream>().start();
	//reader.stream<astra::BodyStream>().set_skeleton_profile(astra::SkeletonProfile::Full);
	//reader.stream<astra::BodyStream>().start();
	auto mode_rgb = reader.stream<astra::ColorStream>().mode();
	mode_rgb.set_width(WIDTH);
	mode_rgb.set_height(HEIGHT);
	reader.stream<astra::ColorStream>().set_mode(mode_rgb);
	reader.stream<astra::ColorStream>().start();

	reader.add_listener(listener);

	while (!isLicenseTimeout)
	{
		int t = (clock() - start)/CLOCKS_PER_SEC;
		isLicenseTimeout = (t >= MAX_TIME_FOR_NO_LICENSE) ? true : false;
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
		if (-1 == res)
		{
			std::cout << "can't get stream. maybe the device is disconnected"<<std::endl;
			return 0;
		}
		std::cout << "capture finish because of [" << resInfo[res] << "]"<<std::endl;
		isContinue = (res == 100) ? false : true;
	}
	return 0;
}