#include "presenter.h"

Presenter::Presenter(IView *v, IModel *m)
    : view(v)
    , model(m)
{

}

QVariant Presenter::getParamter(QString key)
{
    return model->getParamter(key);
}

void Presenter::setParameter(QString key, QVariant v)
{
    model->setParameter(key, v);
    view->showMessage("Parameter("+key+") saved!");
}
