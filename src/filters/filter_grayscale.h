#ifndef CVEDIT_FILTER_GRAYSCALE_H
#define CVEDIT_FILTER_GRAYSCALE_H
#include "filter.h"

class filter_grayscale : public filter
{
public:
    const char * name () const override { return "grayscale"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty ()) return;
        cv::Mat gray;
        cv::cvtColor (mat, gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor (gray, mat, cv::COLOR_GRAY2RGB);
    }
};

#endif //CVEDIT_FILTER_GRAYSCALE_H