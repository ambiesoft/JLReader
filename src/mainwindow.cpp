#include <QDir>

#include "../../lsMisc/stdQt/stdQt.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace AmbiesoftQt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_Test_triggered()
{
//    QString file=pathCombine(QDir::currentPath(),
//                             "../../src/xmlsample/340AC0000000033_20190701_431AC0000000006.xml");
    QString file=pathCombine(QDir::currentPath(),
                             "../../src/xmlsample/sample01.xml");

    Alert(this, QDir::currentPath());
}
