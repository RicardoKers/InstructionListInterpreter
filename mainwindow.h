#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qlabel.h"
#include "qtablewidget.h"
#include <QTimer.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionSair_triggered();

    void on_actionAvan_ar_triggered();

    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void timer1Event();

private:
    Ui::MainWindow *ui;
    QTimer *timer1;
};
#endif // MAINWINDOW_H
