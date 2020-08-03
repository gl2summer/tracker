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


void Model::setParameter(QString key, QVariant v)
{
    QSettings setting("hao", "tracker");
    setting.setValue(key, v);
}
