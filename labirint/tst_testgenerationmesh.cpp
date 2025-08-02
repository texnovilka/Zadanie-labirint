#include "labirint.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

using namespace testing;

labirint _labirint;
QVector<QVector<int>> staticmesh={{0,-1,0-1},{0,0,0,0},{-1,-1,-1,0},{-1,0,-1,0}};

int bfs( int finishRow, int finishColum)
{
    int const INF= 1e9;
    int h=staticmesh.size(), w= staticmesh[0].size();

    QVector<QVector<int>> dist (h, QVector<int>(w, INF));
    QVector<QVector<bool>> visited (h, QVector<bool>(w));
    QVector<QVector<QPair<int,int>>> PathLabirint(h, QVector<QPair<int,int>>(w,{-1,-1})); //  кординаты пути

    QQueue<QPair<int,int>> q;
    dist[0][0]=0;

    visited[0][0]=true;
    q.enqueue({0,0});

    QVector<int> drow= {0,-1,0,1}; // движение строки
    QVector<int> dcolum= {-1,0,1,0};// движение столбца


    while(!q.isEmpty())
    {
        auto[row, colum]=q.dequeue();

        for(int d=0; d<drow.size(); d++)
        {
            int trow= row+drow[d];
            int tcolum= colum+dcolum[d];

            if(0<=trow && trow<h && 0<=tcolum && tcolum<w && !visited[trow][tcolum] &&
                staticmesh[trow][tcolum]!=-1)
            {
                dist[trow][tcolum]=dist[row][colum]+1;

                PathLabirint[trow][tcolum]={row,colum};
                visited[trow][tcolum]=true;
                q.enqueue({trow,tcolum});
            }
        }
    }

    return dist[finishRow][finishColum];

}


TEST(TestsGenerationMesh, TestGenerationMesh)
{
    _labirint.SetLineMap(4,4);

    ASSERT_NE(_labirint.RecivedMesh(), staticmesh);
}

TEST(TestsGenerationPath, TestBFSNotEmpty)
{
    _labirint.SetPositionA(0,0);

    ASSERT_EQ( _labirint.bfs(0,0), 0);
}

TEST(TestsGenerationPath, TestBFSBorder)
{


    ASSERT_EQ( bfs(0,1), _labirint.RecivedConst());
}

TEST(TestsGenerationPath, TestBFSIsolationCell)
{


    ASSERT_EQ(bfs(3,1), _labirint.RecivedConst());
}

TEST(TestsGenerationPath, TestBFCountSteps)
{


    ASSERT_EQ(bfs(1,1), 2);
}

