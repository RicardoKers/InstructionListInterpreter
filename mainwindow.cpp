#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox.h>
#include "simulaIL.h"
#include "stdio.h"


Data* MemoryData;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(timer1Event()));

    // inicialização
    MemoryData=inicializa();
    char tmp[200];
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(9);
    ui->tableWidget->setColumnWidth(0,30);
    QStringList headers;
    headers << " " << "X" << "Y" << "M" << "R" << "TP" << "TV" << "CP" << "CV";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setColumnWidth(1,60);
    ui->tableWidget->setColumnWidth(2,60);
    ui->tableWidget->setColumnWidth(3,60);
    ui->tableWidget->setColumnWidth(4,60);
    ui->tableWidget->setColumnWidth(5,60);
    ui->tableWidget->setColumnWidth(6,60);
    ui->tableWidget->setColumnWidth(7,60);
    ui->tableWidget->setColumnWidth(8,60);

    for (int i = 0; i < Max_Memorias; i++)
    {
    /*
    Entradas[i]
    Saidas[i]
    Memorias[i]
    Registradores[i]
    TempPreset[i]
    TempValue[i]
    ContPreset[i]
    ContValue[i]
    */

        ui->tableWidget->removeRow(i);
        ui->tableWidget->insertRow(i);
        sprintf(tmp,"%d",i);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Entradas[i]);
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Saidas[i]);
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Memorias[i]);
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Registradores[i]);
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->TempPreset[i]);
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->TempValue[i]);
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->ContPreset[i]);
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->ContValue[i]);
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(tmp));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSair_triggered()
{
    exit(0);
}

void MainWindow::on_actionAvan_ar_triggered()
{
    char tmp[200];
    for (int i = 0; i < Max_Memorias; i++)
    {
         ui->tableWidget->removeRow(i);
        ui->tableWidget->insertRow(i);
        sprintf(tmp,"%d",i);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Entradas[i]);
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Saidas[i]);
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Memorias[i]);
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Registradores[i]);
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->TempPreset[i]);
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->TempValue[i]);
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->ContPreset[i]);
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->ContValue[i]);
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(tmp));
    }

    if(MemoryData->acumulador==0) ui->label->setText("Acumulador = 0");
    else ui->label->setText("Acumulador = 1");
}

int altLinha=0;
int altColuna=0;

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    // Valor alterado na tabela
    //"X" "Y" "M" "R" "TP" "TV" "CP" "CV";
    char tmp[100];
    if(altColuna==1) //X
    {
        if(item->text().toInt()==1)
            MemoryData->Entradas[altLinha]=1;
        if(item->text().toInt()==0)
            MemoryData->Entradas[altLinha]=0;
    }
    if(altColuna==3) //M
    {
        if(item->text().toInt()==1)
            MemoryData->Memorias[altLinha]=1;
        if(item->text().toInt()==0)
            MemoryData->Memorias[altLinha]=0;
    }
    if(altColuna==4) //R
    {
        MemoryData->Registradores[altLinha]=item->text().toInt();
    }
    if(altColuna==5) //TP
    {
        MemoryData->TempPreset[altLinha]=item->text().toUInt();
    }
    if(altColuna==6) //TV
    {
        MemoryData->TempValue[altLinha]=item->text().toUInt();
    }
    if(altColuna==7) //CP
    {
        MemoryData->ContPreset[altLinha]=item->text().toUInt();
    }
    if(altColuna==8) //CV
    {
        MemoryData->ContValue[altLinha]=item->text().toUInt();
    }
    if(altColuna>0) timer1->start(10);
    altLinha=0;
    altColuna=0;

    //sprintf(tmp,"L=%d C=%d",altLinha,altColuna);
    //QMessageBox::warning(this,"Aviso",tmp);
}


void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    altLinha=row;
    altColuna=column;
}

void MainWindow::timer1Event()
{
    timer1->stop();
    char tmp[200];
    for (int i = 0; i < Max_Memorias; i++)
    {
        ui->tableWidget->removeRow(i);
        ui->tableWidget->insertRow(i);
        sprintf(tmp,"%d",i);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Entradas[i]);
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Saidas[i]);
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Memorias[i]);
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->Registradores[i]);
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->TempPreset[i]);
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->TempValue[i]);
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->ContPreset[i]);
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(tmp));
        sprintf(tmp,"%d",MemoryData->ContValue[i]);
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(tmp));
    }
}

