#ifndef BASICMODE_H
#define BASICMODE_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include <QPoint>
#include "itemgrid.h"

namespace Ui {
class BasicMode;
}

class BasicMode : public QWidget
{
    Q_OBJECT

public:
    explicit BasicMode(QWidget *parent = nullptr);
    ~BasicMode();

    void changeToThis();
    void getTransparentPixmap();
    void reset();

    // 计时器控制
    void showTimer();
    void hideTimer();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onHintClicked();
    void onOptionsClicked();
    void onHelpClicked();
    void clearPath();
    void updateTimer();

private:
    bool isRowClear(int row, int col1, int col2);
    bool isColClear(int col, int row1, int row2);
    bool isStraightClear(int row1, int col1, int row2, int col2);
    bool isOneCornerClear(int row1, int col1, int row2, int col2, QVector<QPoint>& path);
    bool isTwoCornerClear(int row1, int col1, int row2, int col2, QVector<QPoint>& path);
    bool canConnect(int row1, int col1, int row2, int col2, QVector<QPoint>& path);
    void eliminate(int row1, int col1, int row2, int col2, const QVector<QPoint>& path);
    void tryHint();

    void startTimer();
    void stopTimer();
    void resetTimer();
    QString formatTime(int seconds);

    Ui::BasicMode *ui;
    std::vector<QPixmap> texture;
    ItemGrid* itemGrid;

    bool hasSelected;
    int selectedRow;
    int selectedCol;

    QVector<QPoint> currentPath;
    QTimer* pathTimer;

    QTimer* gameTimer;
    int elapsedSeconds;
    bool isTiming;
    bool timerVisible;
};

#endif // BASICMODE_H