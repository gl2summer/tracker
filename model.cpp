#include "model.h"

#include <QSettings>

Model::Model()
{

}

Model::~Model()
{

}


QVariant Model::getParamter(QString key)
{
    QSettings setting("hao", "tracker");
    return setting.value(key);
}


void Model::setParameter(QString key, QVariant v, std::function<void(Result)> cb)
{
    QSettings setting("hao", "tracker");
    setting.setValue(key, v);
    if(cb != nullptr) {
        cb(SUCCESS);
    }
}
