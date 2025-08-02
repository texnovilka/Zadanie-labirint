#include "Labirint.h"

Labirint::Labirint(QObject *parent) {}

int Labirint::bfs(int finishRow, int finishColum)
{
    CompleatPathLabirint.clear(); // предварительное очищение  вектора пар кординат ячеек
    int h= MeshMap.size(), w= MeshMap[0].size(); // высота и ширина карты

    QVector<QVector<int>> dist (h, QVector<int>(w, INF));// массив пройденных ячеек
    QVector<QVector<bool>> visited (h, QVector<bool>(w));// массив посещеных ячеек
    QVector<QVector<QPair<int,int>>> PathLabirint(h, QVector<QPair<int,int>>(w,{-1,-1})); //  кординаты пути

    QQueue<QPair<int,int>> q;
    dist[PositionA.first][PositionA.second]=0;

    visited[PositionA.first][PositionA.second]=true;
    q.enqueue({PositionA.first,PositionA.second}); // добавление стартовой ячейки в очередь

    QVector<int> drow= {0,-1,0,1}; // движение строки
    QVector<int> dcolum= {-1,0,1,0};// движение столбца


    while(!q.isEmpty())
    {
        auto[row, colum]=q.dequeue(); // извленчение кординат ячейки

        for(int d=0; d<drow.size(); d++)
        {
            int trow= row+drow[d];
            int tcolum= colum+dcolum[d];

            if(0<=trow && trow<h && 0<=tcolum && tcolum<w && !visited[trow][tcolum] &&
                MeshMap[trow][tcolum]!=-1)
            {
                dist[trow][tcolum]=dist[row][colum]+1; // добавление одного шага

                PathLabirint[trow][tcolum]={row,colum}; // добавление ячейки в решение
                visited[trow][tcolum]=true;
                q.enqueue({trow,tcolum});// добавление следющей ячейки в очередь
            }
        }
    }
    // востановление пути
    if (dist[finishRow][finishColum]!=INF) // востановление можно начать если  путь существует
    {
        int row=finishRow;
        int colum=finishColum;

        while(row!=-1 && colum !=-1)  // обходим PathLabirint в обратном пути
        {
            CompleatPathLabirint.append({row, colum});
            auto[prow, pcolum]=PathLabirint[row][colum];
            row=prow;
            colum=pcolum;

        }
    }

    return dist[finishRow][finishColum];
}

void Labirint::SetLineMap(int row, int colum)
{
    MeshMap.clear();
    QVector<QVector<int>> k;
    for (int i=0; i<row; i++)
    {
        QVector<int> line;
        for(int j=0; j<colum; j++)
        {
            int ValueCell=QRandomGenerator::global()->bounded(-1, 1); // генерация произвольным образовм двух состояний ячейки: -1 стена, 0  проходимая ячейка
            line.append(ValueCell);
        }

        MeshMap.append(line);
    }
}

QVector<QPair<int, int> > Labirint::RecivedPath()
{
  return CompleatPathLabirint;
}

int Labirint::RecivedConst()
{
    return INF;
}

QVector<QVector<int> > Labirint::RecivedMesh()
{
    return MeshMap;
}


void Labirint::SetPositionA(int a, int b)
{
    PositionA.first=a;
    PositionA.second=b;
}

void Labirint::SetSizeMaze(int row, int colum)
{
    CountRow=row;
    CountColum=colum;
}

void Labirint::run()
{
    SetLineMap(CountRow, CountColum);
    emit finished();
}
