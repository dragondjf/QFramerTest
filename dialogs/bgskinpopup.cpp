#include "bgskinpopup.h"
#include<QFormLayout>
#include<QLabel>
#include<QEvent>
BgSkinPopup::BgSkinPopup(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedSize(200, 120);
    initUI();
    initConnect();
    installEventFilter(this);

    setObjectName(QString("SkinPopup"));
    setStyleSheet(QString(
                      "QWidget#SkinPopup{\
                          background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                            stop: 0 green, stop: 1 yellow);\
                          color: white;\
                      }"
                    )
    );
}

void BgSkinPopup::initUI()
{
    redSlider = new QSlider;
    redSlider->setOrientation(Qt::Horizontal);
    redSlider->setRange(0, 255);
    greenSlider = new QSlider;
    greenSlider->setOrientation(Qt::Horizontal);
    greenSlider->setRange(0, 255);
    blueSlider = new QSlider;
    blueSlider->setOrientation(Qt::Horizontal);
    blueSlider->setRange(0, 255);

    QFormLayout* mainLayout = new QFormLayout;
    mainLayout->addRow(tr("Red"), redSlider);
    mainLayout->addRow(tr("Green"), greenSlider);
    mainLayout->addRow(tr("Blue"), blueSlider);
    setLayout(mainLayout);

}

void BgSkinPopup::initConnect()
{
    connect(redSlider, SIGNAL(valueChanged()), this, SLOT(updateBackgroundColor()));
    connect(greenSlider, SIGNAL(valueChanged()), this, SLOT(updateBackgroundColor()));
    connect(blueSlider, SIGNAL(valueChanged()), this, SLOT(updateBackgroundColor()));
}


bool BgSkinPopup::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        close();
        return true;
    }
    else
    {
        return  QWidget::eventFilter(watched, event);
    }
}

void BgSkinPopup::updateBackgroundColor(const int value)
{
    int red = redSlider->value();
    int green = greenSlider->value();
    int blue = blueSlider->value();
    QString style = QString("QWidget#SkinPopup{\
                            background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0,\
                                                              stop:0 rgb(%1, 0, 0),\
                                                              stop:0.5 rgb(0, %2, 0),\
                                                              stop:1 rgb(0, 0, %3));\
                        }").arg(QString::number(blue), QString::number(green), QString::number(red));

    setStyleSheet(style);
}
