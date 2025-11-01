#ifndef CVEDIT_FILTER_KUWAHARA_H
#define CVEDIT_FILTER_KUWAHARA_H
#include "filter.h"

class filter_kuwahara : public filter
{
public:
    const char * name () const override { return "kuwahara"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty ()) return;
    }

    void set_k_size (int k)
    {
        k_size_ = std::max (3, k - !(k % 2));
    }

private:
    int k_size_ {3}; /// min k = 3 and must be odd
};

#endif //CVEDIT_FILTER_KUWAHARA_H