#ifndef CVEDIT_FILTER_H
#define CVEDIT_FILTER_H
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <QJsonArray>

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

    int num_order () const { return num_order_; }

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

    QJsonObject to_json () const
    {
        QJsonObject result;
        QJsonArray filters_array;

        for (const auto & f : filters_)
        {
            if (!f->enabled ())
                continue;

            QJsonObject filter_obj;
            filter_obj["id"] = f->id ();
            filter_obj["enabled"] = f->enabled ();
            filter_obj["order"] = f->num_order_;
            filter_obj["parameters"] = f->parameters ();

            filters_array.append (filter_obj);
        }

        result["filters"] = filters_array;
        return result;
    }

    bool from_json (const QJsonObject & json)
    {
        if (!json.contains ("filters") || !json["filters"].isArray ())
            return false;

        disable_all ();

        QJsonArray filters_array = json["filters"].toArray ();
        bool all_ok = true;

        for (const auto & filter_val : filters_array)
        {
            if (!filter_val.isObject ())
            {
                all_ok = false;
                continue;
            }

            QJsonObject filter_obj = filter_val.toObject ();

            if (!filter_obj.contains ("id") || !filter_obj["id"].isString ())
            {
                all_ok = false;
                continue;
            }

            QString filter_id = filter_obj["id"].toString ();

            filter * found_filter = nullptr;
            for (auto & f : filters_)
            {
                if (QString (f->id ()) == filter_id)
                {
                    found_filter = f.get ();
                    break;
                }
            }

            if (!found_filter)
                continue;

            if (filter_obj.contains ("enabled") && filter_obj["enabled"].isBool ())
                found_filter->set_enabled (filter_obj["enabled"].toBool ());

            if (filter_obj.contains ("order") && filter_obj["order"].isDouble ())
                found_filter->set_num_order (filter_obj["order"].toInt ());

            if (filter_obj.contains ("parameters") && filter_obj["parameters"].isObject ())
            {
                QJsonObject params = filter_obj["parameters"].toObject ();
                if (!found_filter->set_parameters (params))
                    all_ok = false;
            }
        }

        return all_ok;
    }

    std::vector<std::unique_ptr<filter>> & filters () { return filters_; }

private:
    std::vector<std::unique_ptr<filter>> filters_;
};

#endif //CVEDIT_FILTER_H