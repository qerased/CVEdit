#ifndef CVEDIT_FILTER_H
#define CVEDIT_FILTER_H
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "filter_param_helper.h"

struct frame_info
{
    unsigned int tick_num_;
};

class filter
{
public:
    virtual ~filter ();
    virtual const char * id () const = 0;

    void set_enabled (bool e) { enabled_ = e; }
    bool enabled () const { return enabled_; }
    virtual void apply (cv::Mat & mat) = 0;

    virtual void apply (cv::Mat & mat, frame_info & fi)
    {
        apply (mat);
    }

    void set_num_order (int new_ord)
    {
        num_order_ = new_ord;
    }

    virtual QJsonObject parameters () const { return QJsonObject (); }
    virtual bool set_parameters (const QJsonObject & json) { return true; }

private:
    bool enabled_ {true};
    int num_order_{};

    friend class filter_chain;
};


class filter_chain
{
public:
    template<typename T, typename... Args>
    T * add (Args&&... args)
    {
        auto ptr = std::make_unique<T> (std::forward<Args> (args)...);
        T * raw = ptr.get ();
        filters_.push_back (std::move (ptr));
        return raw;
    }

    void clear () { filters_.clear (); }

    void apply_all (cv::Mat& mat, frame_info & fi)
    {
        std::ranges::sort (filters_, []
        (const std::unique_ptr<filter> & a, const std::unique_ptr<filter> & b)
        {
            return a->num_order_ < b->num_order_;
        });

        for (auto & f : filters_)
            if (f->enabled ())
                f->apply(mat, fi);
    }

    void disable_all ()
    {
        for (auto & f : filters_)
            f->set_enabled (false);
    }

    std::vector<std::unique_ptr<filter>> & filters () { return filters_; }

private:
    std::vector<std::unique_ptr<filter>> filters_;
};

#endif //CVEDIT_FILTER_H