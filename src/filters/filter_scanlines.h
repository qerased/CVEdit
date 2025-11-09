#ifndef CVEDIT_FILTER_SCANLINES_H
#define CVEDIT_FILTER_SCANLINES_H

#include "filter.h"

struct scanlines_params
{
    Q_GADGET
    Q_PROPERTY (int period MEMBER period)
    Q_PROPERTY (float k MEMBER k)
    Q_PROPERTY (float speed MEMBER speed)

public:
    int period{2};
    float k{0.25f};
    float speed{0.f};
};

class filter_scanlines : public filter
{
public:
    const char * id () const override { return "scanlines"; }

    void apply (cv::Mat & mat) override
    {
        frame_info fake{0};
        apply (mat, fake);
    }

    void apply (cv::Mat & mat, frame_info &fi) override
    {
        if (mat.empty () || params_.k <= 0) return;

        int offset = static_cast<int> (std::floor (params_.speed * static_cast<float> (fi.tick_num_))) % params_.period;
        for (int y = 0; y < mat.rows; y++)
        {
            if ((y + offset) % params_.period == 0)
            {
                cv::Vec3b * row = mat.ptr<cv::Vec3b> (y);
                for (int x = 0; x < mat.cols; x++)
                {
                    row[x][0] = static_cast<uint8_t> ((1. - params_.k) * row[x][0]);
                    row[x][1] = static_cast<uint8_t> ((1. - params_.k) * row[x][1]);
                    row[x][2] = static_cast<uint8_t> ((1. - params_.k) * row[x][2]);
                }
            }
        }
    }

    void set_density (int dens) { params_.period = std::max(1, dens); }
    void set_darkness (float dark) { params_.k = dark; }
    void set_speed (float speed) { params_.speed = speed; }

    bool set_parameters (const QJsonObject &json) override
    {
        scanlines_params tmp = params_;
        bool ok = json_to_filter (&tmp, scanlines_params::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;
            params_.period = std::max(1, params_.period);
        }
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, scanlines_params::staticMetaObject);
    }

private:
    scanlines_params params_;
};

#endif //CVEDIT_FILTER_SCANLINES_H