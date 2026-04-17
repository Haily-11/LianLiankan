#include "basicmode.h"
#include "gamewindow.h"
#include "ui_basicmode.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPen>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QFrame>
#include <cmath>
#include <algorithm>

BasicMode::BasicMode(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BasicMode)
    , itemGrid(new ItemGrid(this))
    , hasSelected(false)
    , selectedRow(-1)
    , selectedCol(-1)
    , elapsedSeconds(0)
    , isTiming(false)
    , timerVisible(false)
{
    ui->setupUi(this);

    connect(ui->BtnRestart, &QPushButton::clicked, static_cast<GameWindow*>(parent),
            &GameWindow::onBtnRestartClicked);
    connect(ui->BtnBackToMenu, &QPushButton::clicked, static_cast<GameWindow*>(parent),
            &GameWindow::onBtnBackToMenuClicked);
    connect(ui->BtnHint, &QPushButton::clicked, this, &BasicMode::onHintClicked);
    connect(ui->BtnOptions, &QPushButton::clicked, this, &BasicMode::onOptionsClicked);
    connect(ui->BtnHelp, &QPushButton::clicked, this, &BasicMode::onHelpClicked);

    getTransparentPixmap();

    pathTimer = new QTimer(this);
    pathTimer->setSingleShot(true);
    connect(pathTimer, &QTimer::timeout, this, &BasicMode::clearPath);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &BasicMode::updateTimer);

    ui->labelTimer->hide();
}

BasicMode::~BasicMode()
{
    delete ui;
    if(itemGrid) delete itemGrid;
}

void BasicMode::changeToThis(){
    static_cast<GameWindow*>(parent())->setWindowTitle("欢乐连连看-基本模式");
    static_cast<GameWindow*>(parent())->paintBackground(":/img/res/fruit_bg.bmp");

    GameWindow* gameWin = static_cast<GameWindow*>(parent());
    if (gameWin && gameWin->isTimerEnabled()) {
        showTimer();
    } else {
        hideTimer();
    }

    reset();
    this->show();
}

void BasicMode::getTransparentPixmap(){
    QPixmap textureOrigin(":/img/res/fruit_element.bmp");
    QImage mask(":/img/res/fruit_mask.bmp");

    if(textureOrigin.size() != mask.size()){
        return;
    }

    mask.invertPixels();
    textureOrigin.setMask(QBitmap::fromImage(mask));

    for(int i = 0; i < 10; i++){
        texture.push_back(textureOrigin.copy(0, 40 * i, 40, 40));
    }
}

void BasicMode::reset(){
    if(itemGrid){
        itemGrid->reset();
    }
    hasSelected = false;
    selectedRow = -1;
    selectedCol = -1;
    currentPath.clear();
    pathTimer->stop();

    resetTimer();

    GameWindow* gameWin = static_cast<GameWindow*>(parent());
    if (gameWin && gameWin->isTimerEnabled() && timerVisible) {
        startTimer();
    }

    update();
}

void BasicMode::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    QRect area = event->rect();

    for(int row = 0; row < ROWS; row++){
        for(int col = 0; col < COLS; col++){
            QRect fruitRect(col * 40 + COL_OFFSET, row * 40 + ROW_OFFSET, 40, 40);
            if(!area.intersects(fruitRect)) continue;

            char type = itemGrid->getCell(row, col);
            if(type){
                painter.drawPixmap(fruitRect, texture[type - 1]);
            }
        }
    }

    if(hasSelected && selectedRow >= 0 && selectedCol >= 0){
        QRect selectedRect(selectedCol * 40 + COL_OFFSET, selectedRow * 40 + ROW_OFFSET, 40, 40);
        painter.setPen(QPen(Qt::red, 3));
        painter.drawRect(selectedRect);
    }

    if(!currentPath.isEmpty()){
        painter.setPen(QPen(Qt::green, 3));
        for(int i = 0; i < currentPath.size() - 1; i++){
            QPoint p1(currentPath[i].y() * 40 + COL_OFFSET + 20,
                      currentPath[i].x() * 40 + ROW_OFFSET + 20);
            QPoint p2(currentPath[i+1].y() * 40 + COL_OFFSET + 20,
                      currentPath[i+1].x() * 40 + ROW_OFFSET + 20);
            painter.drawLine(p1, p2);
        }
    }
}

void BasicMode::mousePressEvent(QMouseEvent* event){
    if(event->button() != Qt::LeftButton) return;

    int col = (event->pos().x() - COL_OFFSET) / 40;
    int row = (event->pos().y() - ROW_OFFSET) / 40;

    if(row < 0 || row >= ROWS || col < 0 || col >= COLS) return;

    char type = itemGrid->getCell(row, col);
    if(type == 0) return;

    if(!hasSelected){
        hasSelected = true;
        selectedRow = row;
        selectedCol = col;
        update();
        return;
    }

    if(selectedRow == row && selectedCol == col){
        hasSelected = false;
        selectedRow = -1;
        selectedCol = -1;
        update();
        return;
    }

    char type1 = itemGrid->getCell(selectedRow, selectedCol);
    char type2 = itemGrid->getCell(row, col);

    if(type1 != type2){
        hasSelected = true;
        selectedRow = row;
        selectedCol = col;
        update();
        return;
    }

    QVector<QPoint> path;
    if(canConnect(selectedRow, selectedCol, row, col, path)){
        eliminate(selectedRow, selectedCol, row, col, path);
        hasSelected = false;
        selectedRow = -1;
        selectedCol = -1;
        update();

        if(itemGrid->getRemaining() == 0){
            stopTimer();
            QMessageBox::information(this, "胜利", "恭喜你完成了游戏！");
        }
    } else {
        hasSelected = true;
        selectedRow = row;
        selectedCol = col;
        update();
    }
}

bool BasicMode::isRowClear(int row, int col1, int col2){
    if(col1 > col2) std::swap(col1, col2);
    for(int c = col1 + 1; c < col2; c++){
        if(itemGrid->getCell(row, c) != 0) return false;
    }
    return true;
}

bool BasicMode::isColClear(int col, int row1, int row2){
    if(row1 > row2) std::swap(row1, row2);
    for(int r = row1 + 1; r < row2; r++){
        if(itemGrid->getCell(r, col) != 0) return false;
    }
    return true;
}

bool BasicMode::isStraightClear(int row1, int col1, int row2, int col2){
    if(row1 == row2){
        return isRowClear(row1, col1, col2);
    }
    if(col1 == col2){
        return isColClear(col1, row1, row2);
    }
    return false;
}

bool BasicMode::isOneCornerClear(int row1, int col1, int row2, int col2, QVector<QPoint>& path){
    if(itemGrid->getCell(row1, col2) == 0){
        if(isRowClear(row1, col1, col2) && isColClear(col2, row1, row2)){
            path.clear();
            path.append(QPoint(row1, col1));
            path.append(QPoint(row1, col2));
            path.append(QPoint(row2, col2));
            return true;
        }
    }

    if(itemGrid->getCell(row2, col1) == 0){
        if(isColClear(col1, row1, row2) && isRowClear(row2, col1, col2)){
            path.clear();
            path.append(QPoint(row1, col1));
            path.append(QPoint(row2, col1));
            path.append(QPoint(row2, col2));
            return true;
        }
    }

    return false;
}

bool BasicMode::isTwoCornerClear(int row1, int col1, int row2, int col2, QVector<QPoint>& path){
    for(int c = 0; c < COLS; c++){
        if(c != col1 && c != col2){
            if(itemGrid->getCell(row1, c) == 0 && itemGrid->getCell(row2, c) == 0){
                if(isRowClear(row1, col1, c) && isColClear(c, row1, row2) && isRowClear(row2, c, col2)){
                    path.clear();
                    path.append(QPoint(row1, col1));
                    path.append(QPoint(row1, c));
                    path.append(QPoint(row2, c));
                    path.append(QPoint(row2, col2));
                    return true;
                }
            }
        }
    }

    for(int r = 0; r < ROWS; r++){
        if(r != row1 && r != row2){
            if(itemGrid->getCell(r, col1) == 0 && itemGrid->getCell(r, col2) == 0){
                if(isColClear(col1, row1, r) && isRowClear(r, col1, col2) && isColClear(col2, r, row2)){
                    path.clear();
                    path.append(QPoint(row1, col1));
                    path.append(QPoint(r, col1));
                    path.append(QPoint(r, col2));
                    path.append(QPoint(row2, col2));
                    return true;
                }
            }
        }
    }

    return false;
}

bool BasicMode::canConnect(int row1, int col1, int row2, int col2, QVector<QPoint>& path){
    if(row1 == row2 && col1 == col2) return false;

    if(isStraightClear(row1, col1, row2, col2)){
        path.clear();
        path.append(QPoint(row1, col1));
        path.append(QPoint(row2, col2));
        return true;
    }

    if(isOneCornerClear(row1, col1, row2, col2, path)){
        return true;
    }

    if(isTwoCornerClear(row1, col1, row2, col2, path)){
        return true;
    }

    return false;
}

void BasicMode::eliminate(int row1, int col1, int row2, int col2, const QVector<QPoint>& path){
    itemGrid->setCell(row1, col1, 0);
    itemGrid->setCell(row2, col2, 0);
    itemGrid->decreaseRemaining();
    itemGrid->decreaseRemaining();

    currentPath = path;
    update();
    pathTimer->start(300);
}

void BasicMode::clearPath(){
    currentPath.clear();
    update();
}

void BasicMode::onHintClicked(){
    tryHint();
}

void BasicMode::tryHint(){
    for(int row1 = 0; row1 < ROWS; row1++){
        for(int col1 = 0; col1 < COLS; col1++){
            char type1 = itemGrid->getCell(row1, col1);
            if(type1 == 0) continue;

            for(int row2 = 0; row2 < ROWS; row2++){
                for(int col2 = 0; col2 < COLS; col2++){
                    if(row1 == row2 && col1 == col2) continue;
                    char type2 = itemGrid->getCell(row2, col2);
                    if(type2 != type1) continue;

                    QVector<QPoint> path;
                    if(canConnect(row1, col1, row2, col2, path)){
                        hasSelected = true;
                        selectedRow = row1;
                        selectedCol = col1;
                        update();

                        QTimer::singleShot(500, this, [this, row1, col1, row2, col2, path](){
                            if(itemGrid->getCell(row1, col1) != 0 && itemGrid->getCell(row2, col2) != 0){
                                eliminate(row1, col1, row2, col2, path);
                                hasSelected = false;
                                selectedRow = -1;
                                selectedCol = -1;
                                update();

                                if(itemGrid->getRemaining() == 0){
                                    stopTimer();
                                    QMessageBox::information(this, "胜利", "恭喜你完成了游戏！");
                                }
                            }
                        });
                        return;
                    }
                }
            }
        }
    }

    QMessageBox::information(this, "提示", "当前没有可消除的图片，请重新开始游戏！");
}

void BasicMode::onOptionsClicked()
{
    QDialog* settingsDlg = new QDialog(this);
    settingsDlg->setWindowTitle("设置");
    settingsDlg->setFixedSize(300, 200);
    settingsDlg->setModal(true);

    QVBoxLayout* layout = new QVBoxLayout(settingsDlg);

    QLabel* titleLabel = new QLabel("游戏设置", settingsDlg);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QFrame* line = new QFrame(settingsDlg);
    line->setFrameShape(QFrame::HLine);
    layout->addWidget(line);

    QCheckBox* timerCheckBox = new QCheckBox("显示游戏计时器", settingsDlg);
    timerCheckBox->setChecked(static_cast<GameWindow*>(parent())->isTimerEnabled());
    layout->addWidget(timerCheckBox);

    QLabel* tipLabel = new QLabel("开启后，游戏界面将显示用时计时器", settingsDlg);
    tipLabel->setStyleSheet("color: #888; font-size: 12px; margin-left: 25px;");
    layout->addWidget(tipLabel);

    layout->addStretch();

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* saveBtn = new QPushButton("保存", settingsDlg);
    QPushButton* cancelBtn = new QPushButton("取消", settingsDlg);

    btnLayout->addStretch();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    connect(saveBtn, &QPushButton::clicked, [this, timerCheckBox, settingsDlg](){
        bool enabled = timerCheckBox->isChecked();
        static_cast<GameWindow*>(parent())->setTimerEnabled(enabled);

        if (enabled) {
            showTimer();
            if (itemGrid->getRemaining() > 0 && !isTiming) {
                startTimer();
            }
        } else {
            hideTimer();
        }

        settingsDlg->accept();
        QMessageBox::information(this, "设置", "设置已保存！");
    });

    connect(cancelBtn, &QPushButton::clicked, settingsDlg, &QDialog::reject);

    settingsDlg->exec();
    delete settingsDlg;
}

void BasicMode::onHelpClicked()
{
    QString helpText =
        "═══════════ 游戏规则 ═══════════\n\n"
        "1. 点击两个相同花色的图片\n"
        "2. 它们之间的路径最多只能拐两个弯\n"
        "3. 路径上不能有其他图片阻挡\n"
        "4. 满足以上条件即可成功消除！\n\n"
        "祝您游戏愉快！";

    QMessageBox::information(this, "帮助", helpText);
}

// ========== 计时器功能实现 ==========

void BasicMode::showTimer()
{
    timerVisible = true;
    ui->labelTimer->show();
    if (isTiming && !gameTimer->isActive()) {
        gameTimer->start(1000);
    }
}

void BasicMode::hideTimer()
{
    timerVisible = false;
    ui->labelTimer->hide();
    if (gameTimer->isActive()) {
        gameTimer->stop();
    }
}

void BasicMode::startTimer()
{
    if (!isTiming && timerVisible) {
        isTiming = true;
        gameTimer->start(1000);
    }
}

void BasicMode::stopTimer()
{
    if (isTiming) {
        isTiming = false;
        if (gameTimer->isActive()) {
            gameTimer->stop();
        }
    }
}

void BasicMode::resetTimer()
{
    stopTimer();
    elapsedSeconds = 0;
    ui->labelTimer->setText("时间: 00:00");
}

QString BasicMode::formatTime(int seconds)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("时间: %1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'));
}

void BasicMode::updateTimer()
{
    if (!isTiming) return;
    elapsedSeconds++;
    ui->labelTimer->setText(formatTime(elapsedSeconds));
}