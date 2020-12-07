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
#include <windows.data.json.h>
#endif // WIN
#ifdef LINUX
#include <unistd.h>
#include <sys/types.h>
#endif // LINUX



using namespace cv;

MultiFrameListener::MultiFrameListener(int width, int height, bool isTrainCapture):pointDataWindow(width,height)
{
	this->isTrainCapture = isTrainCapture;
	this->captureValid = !isTrainCapture;
}

MultiFrameListener::~MultiFrameListener()
{
	this->save_close_body_json();
}

void MultiFrameListener::on_frame_ready(astra::StreamReader& reader,
	astra::Frame& frame)
{
	const astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();
	const astra::ColorFrame colorFrame = frame.get<astra::ColorFrame>();
	const astra::BodyFrame bodyFrame = frame.get<astra::BodyFrame>();

	this->process_body_3d(bodyFrame, depthFrame);
	this->process_rgb(colorFrame);
	
	
}

void MultiFrameListener::process_depth(const astra::DepthFrame& depthFrame)
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

void MultiFrameListener::process_rgb(const astra::ColorFrame& colorFrame)
{
	if (!colorFrame.is_valid())
		return;
	int width = colorFrame.width();
	int height = colorFrame.height();

	astra::RgbPixel* buffer_color = (astra::RgbPixel*)malloc(colorFrame.length() * sizeof(astra::RgbPixel));
	colorFrame.copy_to(buffer_color);
	uint8_t* data = (uint8_t*)malloc(width * height * 3 * sizeof(uint8_t));
	for (size_t j = 0; j < height; j++)
	{
		for (size_t i = 0; i < width; i++)
		{
			data[3 * (i+width*j)] = buffer_color[(width-i + width * j)].b;
			data[3 * (i + width * j) + 1] = buffer_color[(width - i + width * j)].g;
			data[3 * (i + width * j) + 2] = buffer_color[(width - i + width * j)].r;
		}
	}

	Mat frame = Mat(height, width, CV_8UC3, (uint8_t *)data);
	write_video(this->videoRgbOutput,frame, Size(width, height), this->captureValid);
	cv::imshow("rgb", frame);
	waitKey(10);
	free(buffer_color);
	free(data);
}

void MultiFrameListener::process_point(const astra::PointFrame& pointFrame)
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

void MultiFrameListener::process_point_rgb(const astra::ColorFrame& colorFrame, const astra::PointFrame& pointFrame)
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

void MultiFrameListener::process_depth_rgb(const astra::DepthFrame& depthFrame, const astra::ColorFrame& colorFrame)
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

void MultiFrameListener::process_body_3d(const astra::BodyFrame& bodyFrame,const astra::DepthFrame& depthFrame)
{
	if (!bodyFrame.is_valid()||!depthFrame.is_valid() || bodyFrame.info().width() == 0 || bodyFrame.info().height() == 0)
		return;
	const auto& bodies = bodyFrame.bodies();
	if (bodies.empty())
	{
		this->captureValid = !(this->isTrainCapture);
		this->save_close_body_json();
		return;
	}
	else if(!this->captureValid)
	{
		this->captureValid = true;
		this->bodyFrameBeginIdx = bodyFrame.frame_index();
	}
	int width = depthFrame.width();
	int height = depthFrame.height();
	jsonxx::json j;
	j["FrameId"] = bodyFrame.frame_index() - this->bodyFrameBeginIdx;
	for (auto& body : bodies)
	{
		std::map<astra::JointType, astra::Vector3f> jointPositions;
		std::map<astra::JointType, astra::JointStatus> jointStatus;

		for (auto& joint : body.joints())
		{
			astra::JointType type = joint.type();
			astra::JointStatus status = joint.status();
			const auto& pose = joint.depth_position();
			if (pose.x > width || pose.y > height)
				continue;
			int16_t z = depthFrame.data()[(int)pose.y * width + (int)pose.x];
			jointPositions[type] = astra::Vector3f(pose.x, pose.y, z);
			jointStatus[type] = status;
			j["body-"+std::to_string(body.id())][std::to_string((int)type)] = { int(pose.x),int(pose.y),z,(int)status};
		}
		this->pointDataWindow.display_joints(jointPositions,jointStatus);
		this->write_body(j);
	}
}


void MultiFrameListener::write_video(VideoWriter &writer,cv::Mat frame, cv::Size s, bool valid)
{
	if (writer.isOpened())
	{
		if (!valid)
		{
			writer.release();
		}
		writer.write(frame);
	}
	else if(valid)
	{
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		char filename[20];
		snprintf(filename, 15, "%04d%02d%02d%02d%02d%02d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		std::cout << "Create video file. name:" << filename<<".avi" << std::endl;

		if (-1 == access("./output", 0))
		{
#ifdef WIN
			mkdir("./output");
#endif // WIN
#ifdef LINUX
			mkdir("./output", 0777);
#endif // LINUX
		}
		writer.open("./output/" + std::string(filename)+".avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, s, true);
	}
}

void MultiFrameListener::write_body(jsonxx::json j)
{
	if (this->jointPosOutput.is_open())
	{
		this->jointJsonVec.push_back(j);
	}
	else
	{
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		char filename[20];
		snprintf(filename, 15, "%04d%02d%02d%02d%02d%02d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		std::cout << "Create joint file. name:" << filename<<".json" << std::endl;

		if (-1 == access("./output", 0))
		{
#ifdef WIN
			mkdir("./output");
#endif // WIN
#ifdef LINUX
			mkdir("./output", 0777);
#endif // LINUX
		}
		this->jointPosOutput.open("./output/" + std::string(filename)+".json",std::ios::out|std::ios::trunc);
		this->jointJsonVec.clear();
		std::cout << "1";
	}
}

void MultiFrameListener::save_close_body_json()
{
	if (this->jointPosOutput.is_open())
	{
		this->jointPosOutput <<std::setw(4)<< this->jointJsonVec << std::endl;
		this->jointPosOutput.close();
		this->jointJsonVec.clear();
	}
}




