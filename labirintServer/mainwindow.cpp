#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&thread, &QThread::started,& _labirint, &Labirint::run); // установление связи сигнала: started и слота:  run
    connect(&_labirint, &Labirint::finished, this, &MainWindow::SendMesh);//установление связи сигнала: finished и слота:  Recived
    _labirint.moveToThread(&thread); // перемещение экземпляра класа labirint в новый поток


    connect(this, SIGNAL(CallSendPath()),this,SLOT(SendPath())); //установление связи сигнала: CallSendPath() и слота:  SendPath()
}

MainWindow::~MainWindow()
{
    delete ui;
}
/**
 * @brief  decision: скруктура для отправки решения лаабиринта по сети
 */
struct decision
{
    /**
     * @brief Count:количество шагов
     */
    int Count;
    /**
     * @brief INF: константа
     */
    int const INF;
    /**
     * @brief path: вектор пар кординат ячеек
     */
    QVector<QPair<int,int>> path;
};
/**
 * @brief serializePerson: метод производитсериализацию данных для отправки по сети
 * @param _decision: экземпляр скруктуры
 * @return QByteArray
 */
QByteArray serializePerson(const decision& _decision)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    stream << _decision.Count << _decision.INF<< _decision.path;


    return data;
}

void MainWindow::on_StartServer_clicked()
{
    _Server = new QTcpServer();
    // запуск сервера при  нажатие на кнопку
    if (_Server->listen(QHostAddress::Any, 8080))
    {
        connect(_Server, SIGNAL(newConnection()),this,SLOT(newConnection()));
        QMessageBox::information(this,"","сервер запущен");
        ui->statusConected->setProperty("state", 1);
        style()->polish(ui->statusConected);
    }
    else
    {
        QMessageBox::information(this,"","провал запуска сервера");
        ui->statusConected->setProperty("state", 0);
        style()->polish(ui->statusConected);
    }
}

void MainWindow::SendMesh()
{
    thread.quit(); // остановка потока

    ui->label->setText("генерация поля завершена начита отправка клиенту");
    // формирование  массива байтов
    QByteArray bytes;

    //создание потока данных
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    stream << _labirint.RecivedMesh(); // добавление данных о карте в поток данных

     foreach (QTcpSocket *socket, Client_Conection_List)
     {
          if(socket->socketDescriptor()== Info_Client.toInt())// проверка соотвествия клиента
          {
             socket->write(bytes); // отправка данных клиенту
          }

     }
      ui->label->setText(" отправка клиенту завершена");
}

void MainWindow::SendPath()
{
     ui->label->setText(" отправка решения лабиринта");

    decision _decision={countStep, _labirint.RecivedConst(), _labirint.RecivedPath()}; // создание экземпляра скруктуры

    QByteArray data=serializePerson(_decision); // создание массива байтов из данных скруктуры
    QByteArray block; //создание общего массива байтов

    //создание потока данных
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_5_15);
    out << (quint32)data.size(); // задание размера массива
    out.writeRawData(data.data(), data.size()); // запись данных


    foreach (QTcpSocket *socket, Client_Conection_List)
    {
        if(socket->socketDescriptor()== Info_Client.toInt())// проверка соотвествия клиента
        {
            socket->write(block);// отправка данных

        }

    }
  ui->label->setText(" отправка решения лабиринта завершена");
}

void MainWindow::Read_Data_Fron_Socket()
{
    QTcpSocket *socket=reinterpret_cast<QTcpSocket*>(sender());

     Info_Client=QString::number(socket->socketDescriptor());

// чтение данных с клиента
    QByteArray message_From_Server = socket->readAll();
    QString message="Client:" + QString::number(socket->socketDescriptor()) + " Send message::" + QString::fromStdString(message_From_Server.toStdString());

// отделение принятой информации
    QStringList Str=message.split("Send message::");
    QString str=Str[1];

//получение кординат ячеек старта и конца----------------------
    if (str.contains("cell", Qt::CaseInsensitive))
    {
        QStringList StrMessageCell;
        QStringList StrMessageCellRow;
        QStringList StrMessageCellColum;
        str.remove("cell", Qt::CaseInsensitive);

      // отслеживание метки start для стартовой ячейки
        if(str.contains("Start", Qt::CaseInsensitive))
        {
            str.remove("Start", Qt::CaseInsensitive);
            StrMessageCell=str.split("|");
            StrMessageCellRow=StrMessageCell[0].split(":");
            StrMessageCellColum=StrMessageCell[1].split(":");
            _labirint.SetPositionA(StrMessageCellRow[1].toInt(),StrMessageCellColum[1].toInt());
        }
        else
        { // отслеживание метки End для конечной ячейки
          str.remove("End", Qt::CaseInsensitive);
          StrMessageCell=str.split("|");
          StrMessageCellRow=StrMessageCell[0].split(":");
          StrMessageCellColum=StrMessageCell[1].split(":");
          countStep=_labirint.bfs( StrMessageCellRow[1].toInt(),StrMessageCellColum[1].toInt());
          emit CallSendPath();
        }
    }
//---------------------------------------
    else
    {
//получение размера карты лабиринта----------------------------------------

    QStringList StrMessageSize=Str[1].split("|");
    QStringList StrCountRow=StrMessageSize[0].split(":");
    QStringList StrCountColum=StrMessageSize[1].split(":");

    _labirint.SetSizeMaze(StrCountRow[1].toInt(),StrCountColum[1].toInt());


   ui->label->setText("начита генерация поля лабиринта");

    thread.start();// запуск потока
//-----------------------------------------------------------------------------------
    }
}

void MainWindow::newConnection()
{// создание нового  соединения
    while (_Server->hasPendingConnections())
    {
        Add_New_CLient_Conection(_Server->nextPendingConnection());
    }
}

void MainWindow::Add_New_CLient_Conection(QTcpSocket *socket)
{
    Client_Conection_List.append(socket);
    connect(socket,SIGNAL(readyRead()), this, SLOT(Read_Data_Fron_Socket()));//установление связи сигнала готовности чтения:  и слота:  Read_Data_From_Socket()
    Info_Client="Client:"+QString::number(socket->socketDescriptor())+ "подключился к серверу";
    ui->lstConsole->addItem(Info_Client);
}
