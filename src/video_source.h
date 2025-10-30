#ifndef CVEDIT_VIDEO_SOURCE_H
#define CVEDIT_VIDEO_SOURCE_H
#include <optional>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>


class video_source
{
public:
    video_source () = default;
    ~video_source ();

    bool open_webcam (int index = 0,
                      std::optional<int> width = std::nullopt,
                      std::optional<int> height = std::nullopt);

    bool open_video (const std::string & path);

    bool is_opened () const;
    void close ();

    bool read (cv::Mat& out_img);

    double fps () const;
    cv::Size frame_size () const;
private:
    cv::VideoCapture cap_;
};


#endif //CVEDIT_VIDEO_SOURCE_H