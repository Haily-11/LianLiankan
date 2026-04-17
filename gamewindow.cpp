#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "basicmode.h"
#include "startmenu.h"
#include <QMessageBox>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow),
    startMenu(new StartMenu(this)),
    basicMode(nullptr),
    leisureMode(nullptr),
    levelMode(nullptr),
    timerEnabled(false)
{
    ui->setupUi(this);
    startMenu->changeToThis();
    startMenu->raise();
}

GameWindow::~GameWindow()
{
    delete ui;
    delete startMenu;
    if(basicMode) delete basicMode;
    if(leisureMode) delete leisureMode;
    if(levelMode) delete levelMode;
}

void GameWindow::paintBackground(QString feage){
    QPalette palette;
    palette.setBrush(QPalette::Window,QBrush(QPixmap(feage)));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
}

void GameWindow::hideAll(){
    startMenu->hide();
    if(basicMode) basicMode->hide();
    if(leisureMode) leisureMode->hide();
    if(levelMode) levelMode->hide();
}

void GameWindow::onBtnBasicModeClicked(){
    hideAll();
    if(!basicMode){
        basicMode = new BasicMode(this);
    }
    basicMode->changeToThis();
}

void GameWindow::onBtnLeisureModeClicked(){
    hideAll();
    if(!leisureMode){
        leisureMode = new BasicMode(this);
    }
    leisureMode->changeToThis();
    QMessageBox::information(this, "休闲模式", "休闲模式已开启！\n\n提示次数无限制，适合新手练习。");
}

void GameWindow::onBtnLevelModeClicked(){
    hideAll();
    if(!levelMode){
        levelMode = new BasicMode(this);
    }
    levelMode->changeToThis();
    QMessageBox::information(this, "闯关模式", "闯关模式开发中...\n\n即将推出！");
}

void GameWindow::onBtnRestartClicked(){
    if(basicMode && basicMode->isVisible()){
        basicMode->reset();
    } else if(leisureMode && leisureMode->isVisible()){
        leisureMode->reset();
    } else if(levelMode && levelMode->isVisible()){
        levelMode->reset();
    }
}

void GameWindow::onBtnBackToMenuClicked(){
    hideAll();
    startMenu->changeToThis();
}