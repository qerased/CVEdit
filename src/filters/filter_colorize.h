#ifndef CVEDIT_FILTER_COLORIZE_H
#define CVEDIT_FILTER_COLORIZE_H

#include "filter.h"

struct colorize_params
{
    Q_GADGET
    Q_PROPERTY (float k)
    Q_PROPERTY (unsigned int tint_b)
    Q_PROPERTY (unsigned int tint_g)
    Q_PROPERTY (unsigned int tint_r)

public:
    float k{0};
    unsigned int tint_b{180};
    unsigned int tint_g{180};
    unsigned int tint_r{255};
};

class filter_colorize : public filter
{
public:
    const char * id () const override { return "colorize"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty() || params_.k <= 0) return;
        cv::Mat tint (mat.size (), CV_8UC3, cv::Scalar(params_.tint_b, params_.tint_g, params_.tint_r));
        cv::addWeighted (mat, 1. - params_.k, tint, params_.k, 0., mat);
    }

    void set_tint_color (const cv::Scalar & new_col)
    {
        params_.tint_b = new_col[0];
        params_.tint_g = new_col[1];
        params_.tint_r = new_col[2];
    }

    void set_strength (const double newk) { params_.k = newk; }

    bool set_parameters (const QJsonObject &json) override
    {
        colorize_params tmp = params_;
        bool ok = json_to_filter (&tmp, colorize_params::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;
            params_.tint_b = std::clamp (params_.tint_b, 0u, 255u);
            params_.tint_g = std::clamp (params_.tint_g, 0u, 255u);
            params_.tint_r = std::clamp (params_.tint_r, 0u, 255u);
        }
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, colorize_params::staticMetaObject);
    }

private:
    colorize_params params_;
};

#endif //CVEDIT_FILTER_COLORIZE_H