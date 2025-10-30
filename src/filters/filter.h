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
    bool enabled () const { return enabled_; }
    virtual void apply (cv::Mat & mat);

private:
    bool enabled_ {true};
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

    void apply_all (cv::Mat& mat)
    {
        for (auto & f : filters_)
            if (f->enabled ())
                f->apply(mat);
    }

    std::vector<std::unique_ptr<filter>> & filters () { return filters_; }

private:
    std::vector<std::unique_ptr<filter>> filters_;
};

#endif //CVEDIT_FILTER_H