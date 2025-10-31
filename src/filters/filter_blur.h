#ifndef CVEDIT_FILTER_BLUR_H
#define CVEDIT_FILTER_BLUR_H
#include "filter.h"

class filter_blur : public filter
{
public:
    const char * name () const override { return "Gaussian Blur"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty ()) return;
        cv::GaussianBlur (mat, mat, {k_size, k_size}, sigmaX, sigmaY);
    }

    void change_k_size (int new_k)
    {
        /// k size must be odd
        k_size = std::max (1, new_k - !(new_k % 2));
    }
private:
    int k_size {5};
    int sigmaX{}, sigmaY{};
};

#endif //CVEDIT_FILTER_BLUR_H
