#ifndef CVEDIT_FILTER_HUE_SHIFT_H
#define CVEDIT_FILTER_HUE_SHIFT_H

#include "filter.h"

class filter_hue_shift : public filter
{
public:
    const char * name () const { return "hue shift"; }

    void apply (cv::Mat &mat) override
    {
        frame_info fake{0};
        apply (mat, fake);
    }

    void apply (cv::Mat &mat, frame_info &fi) override
    {
        if (mat.empty ()) return;
        float deg = std::fmod (deg_per_tick_ * static_cast<float>(fi.tick_num_), 360.f) * 0.5f;

        cv::Mat hsv;
        cv::cvtColor (mat, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> channels;
        cv::split (hsv, channels);

        channels[0].forEach<uchar> ([deg](uchar & h, const int *)
        {
           int v = static_cast<int> (h) + static_cast<int> (deg);
           if (v < 0)
               v += 180;
           else if (v >= 180)
               v -= 180;
           h = static_cast<uchar> (v);
        });

        cv::merge (channels, hsv);
        cv::cvtColor (hsv, mat, cv::COLOR_BGR2HSV);
    }

    void set_speed (float speed) { deg_per_tick_ = speed; }
private:
    float deg_per_tick_{1.2f};

};

#endif //CVEDIT_FILTER_HUE_SHIFT_H