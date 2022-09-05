#include "EditorWidget.h"

#include <QDebug>
#include <QMetaProperty>
#include <QVariant>

EditorWidget::EditorWidget(QWidget *parent)
    : QWidget{parent}
{

}

void EditorWidget::setupUIFor(const QMetaObject &obj)
{
    qDebug()<< "Setting up editing UI for"<<obj.className();
    // visit every exposed property
    for(int i=0; i<obj.propertyCount(); ++i) {
        const auto &prop = obj.property(i);
        qDebug() <<prop.name();
    }
}
