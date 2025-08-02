#include <QApplication>
#include "mainwindow.h"
#include <gtest/gtest.h>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

   ::testing::InitGoogleTest(&argc, argv);


    w.show();
   return  app.exec() ;/**/// или RUN_ALL_TESTS(); для работы тестов*/
}
