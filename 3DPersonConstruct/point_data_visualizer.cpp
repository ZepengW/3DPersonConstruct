#include "point_data_visualizer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <astra/astra.hpp>


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_down_callback(GLFWwindow* window, int nKey, int nScanCode, int nAction, int nMode);

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

//void PointDataWindow::display(const void* depthData, const void* colorData, int w, int h)
//{
//	if ((this->width != w) || (this->height != h))
//		return;
//	const int16_t* d = (astra::int16_t*)depthData;
//	const astra::RgbPixel* c = (astra::RgbPixel*)colorData;
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glBegin(GL_POINTS);
//	for (int i = 0; i < height; i++)
//	{
//		for (int j = 0; j < width; j++)
//		{
//			glColor3f((float)c[i * width + j].r/255, (float)c[i * width + j].g / 255, (float)c[i * width + j].b / 255);
//			glVertex3f((float)(j - width / 2) / (width / 2), -(float)(i - height / 2) / (height / 2), (float)d[(i * width + j)]/8000);
//			std::cout << d[(i * width + j)] << ";" << (float)d[(i * width + j)] / 8000
//				<< ";" << (float)d[(i * width + j)] / 8000.0 << std::endl;
//		}
//	}
//	glEnd();
//	glFlush();
//	glfwSwapBuffers((GLFWwindow*)this->window);
//	glfwPollEvents();
//}

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
			/*std::cout << d[i * width + j].x << ";" << d[i * width + j].y
				<< ";" << d[i * width + j].z << std::endl;*/
		}
	}
	glEnd();
	glFlush();
	glfwSwapBuffers((GLFWwindow*)this->window);
	glfwPollEvents();
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
