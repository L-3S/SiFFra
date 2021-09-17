#include "FbsfMessageBox.h"

FbsfMessageBox::FbsfMessageBox(QQuickItem *parent) :
    QQuickItem(parent)
{
}

int FbsfMessageBox::show()
{
    int ret = msgBox.exec();
    msgBox.close();
    return ret;
}

