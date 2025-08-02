#ifndef LABIRINT_H
#define LABIRINT_H


#include <QVector>
#include <QRandomGenerator>
#include <QQueue>
#include <QThread>
#include <QDebug>

/**
 * @brief класс лабиринта
 */
class labirint : public QObject
{

Q_OBJECT

private:
const  int  INF=1e9; //  число шагов которого явно не окажется для сравнения с реальным количеством шгаов
QVector<QPair<int,int>>CompleatPathLabirint; // вектор который хранит произвольное количество пар чисел кординат ячейки
QVector<QVector<int>> MeshMap; //вектор хранящий массив размером CountRowХCountColum ячеек поля лабиринта
int startPositionRow=0; // строка стартовой ячейки
int startPositionCollum=0; //столбец стартовой ячейки
int CountRow; //количество строк
int CountColum; // количество столбцов
QPair<int,int> PositionA; // пара кординат стартовой ячейки


public:

/**
     * @brief labirint
     * @param parent
     */

    explicit labirint(QObject* parent=0);

    /**
     * @brief   bfs: метод создающий кратчайщий путь решения лабиринта и количество шагов между стартовой и конечной ячейкой (поиск в ширину)
     * @param  finishRow: строка конечной ячейки
     * @param  finishColum: столбец конечной ячейки
     * @return int
     */

    int bfs(int finishRow, int finishColum);

    /**
     * @brief  SetLineMap: метод создающий карту
     * @param  row: количество строк
     * @param  colum: количество столбцов
     */

     void SetLineMap (int row, int colum);

     /**
     * @brief  RecivedPath: возвращает путь решения лабиринта
     * @return  QVector<QPair<int,int>>
     */

    QVector<QPair<int,int>> RecivedPath();

    /**
     * @brief   RecivedConst: возвращает константу INF
     * @return int
     */

    int RecivedConst();

    /**
     * @brief метод RecivedMesh: возвращает карту лабиринта
     * @return  QVector<QVector<int>>
     */

    QVector<QVector<int>>RecivedMesh();

    /**
     * @brief  метод SetPositionA: заполняет пару кодинат стартовой ячейки
     * @param  a: номер строки
     * @param   b: номер столбца
     */

    void SetPositionA(int a, int b);
    /**
     * @brief  SetSizeMaze: заполнени размеров лабиринта
     * @param   row: количество строк
     * @param  colum: количество столбцов
     */

    void SetSizeMaze(int row, int colum);


public slots:
    /**
     * @brief  слот run: вызывается при создание нового потока
     */
    void run();


signals:
    /**
     * @brief сигнал finished: вызывается в слоте run по завершению всех операций
     */
    void finished();
};

#endif // LABIRINT_H
