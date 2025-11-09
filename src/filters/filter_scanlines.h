#ifndef CVEDIT_FILTER_SCANLINES_H
#define CVEDIT_FILTER_SCANLINES_H

#include "filter.h"

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
        if (mat.empty () || k_ <= 0) return;

        int offset = static_cast<int> (std::floor (speed_ * static_cast<float> (fi.tick_num_))) % period_;
        for (int y = 0; y < mat.rows; y++)
        {
            if ((y + offset) % period_ == 0)
            {
                cv::Vec3b * row = mat.ptr<cv::Vec3b> (y);
                for (int x = 0; x < mat.cols; x++)
                {
                    row[x][0] = static_cast<uint8_t> ((1. - k_) * row[x][0]);
                    row[x][1] = static_cast<uint8_t> ((1. - k_) * row[x][1]);
                    row[x][2] = static_cast<uint8_t> ((1. - k_) * row[x][2]);
                }
            }
        }
    }

    void set_density (int dens) { period_ = std::max(1, dens); }
    void set_darkness (float dark) { k_ = dark; }
    void set_speed (float speed) { speed_ = speed; }
private:
    int period_{2};
    float k_{0.25f};
    float speed_{0.f};
};

#endif //CVEDIT_FILTER_SCANLINES_H