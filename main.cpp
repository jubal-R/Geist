/*
By Jubal - Geist - All purpose text editor
    Copyright (C) 2016  Jubal
*/
#include <QApplication>
#include "mainwindow.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    if(argc > 1){
        w.openWith(QString::fromStdString(argv[1]));
    }
    
    return a.exec();
}
