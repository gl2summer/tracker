#ifndef IVIEW_H
#define IVIEW_H

#include <QString>



class IView
{
public:
    IView(){}
    virtual ~IView(){}

    virtual void showMessage(QString) = 0;
};

#endif // IVIEW_H
