#ifndef CVEDIT_FILTER_GRADIENT_OVERLAY_H
#define CVEDIT_FILTER_GRADIENT_OVERLAY_H

#include "filter.h"

struct gradient_overlay_parameters
{
    Q_GADGET
    Q_PROPERTY (float c0_b)
    Q_PROPERTY (float c0_g)
    Q_PROPERTY (float c0_r)

    Q_PROPERTY (float c1_b)
    Q_PROPERTY (float c1_g)
    Q_PROPERTY (float c1_r)

    Q_PROPERTY (int axis)
    Q_PROPERTY (float k)

public:
    float c0_b = 255.f;
    float c0_g = 128.f;
    float c0_r = 128.f;

    float c1_b = 128.f;
    float c1_g = 128.f;
    float c1_r = 255.f;

    int axis{1};
    float k{0.5f};
};

class filter_gradient_overlay : public filter
{
public:
    const char * id () const override { return "gradient_overlay"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty () || params_.k <= 0.f) return;

        if (need_rebuild_ || gradient_.size() != mat.size())
        {
            build_gradient (mat.size ());
            need_rebuild_ = false;
        }

        cv::addWeighted (mat, 1. - params_.k, gradient_, params_.k, 0., mat);
    }


    void set_strength (float str) { params_.k = std::clamp (str, 0.f, 1.f); }
    void set_axis (float axis) { params_.axis = axis ? 1 : 0; need_rebuild_ = true; }

    void set_first_color (const cv::Scalar & new_col)
    {
        params_.c0_b = new_col[0];
        params_.c0_g = new_col[1];
        params_.c0_r = new_col[2];
        need_rebuild_ = true;
    }

    void set_second_color (const cv::Scalar & new_col)
    {
        params_.c1_b = new_col[0];
        params_.c1_g = new_col[1];
        params_.c1_r = new_col[2];
        need_rebuild_ = true;
    }

    bool set_parameters (const QJsonObject &json) override
    {
        gradient_overlay_parameters tmp = params_;
        bool ok = json_to_filter (&tmp, gradient_overlay_parameters::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;

            params_.c0_b = std::clamp (params_.c0_b, 0.f, 255.f);
            params_.c0_g = std::clamp (params_.c0_g, 0.f, 255.f);
            params_.c0_r = std::clamp (params_.c0_r, 0.f, 255.f);

            params_.c1_b = std::clamp (params_.c1_b, 0.f, 255.f);
            params_.c1_g = std::clamp (params_.c1_g, 0.f, 255.f);
            params_.c1_r = std::clamp (params_.c1_r, 0.f, 255.f);

            params_.k = std::clamp (params_.k, 0.f, 1.f);
            params_.axis = params_.axis ? 1 : 0;
            need_rebuild_ = true;
        }
        return ok;
    }

    QJsonObject parameters () const override
    {
        return filter_to_json (&params_, gradient_overlay_parameters::staticMetaObject);
    }
private:
    gradient_overlay_parameters params_;

    bool need_rebuild_ {false};
    cv::Mat gradient_;

    void build_gradient (const cv::Size & sz)
    {
        gradient_.create (sz, CV_8UC3);

        if (params_.axis == 1) // vertical
        {
            for (int y = 0; y < sz.height; y++)
            {
                float t = (sz.height > 1) ? static_cast<float> (y) / (sz.height - 1) : 0.f;

                cv::Vec3b c
                (
                    static_cast<uint8_t> ((1 - t) * params_.c0_b + t * params_.c1_b),
                    static_cast<uint8_t> ((1 - t) * params_.c0_g + t * params_.c1_g),
                    static_cast<uint8_t> ((1 - t) * params_.c0_r + t * params_.c1_r)
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
                    static_cast<uint8_t> ((1 - t) * params_.c0_b + t * params_.c1_b),
                    static_cast<uint8_t> ((1 - t) * params_.c0_g + t * params_.c1_g),
                    static_cast<uint8_t> ((1 - t) * params_.c0_r + t * params_.c1_r)
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