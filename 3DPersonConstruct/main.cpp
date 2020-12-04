#include "camera.h"
#include <key_handler.h>
#include <time.h>


int main(int argc, char** argv)
{
	astra::initialize();

	set_key_handler();

	astra::StreamSet streamSet;
	astra::StreamReader reader = streamSet.create_reader();

	SampleFrameListener listener(640, 480);

	
	reader.stream<astra::ColorStream>().start();
	/*auto depthStream = reader.stream < astra::DepthStream>();
	auto mode = depthStream.mode();
	mode.set_width(640);
	mode.set_height(480);
	mode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
	mode.set_fps(30);
	depthStream.set_mode(mode);*/
	reader.stream<astra::PointStream>().start();
	auto mode = reader.stream<astra::DepthStream>().mode();
	mode.set_width(640);
	mode.set_height(480);
	reader.stream<astra::DepthStream>().set_mode(mode);
	reader.stream<astra::DepthStream>().start();

	reader.add_listener(listener);

	do
	{
		astra_update();
	} while (shouldContinue);
	reader.remove_listener(listener);

	astra::terminate();
	return 0;
}