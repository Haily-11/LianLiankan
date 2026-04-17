#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>

class StartMenu;
class BasicMode;

QT_BEGIN_NAMESPACE
namespace Ui {
class GameWindow;
}
QT_END_NAMESPACE

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow() override;

    void hideAll();
    void paintBackground(QString feage);

    bool isTimerEnabled() const { return timerEnabled; }
    void setTimerEnabled(bool enabled) { timerEnabled = enabled; }

public slots:
    void onBtnBasicModeClicked();      // 基本模式
    void onBtnLeisureModeClicked();    // 休闲模式
    void onBtnLevelModeClicked();      // 闯关模式
    void onBtnRestartClicked();        // 重新开始
    void onBtnBackToMenuClicked();     // 返回主菜单

private:
    Ui::GameWindow *ui;
    StartMenu* startMenu;
    BasicMode* basicMode;
    BasicMode* leisureMode;    // 休闲模式
    BasicMode* levelMode;      // 闯关模式
    bool timerEnabled;
};

#endif // GAMEWINDOW_H