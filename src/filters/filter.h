#ifndef CVEDIT_FILTER_H
#define CVEDIT_FILTER_H
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

class filter
{
public:
    virtual ~filter() = default;
    virtual const char * name () const = 0;

    void set_enabled (bool e) { enabled_ = e; }
    virtual void apply (cv::Mat & mat);

private:
    bool enabled_ {true};
};

#endif //CVEDIT_FILTER_H