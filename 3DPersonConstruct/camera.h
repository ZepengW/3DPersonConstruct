#include <astra/astra.hpp>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "point_data_visualizer.h"
#include <jsonxx/json.hpp>
#include "joint_by_openpose.h"
#include <thread>

class MultiFrameListener : public astra::FrameListener
{
public:
    /// <summary>
    /// 
    /// </summary>
    /// <param name="width"></param>
    /// <param name="length"></param>
    /// <param name="mode">
    ///     0 : collect data mode, using openpose, output: rgb_video, rgbwithJoint_video, joint_json
    ///     1 : collect data mode, using astra bone detect, show bone in 3d
    ///     2 : collect data mode, using space to begin and space to stop.
    ///     3 : collect data mode, using astra bone detect, show bone in 2d
    /// </param>
    MultiFrameListener(int width,int length, int mode);
    ~MultiFrameListener();
private:

    virtual void on_frame_ready(astra::StreamReader& reader,
        astra::Frame& frame) override;
    void process_depth(const astra::DepthFrame& depthFrame);
    void process_rgb(const astra::ColorFrame& colorFrame);
    void process_joint_astra(const astra::BodyFrame& bodyFrame, const astra::DepthFrame& deepthFrame);
    void process_rgb_joints3d_openpose(const astra::ColorFrame& colorFrame, const astra::DepthFrame& depthFrame, astra::CoordinateMapper mapper);
    void process_rgb_joints2d_astra(const astra::ColorFrame& colorFrame, const astra::BodyFrame& bodyFrame);
    int getSmoothDepth(const int16_t* depth, int idx, int width, int height);
    void write_video(cv::VideoWriter &writer,cv::Mat frame,cv::Size s, bool valid, std::string suffixLabel="");
    void write_json(jsonxx::json j);
    void save_close();
    std::vector<jsonxx::json> jointJsonVec;
    

    using duration_type = std::chrono::duration<float>;
    float frameDuration_{ 0.0 };
    using clock_type = std::chrono::system_clock;
    std::chrono::time_point<clock_type> lastTimepoint_{ clock_type::now() };

    cv::VideoWriter videoRgbOutput;
    cv::VideoWriter videoDepthOutput;
    cv::VideoWriter videoRgbJointsOutput;
    std::ofstream jointPosOutput;

    JointByOpenpose jointByOpenpose;

    bool isTrainCapture;
    bool captureValid;
    bool threadLive;

    int bodyFrameBeginIdx = 0;
    int mode = 0;

    

    //PointDataWindow pointDataWindow;
};