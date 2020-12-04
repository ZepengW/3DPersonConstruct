#include "point_data_visualizer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_down_callback(GLFWwindow* window, int nKey, int nScanCode, int nAction, int nMode);
void draw_bone(std::map<astra::JointType, astra::Vector3f> jointPos_r, astra::JointType type1, astra::JointType type2);

PointDataWindow::PointDataWindow(int width, int height)
{
	// Init GLFW
	glfwInit();
	this->width = width;
	this->height = height;
	
	// Create a GLFWwindow object that we can use for GLFW's functions
	this->window = glfwCreateWindow(width, height, "PointDataVisualizer", nullptr, nullptr);
	glfwMakeContextCurrent((GLFWwindow*)this->window);
	glfwSwapInterval(5);
	//event callback
	glfwSetMouseButtonCallback((GLFWwindow*)window, mouse_button_callback);
	glfwSetKeyCallback((GLFWwindow*)window, key_down_callback);
}

PointDataWindow::~PointDataWindow()
{
	glfwDestroyWindow((GLFWwindow*)window);
}

void PointDataWindow::display(const void* pointData, const void* colorData, int w, int h)
{
	if ((this->width != w) || (this->height != h))
		return;
	const astra::Vector3f* d = (astra::Vector3f*)pointData;
	const astra::RgbPixel* c = (astra::RgbPixel*)colorData;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_POINTS);
	int max_x, max_y, max_z;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			max_x = std::max(max_x, (int)d[i * width + j].x);
			max_y = std::max(max_y, (int)d[i * width + j].y);
			max_z = std::max(max_z, (int)d[i * width + j].z);
		}
	}
	//std::cout << "max:" << max_x << ";" << max_y << ";" << max_z << std::endl;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			glColor3f((float)c[i * width + j].r / 255, (float)c[i * width + j].g / 255, (float)c[i * width + j].b / 255);
			glVertex3f(-(float)(j - width / 2) / (width / 2), -(float)(i - height / 2) / (height / 2), d[i * width + j].z/max_z);
			glVertex3f(-(float)(j - width / 2) / (width / 2), -(float)(i - height / 2) / (height / 2), d[i * width + j].z+1 / max_z);
			glVertex3f(-(float)(j - width / 2) / (width / 2), -(float)(i - height / 2) / (height / 2), d[i * width + j].z + 2 / max_z);
			glVertex3f(-(float)(j - width / 2) / (width / 2), -(float)(i - height / 2) / (height / 2), d[i * width + j].z + 3 / max_z);
			glVertex3f(-(float)(j - width / 2) / (width / 2), -(float)(i - height / 2) / (height / 2), d[i * width + j].z + 4 / max_z);
			glVertex3f(-(float)(j - width / 2) / (width / 2), -(float)(i - height / 2) / (height / 2), d[i * width + j].z + 5 / max_z);
		}
	}
	glEnd();
	glFlush();
	glfwSwapBuffers((GLFWwindow*)this->window);
	glfwPollEvents();
}

void PointDataWindow::display_joints(const std::map<astra::JointType, astra::Vector3f> jointPositions)
{
	std::map<astra::JointType, astra::Vector3f> jointPos_r;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPointSize(20);
	
	auto iter = jointPositions.begin();
	while (iter != jointPositions.end())
	{
		float x_r = -(float)(iter->second.x - width / 2) / (width / 2);
		float y_r = -(float)(iter->second.y - height / 2) / (height / 2);
		float z_r = iter->second.z/6000;
		jointPos_r[iter->first] = astra::Vector3f(x_r,y_r,z_r);
		iter++;
	}
	

	glLineWidth(10);
	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);
	draw_bone(jointPos_r,astra::JointType::Head,astra::JointType::Neck);
	draw_bone(jointPos_r,astra::JointType::Neck,astra::JointType::ShoulderSpine);

	draw_bone(jointPos_r,astra::JointType::ShoulderSpine,astra::JointType::LeftShoulder);
	draw_bone(jointPos_r,astra::JointType::LeftShoulder,astra::JointType::LeftElbow);
	draw_bone(jointPos_r,astra::JointType::LeftElbow,astra::JointType::LeftWrist);
	draw_bone(jointPos_r,astra::JointType::LeftWrist,astra::JointType::LeftHand);

	draw_bone(jointPos_r,astra::JointType::ShoulderSpine,astra::JointType::RightShoulder);
	draw_bone(jointPos_r,astra::JointType::RightShoulder,astra::JointType::RightElbow);
	draw_bone(jointPos_r,astra::JointType::RightElbow,astra::JointType::RightWrist);
	draw_bone(jointPos_r,astra::JointType::RightWrist,astra::JointType::RightHand);

	draw_bone(jointPos_r,astra::JointType::ShoulderSpine,astra::JointType::MidSpine);
	draw_bone(jointPos_r,astra::JointType::MidSpine,astra::JointType::BaseSpine);

	draw_bone(jointPos_r,astra::JointType::BaseSpine,astra::JointType::LeftHip);
	draw_bone(jointPos_r,astra::JointType::LeftHip,astra::JointType::LeftKnee);
	draw_bone(jointPos_r,astra::JointType::LeftKnee,astra::JointType::LeftFoot);

	draw_bone(jointPos_r,astra::JointType::BaseSpine,astra::JointType::RightHip);
	draw_bone(jointPos_r,astra::JointType::RightHip,astra::JointType::RightKnee);
	draw_bone(jointPos_r,astra::JointType::RightKnee,astra::JointType::RightFoot);
	glEnd();

	glBegin(GL_POINTS);
	auto it = jointPos_r.begin();
	while (it != jointPos_r.end())
	{
		glColor3f(1.0, 0, 0);
		glVertex3f(it->second.x, it->second.y, it->second.z);
		it++;
	}
	glEnd();

	glFlush();
	glfwSwapBuffers((GLFWwindow*)this->window);
	glfwPollEvents();

}
void draw_bone(std::map<astra::JointType, astra::Vector3f> jointPos_r, astra::JointType type1, astra::JointType type2)
{
	if (0 == jointPos_r.count(type1) ||
		0 == jointPos_r.count(type2))
		return;
	glVertex3f(jointPos_r[type1].x, jointPos_r[type1].y, jointPos_r[type1].z);
	glVertex3f(jointPos_r[type2].x, jointPos_r[type2].y, jointPos_r[type2].z);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_RIGHT:
		glRotated(5, 0, 1, 0);
		break;
	default:
		return;
	}
}

void key_down_callback(GLFWwindow* window, int nKey, int nScanCode, int nAction, int nMode)
{
	if (GLFW_PRESS != nAction)
		return;
	switch (nKey)
	{
	case GLFW_KEY_LEFT:
		glRotated(10, 0, 1, 0);
		break;
	case GLFW_KEY_RIGHT:
		glRotated(-10, 0, 1, 0);
		break;
	case GLFW_KEY_UP:
		glRotated(10, 1, 0, 0);
		break;
	case GLFW_KEY_DOWN:
		glRotated(-10, 1, 0, 0);
		break;
	default:
		break;
	}
}
