#include <astra/astra.hpp>
#include <map>

class PointDataWindow
{
public:
	PointDataWindow(int width, int height );
	~PointDataWindow();
	//input type: astra::Vector3f* and astra::RgbPixel*
	void display(const void* pointData, const void* colorData, int w, int l);
	void display_joints(const std::map<astra::JointType, astra::Vector3f> jointPositions);
private:
	void* window;
	int width;
	int height;
	
};