/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionClose;
    QWidget *centralwidget;
    QLabel *labelTop;
    QProgressBar *progressBar;
    QFrame *line;
    QLabel *label;
    QPushButton *pushButtonGPU;
    QPushButton *pushButtonCPU;
    QPushButton *pushButtonFirstHit;
    QPushButton *pushButtonAlphaComp;
    QPushButton *pushButtonAverage;
    QSlider *slider_IncreaseIntensity;
    QLabel *label_2;
    QLabel *label_sliderValue;
    QMenuBar *menubar;
    QMenu *menuFile;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1000, 812);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        labelTop = new QLabel(centralwidget);
        labelTop->setObjectName(QStringLiteral("labelTop"));
        labelTop->setGeometry(QRect(30, 10, 1001, 16));
        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setEnabled(false);
        progressBar->setGeometry(QRect(787, 20, 201, 23));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        line = new QFrame(centralwidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 60, 1011, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 80, 731, 701));
        pushButtonGPU = new QPushButton(centralwidget);
        pushButtonGPU->setObjectName(QStringLiteral("pushButtonGPU"));
        pushButtonGPU->setGeometry(QRect(120, 40, 91, 23));
        pushButtonCPU = new QPushButton(centralwidget);
        pushButtonCPU->setObjectName(QStringLiteral("pushButtonCPU"));
        pushButtonCPU->setGeometry(QRect(20, 40, 91, 23));
        pushButtonFirstHit = new QPushButton(centralwidget);
        pushButtonFirstHit->setObjectName(QStringLiteral("pushButtonFirstHit"));
        pushButtonFirstHit->setGeometry(QRect(360, 40, 111, 23));
        pushButtonAlphaComp = new QPushButton(centralwidget);
        pushButtonAlphaComp->setObjectName(QStringLiteral("pushButtonAlphaComp"));
        pushButtonAlphaComp->setGeometry(QRect(480, 40, 101, 23));
        pushButtonAverage = new QPushButton(centralwidget);
        pushButtonAverage->setObjectName(QStringLiteral("pushButtonAverage"));
        pushButtonAverage->setGeometry(QRect(220, 40, 131, 23));
        slider_IncreaseIntensity = new QSlider(centralwidget);
        slider_IncreaseIntensity->setObjectName(QStringLiteral("slider_IncreaseIntensity"));
        slider_IncreaseIntensity->setGeometry(QRect(780, 110, 191, 19));
        slider_IncreaseIntensity->setMinimum(1);
        slider_IncreaseIntensity->setMaximum(1000);
        slider_IncreaseIntensity->setOrientation(Qt::Horizontal);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(760, 80, 111, 16));
        label_sliderValue = new QLabel(centralwidget);
        label_sliderValue->setObjectName(QStringLiteral("label_sliderValue"));
        label_sliderValue->setGeometry(QRect(947, 140, 20, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1000, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(actionClose);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Visualisierung 1", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Open ...", 0));
        actionClose->setText(QApplication::translate("MainWindow", "Close", 0));
        labelTop->setText(QApplication::translate("MainWindow", "No data loaded", 0));
        label->setText(QString());
        pushButtonGPU->setText(QApplication::translate("MainWindow", "gpu raycasting", 0));
        pushButtonCPU->setText(QApplication::translate("MainWindow", "cpu raycasting", 0));
        pushButtonFirstHit->setText(QApplication::translate("MainWindow", "first hit raycasting", 0));
        pushButtonAlphaComp->setText(QApplication::translate("MainWindow", "alpha compositing", 0));
        pushButtonAverage->setText(QApplication::translate("MainWindow", "average value raycasting", 0));
        label_2->setText(QApplication::translate("MainWindow", "Intensit\303\244tsverst\303\244rkung", 0));
        label_sliderValue->setText(QApplication::translate("MainWindow", "1", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
