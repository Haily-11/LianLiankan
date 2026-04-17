#include "itemgrid.h"
#include "basicmode.h"
#include "gamewindow.h"
#include <random>
#include <algorithm>

ItemGrid::ItemGrid(BasicMode* _parent):
    parent(_parent),
    topParent(nullptr)
{
    reset();
}

ItemGrid::~ItemGrid()
{
}

void ItemGrid::reset(){
    // 初始化每个格子
    int index = 0;
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS; j++){
            grid[i][j] = (index / 16) % 10 + 1;
            index++;
        }
    }
    remaining = ROWS * COLS;

    // 随机打乱
    std::random_device rd;
    std::mt19937 g(rd());
    char* flatGrid = &grid[0][0];
    std::shuffle(flatGrid, flatGrid + ROWS * COLS, g);
}