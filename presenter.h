#ifndef PRESENTER_H
#define PRESENTER_H

#include "imodel.h"
#include "ipresenter.h"
#include "iview.h"


class Presenter : public IPresenter
{
public:
    Presenter(IView *v, IModel *m);

private:
    IView* view;
    IModel* model;

    // IPresenter interface
public:
    QVariant getParamter(QString key) override;
    void setParameter(QString key, QVariant v) override;
};

#endif // PRESENTER_H
