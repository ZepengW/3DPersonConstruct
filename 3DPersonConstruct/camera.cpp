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


void SampleFrameListener::on_frame_ready(astra::StreamReader& reader,
	astra::Frame& frame)
{
	const astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();
	const astra::ColorFrame colorFrame = frame.get<astra::ColorFrame>();

	if (depthFrame.is_valid())
	{
		auto depthStream = reader.stream<astra::DepthStream>();
		print_depth(depthFrame, depthStream.coordinateMapper());
		//check_fps();
	}
	if (colorFrame.is_valid())
	{
		auto depthStream = reader.stream<astra::DepthStream>();
		print_color(colorFrame);
	}
}

void SampleFrameListener::print_depth(const astra::DepthFrame& depthFrame,
	const astra::CoordinateMapper& mapper)
{
	if (depthFrame.is_valid())
	{
		int width = depthFrame.width();
		int height = depthFrame.height();
		int frameIndex = depthFrame.frame_index();

		//determine if buffer needs to be reallocated
		if (width != lastWidth_ || height != lastHeight_)
		{
			buffer_dep = buffer_dep_ptr(new int16_t[depthFrame.length()]);
			lastWidth_ = width;
			lastHeight_ = height;
		}
		depthFrame.copy_to(buffer_dep.get());

		size_t index = ((width * (height / 2.0f)) + (width / 2.0f));
		short middle = buffer_dep[index];

		float worldX, worldY, worldZ;
		float depthX, depthY, depthZ;
		mapper.convert_depth_to_world(width / 2.0f, height / 2.0f, middle, worldX, worldY, worldZ);
		mapper.convert_world_to_depth(worldX, worldY, worldZ, depthX, depthY, depthZ);

		std::cout << "depth frameIndex: " << frameIndex
			<< " value: " << middle
			<< "depth w:" << width
			<< "depth h:" << height
			//<< " wX: " << worldX
			//<< " wY: " << worldY
			//<< " wZ: " << worldZ
			//<< " dX: " << depthX
			//<< " dY: " << depthY
			//<< " dZ: " << depthZ
			<< std::endl;
	}

}

void SampleFrameListener::print_color(const astra::ColorFrame& colorFrame)
{
	if (colorFrame.is_valid())
	{
		int width = colorFrame.width();
		int height = colorFrame.height();
		int frameIndex = colorFrame.frame_index();

		if (width != lastWidth_ || height != lastHeight_) {
			buffer_col = buffer_col_ptr(new astra::RgbPixel[colorFrame.length()]);
			lastWidth_ = width;
			lastHeight_ = height;
		}
		colorFrame.copy_to(buffer_col.get());

		size_t index = ((width * (height / 2.0f)) + (width / 2.0f));
		astra::RgbPixel middle = buffer_col[index];

		std::cout << "color frameIndex: " << frameIndex
			<< " r: " << static_cast<int>(middle.r)
			<< " g: " << static_cast<int>(middle.g)
			<< " b: " << static_cast<int>(middle.b)
			<< std::endl;
	}
}

void SampleFrameListener::check_fps()
{
	const float frameWeight = 0.5;

	auto newTimepoint = clock_type::now();
	float frameDuration = std::chrono::duration_cast<duration_type>(newTimepoint - lastTimepoint_).count();

	frameDuration_ = frameDuration * frameWeight + frameDuration_ * (1 - frameWeight);
	lastTimepoint_ = newTimepoint;

	double fps = 1.0 / frameDuration_;

	auto precision = std::cout.precision();
	std::cout << std::fixed
		<< std::setprecision(1)
		<< fps << " fps ("
		<< std::setprecision(2)
		<< frameDuration_ * 1000.0 << " ms)"
		<< std::setprecision(precision)
		<< std::endl;
}


