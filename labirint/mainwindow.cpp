#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)     : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


   ui->setupUi(this);
    settings.beginGroup("state");
    restoreState(settings.value("WindowState").toByteArray());// востановление последнего положения окна
    settings.endGroup();

    settings.beginGroup("size");
    restoreGeometry(settings.value("WindowGeometry", geometry()).toByteArray());// востановление последних размеров окна
    settings.endGroup();



    QRegExp rx("[1-9]\\d{0,3}"); // регулярное выражение для валидации тесктовых полей: ввод только цифр и без нуля на первой позиции
    QValidator *roll = new QRegExpValidator(rx, this);

   ui->txtCountColum->setValidator(roll);
    ui->txtCountRow->setValidator(roll);

    connect(&thread, &QThread::started,& _labirint, &labirint::run); // установление связи сигнала: started и слота:  run
    connect(&_labirint, &labirint::finished, this, &MainWindow::Recived); // установление связи сигнала: finished и слота:  Recived
    _labirint.moveToThread(&thread); // перемещение экземпляра класа labirint в новый поток

    TCPSocket= new QTcpSocket(); //
    connect(TCPSocket, SIGNAL(readyRead()), this ,SLOT(Read_Data_From_Socket()));// установление связи сигнала готовности чтения:  и слота:  Read_Data_From_Socket()
    connect(TCPSocket, &QTcpSocket::errorOccurred, this, &MainWindow::StatusConnected);// установление связи сигнала вызова ошибки: finished и слота: StatusConnected

}

MainWindow::~MainWindow()
{   settings.beginGroup("size");
    settings.setValue("WindowGeometry", saveGeometry());// сохранение последних размеров окна
    settings.endGroup();

    settings.beginGroup("state");
    settings.setValue("WindowState", saveState()); //сохранение последнего положения окна
    settings.endGroup();

    delete ui;
}
/**
 * @brief decision: скруктура для приема решения пути по сети
 */
struct decision
{
    /**
     * @brief Count: количество шагов
     */
    int Count;
    /**
     * @brief _INF_: константа
     */
    int  _INF_;
    /**
     * @brief path: решение пути
     */
    QVector<QPair<int,int>> path;
};

/**
 * @brief deserializePerson: метод производит десериализацию данных принятых по сети
 * @param data: принятые данные
 * @return decision
 */
decision deserializePerson(const QByteArray& data)
{
    decision _decision;
    QDataStream stream(data);

    stream >> _decision.Count >>_decision._INF_>>_decision.path;

    return _decision;
}


// проверка текстового поля для количества строк на правильность ввода на клиенте.
void MainWindow::on_txtCountColum_textChanged(const QString &arg1)
{
    if (ui->txtCountColum->text()==NULL || ui->txtCountRow->text()==NULL)
    {
        ui->PushGeneration->setEnabled(false);
        ui->LabelMesegeError->setText( "не коректное значение ширины или высоты");
        ui->LabelMesegeError->setStyleSheet("color: rgb(255,0,0)");
    }
    else
    {
        ui->PushGeneration->setEnabled(true);
        ui->LabelMesegeError->setText("");
        ui->LabelMesegeError->setStyleSheet("color: rgb(0,0,0)");
    }
}
//-------------------------------------------------------------------

// проверка текстового поля количества столбцов на правильность ввода на клиенте.
void MainWindow::on_txtCountRow_textChanged(const QString &arg1)
{

    if (ui->txtCountColum->text()==NULL || ui->txtCountRow->text()==NULL)
    {
        ui->PushGeneration->setEnabled(false);
        ui->LabelMesegeError->setText( "не коректное значение ширины или высоты");
        ui->LabelMesegeError->setStyleSheet("color: rgb(255,0,0)");
    }
    else
    {
        ui->PushGeneration->setEnabled(true);
        ui->LabelMesegeError->setText("");
        ui->LabelMesegeError->setStyleSheet("color: rgb(0,0,0)");
    }
}
//-------------------------------------------------------------------

void MainWindow::on_PushGeneration_clicked()
{
    // сброс флагов
    flagSetingAPosition=false;
    ReciveMeshOrPath=false;

    ui->PushGeneration->setEnabled(false);
    ui->ScenaMap->setEnabled(false);

    // случай если работать с сервером
    if(StatusMode==true)
    {
        if (ui->txtCountRow->text().toInt()>127 ||+ui->txtCountColum->text().toInt()>127)
        {
             QMessageBox::information(this,"","превышен размер пакета для отправки по сети");
            ui->PushGeneration->setEnabled(true);
            ui->ScenaMap->setEnabled(true);
        }
        else
        {
        // отправка на сервер
         if(TCPSocket)
         {
            if (TCPSocket->isOpen())
            {
                QString WriteData="количество строк:"+ui->txtCountRow->text()+"|"+"количество столбцов:"+ui->txtCountColum->text();
                TCPSocket->write(WriteData.toStdString().c_str());

            }
            else
            {
                QMessageBox::information(this,"","ошибка отправления:"+TCPSocket->errorString());
            }
         }
         else
         {
            QMessageBox::information(this,"","ошибка соединения:"+TCPSocket->errorString());
         }
        }
    }
    // случай если работать на клиенте
    else
    {
    _labirint.SetSizeMaze(ui->txtCountRow->text().toInt(),ui->txtCountColum->text().toInt());

    ui->LabelMesegeError->setText("идет генерация поля");
    thread.start(); // запуск потока
    }
}
//-------------------------------------------------------------------

void MainWindow::on_ClientMode_triggered()
{
   ui->ConectedToServer->setEnabled(false);
   StatusMode=false;
   if (TCPSocket->isOpen())
   {
       TCPSocket->close();
       QMessageBox::information(this,"","соединение прервано");
   }
    ui->ServerMode->setEnabled(true);
}
//-------------------------------------------------------------------

void MainWindow::on_ServerMode_triggered()
{

   ui->ServerMode->setEnabled(false);
   ui->ConectedToServer->setEnabled(true);
}
//-------------------------------------------------------------------

void MainWindow::on_ScenaMap_cellClicked(int row, int column)
{

    ui->LabelMesegeError->setText("");// чистка метки

    ui->ScenaMap->clearSelection(); // чистка таблицы
    QTableWidgetItem *t = new QTableWidgetItem("");
    ui->ScenaMap->setItem(row,column,t);

// обработка решения пути при на клиенте
    if (StatusMode==false)
    {
      QVector line=_labirint.RecivedMesh()[row];

      if (flagSetingAPosition)
      {
       ui->ScenaMap->item(row,column)->setBackground(Qt::red);

        int stepCount=_labirint.bfs(row,column);

      // отрисовка решения лабиринта
        if(stepCount!= _labirint.RecivedConst())
        {
           for( int i=1; i<_labirint.RecivedPath().size()-1;i++)
            {
               QPair<int,int> cordinate=_labirint.RecivedPath()[i];
                ui->ScenaMap->item(cordinate.first,cordinate.second)->setBackground(Qt::blue);
            }

        }
        else
        {// обработка при не нахождении пути
           QMessageBox::information(this,"","путь не найден");
           ui->LabelMesegeError->setText("бесконечность");
           if(line[column]==-1) ui->ScenaMap->item(row,column)->setBackground(Qt::gray);
           else ui->ScenaMap->item(row,column)->setBackground(Qt::white);
        }
      }
      else
      {
         // обработка стартовой ячейки
        if(line[column]!=-1)
        {
            _labirint.SetPositionA(row,column);
            ui->ScenaMap->item(row,column)->setBackground(Qt::green);
            flagSetingAPosition=true ;
        }

        else
        {
            // обработка стартовой ячейки при попадании на стену
            QMessageBox::information(this,""," начало пути на стене");
            ui->LabelMesegeError->setText("бесконечность");
            ui->ScenaMap->item(row,column)->setBackground(Qt::gray);
        }
      }
    }
// обработка решения пути при составления решения на сервере
    else
    {
        QVector line=mesh[row];

        if (flagSetingAPosition)
        {
            if(line[column]==-1) ui->ScenaMap->item(row,column)->setBackground(Qt::gray);

            else ui->ScenaMap->item(row,column)->setBackground(Qt::white);
            //------------------------------------------------------------------------
             ui->ScenaMap->item(row,column)->setBackground(Qt::red);
            RowCell=row;
             ColumCell=column;
            if(TCPSocket)
            {
                if (TCPSocket->isOpen())
                {
                    // отправка кординат конечной ячейки
                    ReciveMeshOrPath=true;
                    QString WriteDataTwo=QString("cell")+QString("Endcell")+QString("строка:")+QString::number(row)+"|"+"столбец:"+QString::number(column);
                    TCPSocket->write(WriteDataTwo.toStdString().c_str());
                }
                else
                {
                    QMessageBox::information(this,"","ошибка отправления:"+TCPSocket->errorString());
                }
            }
            else
            {
                QMessageBox::information(this,"","ошибка соединения:"+TCPSocket->errorString());
            }

        }
        else
        { // обработка стартовой ячейки
            if(line[column]!=-1)
            {
                if(TCPSocket)
                {
                    if (TCPSocket->isOpen())
                    {// отправка кординат стартовой ячейки
                        QString WriteDataTwo=QString("cell")+QString("Startcell")+QString("строка:")+QString::number(row)+"|"+"столбец:"+QString::number(column);
                        TCPSocket->write(WriteDataTwo.toStdString().c_str());

                    }
                    else
                    {
                        QMessageBox::information(this,"","ошибка отправления:"+TCPSocket->errorString());
                    }
                }
                else
                {
                    QMessageBox::information(this,"","ошибка соединения:"+TCPSocket->errorString());
                }
                ui->ScenaMap->item(row,column)->setBackground(Qt::green);
                flagSetingAPosition=true ;
            }
            else
            {
                 // обработка стартовой ячейки при попадании на стену
                QMessageBox::information(this,""," начало пути на стене");
                ui->LabelMesegeError->setText("бесконечность");
                ui->ScenaMap->item(row,column)->setBackground(Qt::gray);
            }
        }

    }
}
//__________________________________________________________________________________________________________________

void MainWindow::Recived()
{

    thread.quit(); // остановка потока
    ui->LabelMesegeError->setText("генерация завершена");
    ui->PushGeneration->setEnabled(true);
    ui->ScenaMap->setEnabled(true);

    ui->ScenaMap->clearSelection();
    ui->ScenaMap->setRowCount(ui->txtCountRow->text().toInt());
    ui->ScenaMap->setColumnCount(ui->txtCountColum->text().toInt());

    // отрисовка карты лабиринта
    for (int i = 0;i < _labirint.RecivedMesh().size(); i++)
    {
        QVector line=_labirint.RecivedMesh()[i];
        for (int j = 0; j <line.size(); j++)
        {
            QTableWidgetItem *t = new QTableWidgetItem("");
            ui->ScenaMap->setItem(i,j,t);
            if (line[j]==-1)
            {
                ui->ScenaMap->item(i,j)->setBackground(Qt::gray);
                ui->ScenaMap->item(i,j)->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
            }
        }
    }

}


// сетевая часть клиета------------------------------------------------------------------------------------------
void MainWindow::Read_Data_From_Socket()
{
    if(TCPSocket)
    {
        if (TCPSocket->isOpen())
        {
//прием массива и отрисовка сетки-----------------------------------------------------------------------------------
            // создание потока данных
            QDataStream read(TCPSocket);
           read.setVersion(QDataStream::Qt_5_15);

           if(ReciveMeshOrPath==false)
           {
             read >> mesh; // чтение данных карты лабиринта
               if(!mesh.isEmpty())
                {
                 QMessageBox::information(this,"","карта успешно прислана");
                   ui->PushGeneration->setEnabled(true);
                   ui->ScenaMap->setEnabled(true);

                   ui->ScenaMap->setRowCount(ui->txtCountRow->text().toInt());
                   ui->ScenaMap->setColumnCount(ui->txtCountColum->text().toInt());
                   // отрисовка карты
                   for (int i = 0;i < mesh.size(); i++)
                   {
                       QVector line=mesh[i];
                       for (int j = 0; j <line.size(); j++)
                       {
                           QTableWidgetItem *t = new QTableWidgetItem("");
                           ui->ScenaMap->setItem(i,j,t);
                           if (line[j]==-1)
                           {
                               ui->ScenaMap->item(i,j)->setBackground(Qt::gray);
                               ui->ScenaMap->item(i,j)->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
                           }
                       }
                   }
                }
               else
               {
                   QMessageBox::information(this,"","карта не получена");
               }
            }
//----------------------------------------------------------------------------------------------------------------------
            else
            {
               // проверка на доступность чтения
              if (TCPSocket->bytesAvailable() < (int)sizeof(quint32)) return;
               quint32 blockSize;
               read >> blockSize;
               QByteArray data;

               data.resize(blockSize);// установка размера массива байтов
               read.readRawData(data.data(), blockSize); // чтение массива

                decision _decicion =deserializePerson(data);

                // отрисовка решения лабиринта
                if(!_decicion.path.isEmpty()||_decicion.Count==_decicion._INF_)
                {
                    QMessageBox::information(this,"","решение получено");
                    if(_decicion.Count!= _decicion._INF_)
                    {
                        for( int i=1; i<_decicion.path.size()-1;i++)
                        {
                            QPair<int,int> cordinate=_decicion.path[i];
                            ui->ScenaMap->item(cordinate.first,cordinate.second)->setBackground(Qt::blue);
                        }
                    }
                    else
                    {// обработка при не нахождении пути
                        QMessageBox::information(this,"","путь не найден");
                        ui->LabelMesegeError->setText("бесконечность");

                        if(mesh[RowCell][ColumCell]==-1) ui->ScenaMap->item(RowCell,ColumCell)->setBackground(Qt::gray);
                        else ui->ScenaMap->item(RowCell,ColumCell)->setBackground(Qt::white);
                    }
                }
                else
                {
                    QMessageBox::information(this,"","решение не получено");
                }
            }
        }
        else
        {
            QMessageBox::information(this,"","ошибка приема:"+TCPSocket->errorString());
        }
    }
    else
    {
        QMessageBox::information(this,"","ошибка соединения:"+TCPSocket->errorString());
    }
}

void MainWindow::StatusConnected(QAbstractSocket::SocketError)
{
     QMessageBox::information(this,"","соединение прервано или неустановлено");
    ui->ConectedToServer->setEnabled(true);
     StatusMode=false;
}

void MainWindow::on_ConectedToServer_clicked()
{
    StatusMode=true;


    TCPSocket->connectToHost(QHostAddress::LocalHost,8080);
    TCPSocket->open(QIODevice::ReadWrite);
    if (TCPSocket->isOpen())
    {
        ui->ConectedToServer->setEnabled(false);
        QMessageBox::information(this,"","идет соединение с сервером");
    }
    else QMessageBox::information(this,"","соединение с сервером неудачно");
 }

//__________________________________________________________________________________________________________________


