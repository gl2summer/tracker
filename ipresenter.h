#ifndef IPRESENTER_H
#define IPRESENTER_H

#include <QVariant>



class IPresenter
{
public:
    IPresenter(){}
    virtual ~IPresenter(){}

    virtual QVariant getParamter(QString key) = 0;
    virtual void setParameter(QString key, QVariant v) = 0;
};

#endif // IPRESENTER_H
