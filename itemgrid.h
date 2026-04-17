#ifndef ITEMGRID_H
#define ITEMGRID_H

#include <qpoint.h>

// 常量定义放在这里，确保所有文件都能访问
const int COL_OFFSET = 30;
const int ROW_OFFSET = 30;
const int ROWS = 10;        // 行数
const int COLS = 16;        // 列数

class GameWindow;
class BasicMode;

struct ItemPos{
    int col;
    int row;
    ItemPos(int _col, int _row): col(_col), row(_row) {}
    explicit ItemPos(const QPoint& point): col((point.x()-COL_OFFSET)/40), row((point.y()-ROW_OFFSET)/40) {}
    bool operator==(const ItemPos& obj) const {
        return col==obj.col && row==obj.row;
    }
};

class ItemGrid
{
    friend class BasicMode;
public:
    ItemGrid(BasicMode* _parent);
    ~ItemGrid();
    void reset();

    char getCell(int row, int col) const { return grid[row][col]; }
    void setCell(int row, int col, char value) { grid[row][col] = value; }
    int getRemaining() const { return remaining; }
    void decreaseRemaining() { remaining--; }

private:
    BasicMode* parent;
    GameWindow* topParent;
    char grid[ROWS][COLS]{};
    int remaining;
};

#endif // ITEMGRID_H