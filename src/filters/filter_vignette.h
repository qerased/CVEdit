#ifndef CVEDIT_FILTER_VIGNETTE_H
#define CVEDIT_FILTER_VIGNETTE_H

#include "filter.h"

struct vignette_params
{
    Q_GADGET
    Q_PROPERTY (float strength)
    Q_PROPERTY (float radius)

public:
    float strength{0.5f};
    float radius{0.f};
};

class filter_vignette : public filter
{
public:
    const char * id () const override { return "vignette"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty () || params_.strength <= 0.) return;

        int w = mat.cols;
        int h = mat.rows;

        if (mask_.empty () || mask_.rows != h || mask_.cols != w || cached_radius_ != params_.radius)
        {
            build_mask (w, h, params_.radius);
        }

        cv::Mat srcf;
        mat.convertTo (srcf, CV_32FC3, 1.f / 255.f);

        cv::Mat scale;
        scale = 1.0f - params_.strength * (1.0f - mask_);

        std::vector<cv::Mat> ch;
        cv::split (srcf, ch);

        for (auto& c : ch)
            cv::multiply (c, scale, c);

        cv::merge (ch, srcf);

        srcf.convertTo (mat, CV_8UC3, 255.f);
    }

    void set_strength (float strength) { params_.strength = std::clamp (strength, 0.f, 1.f); }
    void set_radius (float radius) { params_.radius = std::clamp (radius, 0.f, 0.999f); }

    bool set_parameters (const QJsonObject &json) override
    {
        vignette_params tmp = params_;
        bool ok = json_to_filter (&tmp, vignette_params::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;
            params_.strength = std::clamp (params_.strength, 0.f, 1.f);
            params_.radius = std::clamp (params_.radius, 0.f, 0.999f);
        }
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, vignette_params::staticMetaObject);
    }

private:
    vignette_params params_;
    float cached_radius_{-1.f};

    cv::Mat mask_;
    void build_mask (int w, int h, float radius)
    {
        mask_.create (h, w, CV_32F);
        const float cx = (w - 1) * 0.5f;
        const float cy = (h - 1) * 0.5f;
        const float maxd = std::sqrt (cx * cx + cy * cy);

        for (int y = 0; y < h; y++)
        {
            float * row = mask_.ptr<float> (y);
            for (int x = 0; x < w; x++)
            {
                float d = std::sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) / maxd;
                float t = (d - radius) / std::max (1e-6f, (1.f - radius));
                float m = 1.f - std::clamp(t, 0.f, 1.f);
                row[x] = m;
            }
        }
        cached_radius_ = params_.radius;
    }
};

#endif //CVEDIT_FILTER_VIGNETTE_H