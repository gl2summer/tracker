#ifndef MODELIMPL_H
#define MODELIMPL_H

#include "imodel.h"

#include <QString>
#include <QVariant>
#include <functional>



class Model : public IModel
{
public:
    Model();
    virtual ~Model() override;

    // IModel interface
public:
    QVariant getParamter(QString key) override;
    void setParameter(QString key, QVariant v, std::function<void(Result)> cb) override;
};

#endif // MODELIMPL_H
