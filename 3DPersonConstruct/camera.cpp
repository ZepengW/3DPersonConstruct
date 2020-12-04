// This file is part of the Orbbec Astra SDK [https://orbbec3d.com]
// Copyright (c) 2015-2017 Orbbec 3D
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Be excellent to each other.
#include <cstdio>
#include <iostream>
#include <iomanip>
#include "camera.h"
#include <time.h>
#include <LitDepthVisualizer.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#ifdef WIN
#include <io.h>
#include <direct.h>
#endif // WIN
#ifdef LINUX
#include <unistd.h>
#include <sys/types.h>
#endif // LINUX



using namespace cv;

SampleFrameListener::SampleFrameListener(int width, int height):pointDataWindow(width,height)
{

}

void SampleFrameListener::on_frame_ready(astra::StreamReader& reader,
	astra::Frame& frame)
{
	//const astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();
	const astra::ColorFrame colorFrame = frame.get<astra::ColorFrame>();
	const astra::PointFrame pointFrame = frame.get<astra::PointFrame>();

	//this->process_depth(depthFrame);
	this->process_rgb(colorFrame);
	this->process_point(pointFrame);
	this->process_point_rgb(colorFrame, pointFrame);
	//this->process_depth_rgb(depthFrame, colorFrame);
	
}

void SampleFrameListener::process_depth(const astra::DepthFrame& depthFrame)
{
	if (!depthFrame.is_valid())
		return;
	int width = depthFrame.width();
	int height = depthFrame.height();

	const int16_t* depthData = depthFrame.data();
	std::cout << width * height << ";" << depthFrame.length() << std::endl;
	int16_t* data_test=(int16_t*)malloc(depthFrame.length() * sizeof(int16_t));
	depthFrame.copy_to(data_test);
	Mat frame = Mat(height, width, CV_16UC1, data_test);
	cv::imshow("test3", frame);
	waitKey(10);

}

void SampleFrameListener::process_rgb(const astra::ColorFrame& colorFrame)
{
	if (!colorFrame.is_valid())
		return;
	int width = colorFrame.width();
	int height = colorFrame.height();
	int frameIndex = colorFrame.frame_index();

	astra::RgbPixel* buffer_color = (astra::RgbPixel*)malloc(colorFrame.length() * sizeof(astra::RgbPixel));
	colorFrame.copy_to(buffer_color);
	uint8_t* data = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));
	for (size_t i = 0; i < width * height; i++)
	{
		if (i == ((width * (height / 2.0f)) + (width / 2.0f)))
		{
			//std::cout << i << std::endl;
		}
		data[3 * i] = buffer_color[i].b;
		data[3 * i + 1] = buffer_color[i].g;
		data[3 * i + 2] = buffer_color[i].r;
	}
	Mat frame = Mat(height, width, CV_8UC3, (uint8_t *)data);
	//write_video(this->videoRgbOutput,frame, Size(width, height),"RGB");
	cv::imshow("rgb", frame);
	waitKey(10);
	free(buffer_color);
	free(data);
}

void SampleFrameListener::process_point(const astra::PointFrame& pointFrame)
{
	if (!pointFrame.is_valid())
		return;
	int width = pointFrame.width();
	int height = pointFrame.height();

	samples_astra::common::LitDepthVisualizer visualizer_;
	visualizer_.update(pointFrame);
	const astra::RgbPixel* vizBuffer = visualizer_.get_output();
	uint8_t* data = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));
	for (int i = 0; i < width * height; i++)
	{
		data[3 * i] = vizBuffer[i].b;
		data[3 * i + 1] = vizBuffer[i].g;
		data[3 * i + 2] = vizBuffer[i].r;
	}
	Mat frame = Mat(height, width, CV_8UC3, (uint8_t *)data);
	cv::imshow("depth", frame);
	waitKey(10);
	free(data);
}

void SampleFrameListener::process_point_rgb(const astra::ColorFrame& colorFrame, const astra::PointFrame& pointFrame)
{
	if((!colorFrame.is_valid())||
		(!pointFrame.is_valid()))
		return;
	int width = pointFrame.width();
	int height = pointFrame.height();
	
	const astra::Vector3f* pointData = pointFrame.data();
	const astra::RgbPixel* colorData = colorFrame.data();
	
	this->pointDataWindow.display(pointData,colorData,width,height);
}

void SampleFrameListener::process_depth_rgb(const astra::DepthFrame& depthFrame, const astra::ColorFrame& colorFrame)
{
	if ((!depthFrame.is_valid())||
		(!colorFrame.is_valid()))
		return;
	int width = depthFrame.width();
	int height = depthFrame.height();

	const int16_t* depthData = depthFrame.data();
	const astra::RgbPixel* colorData = colorFrame.data();
	this->pointDataWindow.display(depthData, colorData, width, height);
}


void SampleFrameListener::write_video(VideoWriter &writer,cv::Mat frame, cv::Size s,std::string mode)
{
	if (writer.isOpened())
		writer.write(frame);
	else
	{
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		char filename[20];
		snprintf(filename, 15, "%04d%02d%02d%02d%02d%02d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		std::cout << "Create video file. name:" << filename;

		if (-1 == access("./output", 0))
		{
#ifdef WIN
			mkdir("./output");
#endif // WIN
#ifdef LINUX
			mkdir("./output", 0777);
#endif // LINUX
		}
		writer.open("./output/" + std::string(filename) + mode+".avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, s, true);
	}
}




