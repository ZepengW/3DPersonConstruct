#include <astra/astra.hpp>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "point_data_visualizer.h"
#include <jsonxx/json.hpp>
#include "joint_by_openpose.h"

class MultiFrameListener : public astra::FrameListener
{
public:
    MultiFrameListener(int width,int length, bool isTrainCapture);
    ~MultiFrameListener();
private:

    virtual void on_frame_ready(astra::StreamReader& reader,
        astra::Frame& frame) override;
    void process_depth(const astra::DepthFrame& depthFrame);
    void process_rgb(const astra::ColorFrame& colorFrame);
    void process_point(const astra::PointFrame& pointFrame);
    void process_point_rgb(const astra::ColorFrame& colorFrame, const astra::PointFrame& pointFrame);
    void process_depth_rgb(const astra::DepthFrame& depthFrame, const astra::ColorFrame& colorFrame);
    void process_body_3d(const astra::BodyFrame& bodyFrame, const astra::DepthFrame& deepthFrame);
    void process_rgb_body3d(const astra::ColorFrame& colorFrame, const astra::DepthFrame& depthFrame);
    void write_video(cv::VideoWriter &writer,cv::Mat frame,cv::Size s, bool valid, std::string suffixLabel="");
    void write_body(jsonxx::json j);
    void save_close();
    std::vector<jsonxx::json> jointJsonVec;
    

    using duration_type = std::chrono::duration<float>;
    float frameDuration_{ 0.0 };
    using clock_type = std::chrono::system_clock;
    std::chrono::time_point<clock_type> lastTimepoint_{ clock_type::now() };

    cv::VideoWriter videoRgbOutput;
    cv::VideoWriter videoRgbJointsOutput;
    std::ofstream jointPosOutput;

    JointByOpenpose jointByOpenpose;

    bool isTrainCapture;
    bool captureValid;

    int bodyFrameBeginIdx = 0;

    //PointDataWindow pointDataWindow;
};