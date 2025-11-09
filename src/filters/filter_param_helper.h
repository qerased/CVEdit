#ifndef CVEDIT_FILTER_PARAM_HELPER_H
#define CVEDIT_FILTER_PARAM_HELPER_H
#include <QJsonObject>
#include <QMetaProperty>

inline QJsonObject filter_to_json (const void * obj, const QMetaObject & meta)
{
    QJsonObject out;
    for (int i = 0; i < meta.propertyCount (); i++)
    {
        const QMetaProperty prop = meta.property (i);
        if (!prop.isReadable () || QString (prop.name ()) == "object_name")
            continue;
        out[prop.name ()] = QJsonValue::fromVariant (prop.readOnGadget (obj));
    }
    return out;
}

/// TODO: parse errors here
inline bool json_to_filter (void * obj, const QMetaObject & meta, const QJsonObject & in)
{
    bool ok = true;
    for (int i = 0; i < meta.propertyCount (); i++)
    {
        const QMetaProperty prop = meta.property (i);
        const auto name = QString (prop.name ());

        if (!in.contains (name))
            continue;

        QVariant v = in[name].toVariant ();
        if (!v.convert (prop.metaType ()))
        {
            ok = false;
            continue;
        }
        if (!prop.writeOnGadget (obj, v))
            ok = false;
    }
    return ok;
}

#endif //CVEDIT_FILTER_PARAM_HELPER_H
