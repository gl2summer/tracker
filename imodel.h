#ifndef IMODEL_H
#define IMODEL_H

#include <QVariant>



class IModel
{
public:
    IModel(){}
    virtual ~IModel(){}

    virtual QVariant getParamter(QString key) = 0;
    virtual void setParameter(QString key, QVariant v) = 0;
};

#endif // IMODEL_H
