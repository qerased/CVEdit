#ifndef CVEDIT_FILTER_EDGEGLOW_H
#define CVEDIT_FILTER_EDGEGLOW_H
#include "filter.h"

struct bloom_params
{
    Q_GADGET
    Q_PROPERTY (float thresh MEMBER thresh)
    Q_PROPERTY (float radius MEMBER radius)
    Q_PROPERTY (float coeff MEMBER coeff)

    Q_PROPERTY (float glow_color_b MEMBER glow_color_b)
    Q_PROPERTY (float glow_color_g MEMBER glow_color_g)
    Q_PROPERTY (float glow_color_r MEMBER glow_color_r)

public:
    float thresh{0.7};
    float radius{10.};
    float coeff{0.8};

    float glow_color_b{1.f};
    float glow_color_g{0.5f};
    float glow_color_r{0.f};
};

class filter_bloom : public filter
{
public:
    const char * id () const override { return "bloom"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty ()) return;

        cv::Mat srcf;
        mat.convertTo (srcf, CV_32F, 1.0 / 255.0);

        cv::Mat gray;
        cv::cvtColor (srcf, gray, cv::COLOR_BGR2GRAY);

        cv::Mat bright;
        cv::threshold (gray, bright, params_.thresh, 1.0, cv::THRESH_TOZERO);

        cv::Mat glow;
        cv::GaussianBlur (bright, glow, cv::Size(0, 0), params_.radius);

        cv::Mat glow_rgb;
        cv::cvtColor (glow, glow_rgb, cv::COLOR_GRAY2BGR);

        cv::multiply (glow_rgb,
              cv::Scalar{params_.glow_color_b, params_.glow_color_g, params_.glow_color_r},
                   glow_rgb);

        cv::Mat result;
        cv::addWeighted (srcf, 1.0, glow_rgb, params_.coeff, 0.0, result);

        cv::min (result, 1.0, result);
        cv::max (result, 0.0, result);

        result.convertTo (mat, CV_8U, 255.0);
    }

    void set_thresh (float new_thr) { params_.thresh = new_thr; }
    void set_coeff (float new_coef) { params_.coeff = new_coef; }
    void set_radius (float new_rad) { params_.radius = new_rad; }

    void set_bloom_color (cv::Scalar new_col)
    {
        params_.glow_color_b = new_col[0];
        params_.glow_color_g = new_col[1];
        params_.glow_color_r = new_col[2];
    }

    bool set_parameters (const QJsonObject &json) override
    {
        bloom_params tmp = params_;
        bool ok = json_to_filter (&tmp, bloom_params::staticMetaObject, json);
        if (ok) /// TODO: check this in a better way maybe
        {
            params_ = tmp;
            params_.thresh = std::clamp (tmp.thresh, 0.f, 1.f);
            params_.coeff = std::clamp (tmp.coeff, 0.f, 1.f);
            params_.radius = std::clamp (tmp.radius, 0.f, 20.f);
            params_.glow_color_b = std::clamp (tmp.glow_color_b, 0.f, 1.f);
            params_.glow_color_g = std::clamp (tmp.glow_color_g, 0.f, 1.f);
            params_.glow_color_r = std::clamp (tmp.glow_color_r, 0.f, 1.f);
        }
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, bloom_params::staticMetaObject);
    }

private:
    bloom_params params_;
};

#endif //CVEDIT_FILTER_EDGEGLOW_H