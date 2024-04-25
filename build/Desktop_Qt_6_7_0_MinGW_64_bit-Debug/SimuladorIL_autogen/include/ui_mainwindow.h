/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSair;
    QAction *actionAvan_ar;
    QAction *actionRodar;
    QAction *actionParar;
    QAction *actionResetar;
    QAction *actionPausar;
    QAction *actionResetar_2;
    QAction *actionCiclo;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QPlainTextEdit *plainTextEdit;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QTableWidget *tableWidget;
    QFrame *frame;
    QMenuBar *menubar;
    QMenu *menuArquivo;
    QMenu *menuSimular;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1033, 532);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../../../../../Projetos/SimuladorIL/Programas/SimuladorIL/App.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionSair = new QAction(MainWindow);
        actionSair->setObjectName("actionSair");
        actionAvan_ar = new QAction(MainWindow);
        actionAvan_ar->setObjectName("actionAvan_ar");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../../../../../Projetos/SimuladorIL/Programas/SimuladorIL/next.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAvan_ar->setIcon(icon1);
        actionRodar = new QAction(MainWindow);
        actionRodar->setObjectName("actionRodar");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("../../../../../../Projetos/SimuladorIL/Programas/SimuladorIL/play.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRodar->setIcon(icon2);
        actionParar = new QAction(MainWindow);
        actionParar->setObjectName("actionParar");
        QIcon icon3;
        icon3.addFile(QString::fromUtf8("../../../../../../Projetos/SimuladorIL/Programas/SimuladorIL/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionParar->setIcon(icon3);
        actionResetar = new QAction(MainWindow);
        actionResetar->setObjectName("actionResetar");
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("../../../../../../Projetos/SimuladorIL/Programas/SimuladorIL/reset.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionResetar->setIcon(icon4);
        actionResetar->setMenuRole(QAction::NoRole);
        actionPausar = new QAction(MainWindow);
        actionPausar->setObjectName("actionPausar");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8("../../../../../../Projetos/SimuladorIL/Programas/SimuladorIL/pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPausar->setIcon(icon5);
        actionResetar_2 = new QAction(MainWindow);
        actionResetar_2->setObjectName("actionResetar_2");
        actionResetar_2->setIcon(icon4);
        actionCiclo = new QAction(MainWindow);
        actionCiclo->setObjectName("actionCiclo");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8("../../../../../../Projetos/SimuladorIL/Programas/SimuladorIL/Cicle.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCiclo->setIcon(icon6);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        plainTextEdit = new QPlainTextEdit(centralwidget);
        plainTextEdit->setObjectName("plainTextEdit");
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        plainTextEdit->setFont(font);

        horizontalLayout->addWidget(plainTextEdit);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        label->setFont(font1);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        tableWidget = new QTableWidget(centralwidget);
        tableWidget->setObjectName("tableWidget");
        tableWidget->verticalHeader()->setVisible(false);

        horizontalLayout_2->addWidget(tableWidget);


        verticalLayout_2->addLayout(horizontalLayout_2);


        horizontalLayout->addLayout(verticalLayout_2);

        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);

        horizontalLayout->addWidget(frame);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1033, 17));
        menuArquivo = new QMenu(menubar);
        menuArquivo->setObjectName("menuArquivo");
        menuSimular = new QMenu(menubar);
        menuSimular->setObjectName("menuSimular");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);

        menubar->addAction(menuArquivo->menuAction());
        menubar->addAction(menuSimular->menuAction());
        menuArquivo->addAction(actionSair);
        menuSimular->addAction(actionAvan_ar);
        menuSimular->addAction(actionCiclo);
        menuSimular->addAction(actionRodar);
        menuSimular->addAction(actionPausar);
        menuSimular->addAction(actionParar);
        menuSimular->addAction(actionResetar_2);
        toolBar->addAction(actionAvan_ar);
        toolBar->addAction(actionCiclo);
        toolBar->addAction(actionRodar);
        toolBar->addAction(actionPausar);
        toolBar->addAction(actionParar);
        toolBar->addAction(actionResetar);
        toolBar->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "SimuladorIL", nullptr));
#if QT_CONFIG(statustip)
        MainWindow->setStatusTip(QCoreApplication::translate("MainWindow", "Kerschbaumer 2024", nullptr));
#endif // QT_CONFIG(statustip)
        actionSair->setText(QCoreApplication::translate("MainWindow", "Sair", nullptr));
        actionAvan_ar->setText(QCoreApplication::translate("MainWindow", "Avan\303\247ar", nullptr));
        actionRodar->setText(QCoreApplication::translate("MainWindow", "Rodar", nullptr));
        actionParar->setText(QCoreApplication::translate("MainWindow", "Parar", nullptr));
        actionResetar->setText(QCoreApplication::translate("MainWindow", "Resetar", nullptr));
        actionPausar->setText(QCoreApplication::translate("MainWindow", "Pausar", nullptr));
        actionResetar_2->setText(QCoreApplication::translate("MainWindow", "Resetar", nullptr));
        actionCiclo->setText(QCoreApplication::translate("MainWindow", "Ciclo", nullptr));
        plainTextEdit->setPlainText(QCoreApplication::translate("MainWindow", "LD X1\n"
"AND X2\n"
"SET Y1", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Acumulador = 0", nullptr));
        menuArquivo->setTitle(QCoreApplication::translate("MainWindow", "Arquivo", nullptr));
        menuSimular->setTitle(QCoreApplication::translate("MainWindow", "Simular", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
