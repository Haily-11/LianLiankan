#ifndef STARTMENU_H
#define STARTMENU_H

#include <QWidget>

namespace Ui {
class StartMenu;
}

class StartMenu : public QWidget
{
    Q_OBJECT

public:
    explicit StartMenu(QWidget *parent = nullptr);
    ~StartMenu();

    void changeToThis();

private slots:
    void onBasicClicked();
    void onLeisureClicked();
    void onLevelClicked();
    void onRankClicked();
    void onSettingsClicked();
    void onHelpClicked();

private:
    Ui::StartMenu *ui;
};

#endif // STARTMENU_H