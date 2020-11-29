#include <astra/astra.hpp>
#include <chrono>
#include <opencv2/opencv.hpp>

class SampleFrameListener : public astra::FrameListener
{
public:
    SampleFrameListener();
private:
    
    virtual void on_frame_ready(astra::StreamReader& reader,
        astra::Frame& frame) override;
    void print_depth(const astra::DepthFrame& depthFrame,
        const astra::CoordinateMapper& mapper);
    void print_color(const astra::ColorFrame& colorFrame);
    void write_video(cv::VideoWriter &writer,cv::Mat frame,cv::Size s,std::string mode);


    using duration_type = std::chrono::duration<float>;
    float frameDuration_{ 0.0 };
    using clock_type = std::chrono::system_clock;
    std::chrono::time_point<clock_type> lastTimepoint_{ clock_type::now() };

    cv::VideoWriter videoRgbOutput;
    cv::VideoWriter vidroDepthOutput;

};