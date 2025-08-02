#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "labirint.h"


#include <QtCore>
#include <QMainWindow>
#include <QStyle>


#include <QHostAddress>
#include <QTcpSocket>

#include <QDataStream>
#include <QByteArray>
#include <QRegExpValidator>

#include <QThread>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QSettings>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief класс  главного окна MainWindow
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief  конструктор MainWindow
     * @param  parent
     */
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_txtCountRow_textChanged(const QString &arg1);
    void on_txtCountColum_textChanged(const QString &arg1);
    void on_PushGeneration_clicked();
    void on_ClientMode_triggered();
    void on_ServerMode_triggered();
    void on_ScenaMap_cellClicked(int row, int column);
    void on_ConectedToServer_clicked();
    void Recived();

    //сетевая часть клиента-----------------------------------------
    void Read_Data_From_Socket();
    void StatusConnected(QAbstractSocket::SocketError);
//-------------------------------------------------------

private:
    Ui::MainWindow *ui;

  //сетевая часть клиента---------------------------------------------
    QByteArray Data;    
    QTcpSocket *TCPSocket;
    QString message_For_Server;
    QString recived;
    QVector<QVector<int>> mesh;
    QVector<QPair<int, int>> path;
    int RowCell;
    int ColumCell;
    bool ReciveMeshOrPath=false; // статус приема  от сервера fasle: принимается карта лабиринта , true: принимается решение лабиринта.
  //-------------------------------------------------------

    QThread thread;
    labirint _labirint;
    QSettings settings;


    bool StatusMode=false; // статус режима выполнения fasle: выполняется на клиенте, true: выполняется на сервере.
    bool flagSetingAPosition=false; // статус стартовой ячейки fasle: не задана, true: задана.
};
#endif // MAINWINDOW_H
