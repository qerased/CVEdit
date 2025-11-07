#ifndef CVEDIT_FILTER_COLORIZE_H
#define CVEDIT_FILTER_COLORIZE_H

#include "filter.h"

class filter_colorize : public filter
{
public:
    const char * name () const override { return "colorize"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty() || k_ <= 0) return;
        cv::Mat tint (mat.size (), CV_8UC3, tint_);
        cv::addWeighted (mat, 1. - k_, tint, k_, 0., mat);
    }

    void set_tint_color (const cv::Scalar & new_col) { tint_ = new_col; }
    void set_strength (const double newk) { k_ = newk; }

private:
    cv::Scalar tint_{180, 180, 255};
    float k_ {0};
};

#endif //CVEDIT_FILTER_COLORIZE_H