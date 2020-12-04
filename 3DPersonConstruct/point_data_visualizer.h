

class PointDataWindow
{
public:
	PointDataWindow(int width, int height);
	//input type: astra::Vector3f* and astra::RgbPixel*
	void display(const void* pointData, const void* colorData, int w, int l);

private:
	void* window;
	int width;
	int height;
	
};