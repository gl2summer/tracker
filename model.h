#ifndef MODELIMPL_H
#define MODELIMPL_H

#include "imodel.h"

#include <QString>
#include <QVariant>



class Model : public IModel
{
public:
    Model();
    virtual ~Model();

    // IModel interface
public:
    QVariant getParamter(QString key) override;
    void setParameter(QString key, QVariant v) override;
};

#endif // MODELIMPL_H
