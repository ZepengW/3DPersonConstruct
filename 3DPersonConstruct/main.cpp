#include "camera.h"
#include <key_handler.h>
#include <time.h>


int main(int argc, char** argv)
{
	astra::initialize();

	set_key_handler();

	astra::StreamSet streamSet;
	astra::StreamReader reader = streamSet.create_reader();

	SampleFrameListener listener;

	auto depthStream = reader.stream<astra::DepthStream>();
	depthStream.start();
	reader.stream<astra::ColorStream>().start();
	reader.stream<astra::PointStream>().start();

	char serialnumber[256];
	depthStream.serial_number(serialnumber, 256);

	std::cout << "depthStream -- hFov: "
		<< reader.stream<astra::DepthStream>().hFov()
		<< " vFov: "
		<< reader.stream<astra::DepthStream>().vFov()
		<< " serial number: "
		<< serialnumber
		<< std::endl;

	const uint32_t chipId = depthStream.chip_id();

	switch (chipId)
	{
	case ASTRA_CHIP_ID_MX400:
		std::cout << "Chip ID: MX400" << std::endl;
		break;
	case ASTRA_CHIP_ID_MX6000:
		std::cout << "Chip ID: MX6000" << std::endl;
		break;
	case ASTRA_CHIP_ID_UNKNOWN:
	default:
		std::cout << "Chip ID: Unknown" << std::endl;
		break;
	}

	const astra_usb_info_t usbinfo = depthStream.usb_info();

	std::cout << "usbInfo ---pid:" << usbinfo.pid << " vid: " << usbinfo.vid << std::endl;


	reader.add_listener(listener);

	do
	{
		astra_update();
	} while (shouldContinue);
	reader.remove_listener(listener);

	astra::terminate();
	return 0;
}