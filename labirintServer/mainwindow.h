#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Labirint.h"

#include <QMainWindow>
#include<QStyle>
#include <QTcpServer>
#include <QTcpSocket>
#include<QVector>
#include <QDataStream>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QMessageBox>
#include <QThread>
#include <QBuffer>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow
     * @param parent
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_StartServer_clicked();


    // сетевая часть Сервера--------------------------------------------
    void newConnection();
    void Read_Data_Fron_Socket();// чтение данных принятых из сети
    void SendMesh(); // отправка карты лабиринта
    void SendPath();// отправка решения лабиринта
    //----------------------------------


 signals:
    /**
    * @brief CallSendPath : сигнал вызываемый при приеме кординат конечной ячейки
    */
   void  CallSendPath();

private:
    Ui::MainWindow *ui;

    QThread thread;
    Labirint _labirint;


  //  сетевая часть Сервера _____________________________________________
    QString Info_Client;//сетевая информацияо клиенте
    QTcpServer *_Server;
    QList<QTcpSocket*> Client_Conection_List; // список подключенных клиентов
    void Add_New_CLient_Conection(QTcpSocket *socket);
    int countStep; // количество шагов

  //____________________________________________________
};
#endif // MAINWINDOW_H
