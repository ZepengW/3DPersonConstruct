#include <astra/astra.hpp>
#include <chrono>

class SampleFrameListener : public astra::FrameListener
{
public:
    virtual void on_frame_ready(astra::StreamReader& reader,
        astra::Frame& frame) override;
    void print_depth(const astra::DepthFrame& depthFrame,
        const astra::CoordinateMapper& mapper);
    void print_color(const astra::ColorFrame& colorFrame);
    void check_fps();
private:
    using buffer_dep_ptr = std::unique_ptr<int16_t[]>;
    buffer_dep_ptr buffer_dep;
    using buffer_col_ptr = std::unique_ptr<astra::RgbPixel[]>;
    buffer_col_ptr buffer_col;
    unsigned int lastWidth_;
    unsigned int lastHeight_;


    using duration_type = std::chrono::duration<float>;
    float frameDuration_{ 0.0 };
    using clock_type = std::chrono::system_clock;
    std::chrono::time_point<clock_type> lastTimepoint_{ clock_type::now() };

};