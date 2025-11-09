#ifndef CVEDIT_FILTER_HUE_SHIFT_H
#define CVEDIT_FILTER_HUE_SHIFT_H

#include "filter.h"

struct hue_shift_params
{
    Q_GADGET
    Q_PROPERTY (float deg_per_tick)

public:
    float deg_per_tick{1.2f};
};

class filter_hue_shift : public filter
{
public:
    const char * id () const override { return "hue_shift"; }

    void apply (cv::Mat &mat) override
    {
        frame_info fake{0};
        apply (mat, fake);
    }

    void apply (cv::Mat &mat, frame_info &fi) override
    {
        if (mat.empty ()) return;
        float deg = std::fmod (params_.deg_per_tick * static_cast<float>(fi.tick_num_), 360.f) * 0.5f;

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

    void set_speed (float speed) { params_.deg_per_tick = speed; }

    bool set_parameters (const QJsonObject &json) override
    {
        hue_shift_params tmp = params_;
        bool ok = json_to_filter (&tmp, hue_shift_params::staticMetaObject, json);
        if (ok)
            params_ = tmp;
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, hue_shift_params::staticMetaObject);
    }

private:
    hue_shift_params params_;
};

#endif //CVEDIT_FILTER_HUE_SHIFT_H