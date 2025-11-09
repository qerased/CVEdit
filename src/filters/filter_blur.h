#ifndef CVEDIT_FILTER_BLUR_H
#define CVEDIT_FILTER_BLUR_H
#include "filter.h"

struct blur_params
{
    Q_GADGET
    Q_PROPERTY (int k_size)

public:
    int k_size{5};
};

class filter_blur : public filter
{
public:
    const char * id () const override { return "gaussian_blur"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty ()) return;
        cv::GaussianBlur (mat, mat, {params_.k_size, params_.k_size}, 0, 0);
    }

    void change_k_size (int new_k)
    {
        /// k size must be odd
        params_.k_size = std::max (1, new_k - !(new_k % 2));
    }

    bool set_parameters (const QJsonObject & json) override
    {
        blur_params tmp = params_;
        bool ok = json_to_filter (&tmp, blur_params::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;
            params_.k_size = std::max (1, params_.k_size - !(params_.k_size % 2));
        }
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, blur_params::staticMetaObject);
    }
private:
    blur_params params_;
};

#endif //CVEDIT_FILTER_BLUR_H
