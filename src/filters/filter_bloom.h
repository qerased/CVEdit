#ifndef CVEDIT_FILTER_EDGEGLOW_H
#define CVEDIT_FILTER_EDGEGLOW_H
#include "filter.h"

class filter_bloom : public filter
{
public:
    const char * name () const override { return "Edgeglow"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty ()) return;

        cv::Mat srcf;
        mat.convertTo (srcf, CV_32F, 1.0 / 255.0);

        cv::Mat gray;
        cv::cvtColor (srcf, gray, cv::COLOR_BGR2GRAY);

        cv::Mat bright;
        cv::threshold (gray, bright, thresh, 1.0, cv::THRESH_TOZERO);

        cv::Mat glow;
        cv::GaussianBlur (bright, glow, cv::Size(0, 0), radius);

        cv::Mat glow_rgb;
        cv::cvtColor (glow, glow_rgb, cv::COLOR_GRAY2BGR);

        cv::Mat result;
        cv::addWeighted (srcf, 1.0, glow_rgb, coeff, 0.0, result);

        cv::min (result, 1.0, result);
        cv::max (result, 0.0, result);

        result.convertTo (mat, CV_8U, 255.0);
    }

    void set_thresh (float new_thr)
    {
        thresh = new_thr;
    }

    void set_coeff (float new_coef)
    {
        coeff = new_coef;
    }

    void set_radius (float new_rad)
    {
        radius = new_rad;
    }

private:
    float thresh = 0.7;
    float radius = 10.;
    float coeff = 0.8;
};

#endif //CVEDIT_FILTER_EDGEGLOW_H