#ifndef CVEDIT_FILTER_FILM_NOISE_H
#define CVEDIT_FILTER_FILM_NOISE_H
#include "filter.h"

struct film_noise_params
{
    Q_GADGET
    Q_PROPERTY (float grain MEMBER grain)
    Q_PROPERTY (float scratches MEMBER scratches)
    Q_PROPERTY (float flicker MEMBER flicker)

public:
    float grain{0.25f};
    float scratches{0.15f};
    float flicker{0.06f};
};

class filter_film_noise : public filter
{
public:
    const char * id () const override { return "film_noise"; }

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
        float bright = 1.f + params_.flicker * std::sin (phase);
        srcf *= bright;

        /// grain
        if (params_.grain > 0.f)
        {
            cv::Mat noise (mat.size (), CV_32FC3);
            cv::randn (noise, 0.0, params_.grain * 0.8);
            srcf += noise;
        }

        /// scratches
        if (params_.scratches > 0.f)
        {
            rng.state = static_cast<uint64>(0x1234ABCD) + static_cast<uint64>(fi.tick_num_ * 12345678ULL);
            int lines = static_cast<int> (params_.scratches * 6. + rng.uniform (0, 3));
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

    void set_grain (float gr) { params_.grain = gr; }
    void set_scratches (float s) { params_.scratches = s; }
    void set_flicker (float f) { params_.flicker = f; }

    bool set_parameters (const QJsonObject &json) override
    {
        film_noise_params tmp = params_;
        bool ok = json_to_filter (&tmp, film_noise_params::staticMetaObject, json);
        if (ok)
            params_ = tmp;
        return ok;
    }

    QJsonObject parameters () const override
    {
        return filter_to_json (&params_, film_noise_params::staticMetaObject);
    }

private:
    film_noise_params params_;
    cv::RNG rng;
};

#endif //CVEDIT_FILTER_FILM_NOISE_H