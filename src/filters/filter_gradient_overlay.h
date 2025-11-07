#ifndef CVEDIT_FILTER_GRADIENT_OVERLAY_H
#define CVEDIT_FILTER_GRADIENT_OVERLAY_H

#include "filter.h"

class filter_gradient_overlay : public filter
{
public:
    const char * name () const override { return "gradient_overlay"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty () || k_ <= 0.f) return;

        if (need_rebuild_ || gradient_.size() != mat.size())
        {
            build_gradient (mat.size ());
            need_rebuild_ = false;
        }

        cv::addWeighted (mat, 1. - k_, gradient_, k_, 0., mat);
    }


    void set_strength (float str) { k_ = std::clamp (str, 0.f, 1.f); }
    void set_axis (float axis) { axis_ = axis ? 1 : 0; need_rebuild_ = true; }

    void set_first_color (const cv::Scalar & new_col) { c0_ = new_col; need_rebuild_ = true; }
    void set_second_color (const cv::Scalar & new_col) { c1_ = new_col; need_rebuild_ = true; }

private:
    cv::Scalar c0_{255, 128, 128};
    cv::Scalar c1_{128, 128, 255};
    int axis_{1};
    float k_{0.5f};

    bool need_rebuild_ {false};
    cv::Mat gradient_;

    void build_gradient (const cv::Size & sz)
    {
        gradient_.create (sz, CV_8UC3);

        if (axis_ == 1) // vertical
        {
            for (int y = 0; y < sz.height; y++)
            {
                float t = (sz.height > 1) ? static_cast<float> (y) / (sz.height - 1) : 0.f;

                cv::Vec3b c
                (
                    static_cast<uint8_t> ((1 - t) * c0_[0] + t * c1_[0]),
                    static_cast<uint8_t> ((1 - t) * c0_[1] + t * c1_[1]),
                    static_cast<uint8_t> ((1 - t) * c0_[2] + t * c1_[2])
                );
                cv::line (gradient_, {0, y}, {sz.width - 1, y}, cv::Scalar (c[0], c[1], c[2]), 1);
            }
        }
        else
        {
            for (int x = 0; x < sz.width; x++)
            {
                float t = sz.width > 1 ? static_cast<float> (x) / (sz.width - 1) : 0.f;
                cv::Vec3b c
                (
                    static_cast<uint8_t> ((1 - t) * c0_[0] + t * c1_[0]),
                    static_cast<uint8_t> ((1 - t) * c0_[1] + t * c1_[1]),
                    static_cast<uint8_t> ((1 - t) * c0_[2] + t * c1_[2])
                );
                cv::line (gradient_, {x,0}, {x,sz.height - 1}, cv::Scalar(c[0], c[1], c[2]), 1);
            }
        }

        // add some dithering for better quality
        cv::Mat n (sz, CV_8UC3);
        cv::randn (n, 0, 2);
        cv::add (gradient_, n, gradient_, cv::noArray (), CV_8UC3);
    }
};

#endif //CVEDIT_FILTER_GRADIENT_OVERLAY_H