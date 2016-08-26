/*
By Jubal - Geist - All purpose text editor
    Copyright (C) 2016  Jubal
*/
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
