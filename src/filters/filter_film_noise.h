#ifndef CVEDIT_FILTER_FILM_NOISE_H
#define CVEDIT_FILTER_FILM_NOISE_H
#include "filter.h"

class filter_film_noise : public filter
{
public:
    const char * name () const override { return "film noise"; }

    void apply (cv::Mat &mat) override
    {
        frame_info fake{0};
        apply (mat, fake);
    };

    void apply (cv::Mat &mat, frame_info &fi) override
    {
        if (mat.empty ())
            return;

        cv::Mat srcf;
        mat.convertTo (srcf, CV_32F, 1. / 255.);

        /// Flickering
        float phase = static_cast<float> (fi.tick_num_ % 1000) * (2 * M_PI / 100);
        float bright = 1.f + flicker_ * std::sin (phase);
        srcf *= bright;

        /// grain
        if (grain_ > 0.f)
        {
            cv::Mat noise (mat.size (), CV_32FC3);
            cv::randn (noise, 0.0, grain_ * 0.8);
            srcf += noise;
        }

        /// scratches
        if (scratches_ > 0.f)
        {
            rng.state = static_cast<uint64>(0x1234ABCD) + static_cast<uint64>(fi.tick_num_ * 12345678ULL);
            int lines = static_cast<int> (scratches_ * 6. + rng.uniform (0, 3));
            for (int i = 0; i < lines; i++)
            {
                int x1 = rng.uniform (0, mat.cols), y1 = rng.uniform (0, mat.rows);
                int x2 = rng.uniform (0, mat.cols), y2 = rng.uniform (0, mat.rows);

                float intensity = rng.uniform (0.f, 1.f) < 0.5f ? 1.4f : -0.4f;
                int thickness = rng.uniform (1, 2);
                cv::line (srcf, {x1, y1}, {x2, y2},
                    cv::Scalar::all (intensity), thickness, cv::LINE_AA);
            }
        }

        cv::threshold (srcf, srcf, 1., 1., cv::THRESH_TRUNC);
        cv::threshold (srcf, srcf, 0., 0., cv::THRESH_TOZERO);
        srcf.convertTo (mat, CV_8UC3, 255.);
    }

    void set_grain (float gr) { grain_ = gr; }
    void set_scratches (float s) { scratches_ = s; }
    void set_flicker (float f) {flicker_ = f; }
private:
    float grain_{0.25f};
    float scratches_{0.15f};
    float flicker_{0.06f};

    cv::RNG rng;
};

#endif //CVEDIT_FILTER_FILM_NOISE_H