#include "video_source.h"

video_source::~video_source ()
{
    close ();
}

bool video_source::open_webcam (int index, std::optional<int> width, std::optional<int> height)
{
    close ();

    if (!cap_.open (index, cv::CAP_ANY))
        return false;

    if (width) cap_.set (cv::CAP_PROP_FRAME_WIDTH, *width);
    if (height) cap_.set (cv::CAP_PROP_FRAME_HEIGHT, *height);
    return cap_.isOpened ();
}

bool video_source::open_video (const std::string & path)
{
    close ();
    if (!cap_.open (path, cv::CAP_ANY))
        return false;
    return cap_.isOpened ();
}

bool video_source::is_opened () const
{
    return cap_.isOpened ();
}

void video_source::close ()
{
    if (cap_.isOpened ())
        cap_.release ();
}

bool video_source::read (cv::Mat & out_img)
{
    if (!cap_.isOpened ()) return false;
    return cap_.read (out_img);
}

double video_source::fps () const
{
    if (!cap_.isOpened ()) return 0.0;
    return cap_.get (cv::CAP_PROP_FPS);
}

cv::Size video_source::frame_size () const
{
    if (!cap_.isOpened ()) return {};
    int w = static_cast<int> (cap_.get (cv::CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int> (cap_.get (cv::CAP_PROP_FRAME_HEIGHT));
    return cv::Size (w, h);
}

