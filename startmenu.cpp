#include "startmenu.h"
#include "gamewindow.h"
#include "ui_startmenu.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QCheckBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

StartMenu::StartMenu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StartMenu)
{
    ui->setupUi(this);

    connect(ui->BtnBasic, &QPushButton::clicked, this, &StartMenu::onBasicClicked);
    connect(ui->BtnLeisure, &QPushButton::clicked, this, &StartMenu::onLeisureClicked);
    connect(ui->BtnLevel, &QPushButton::clicked, this, &StartMenu::onLevelClicked);
    connect(ui->BtnRank, &QPushButton::clicked, this, &StartMenu::onRankClicked);
    connect(ui->BtnSettings, &QPushButton::clicked, this, &StartMenu::onSettingsClicked);
    connect(ui->BtnHelp, &QPushButton::clicked, this, &StartMenu::onHelpClicked);
}

StartMenu::~StartMenu()
{
    delete ui;
}

void StartMenu::changeToThis(){
    static_cast<GameWindow*>(parent())->setWindowTitle("欢乐连连看");
    static_cast<GameWindow*>(parent())->paintBackground(":/img/res/llk_main.bmp");
    this->show();
}

void StartMenu::onBasicClicked()
{
    static_cast<GameWindow*>(parent())->onBtnBasicModeClicked();
}

void StartMenu::onLeisureClicked()
{
    static_cast<GameWindow*>(parent())->onBtnLeisureModeClicked();
}

void StartMenu::onLevelClicked()
{
    static_cast<GameWindow*>(parent())->onBtnLevelModeClicked();
}

void StartMenu::onRankClicked()
{
    QFile file("rankings.txt");
    QString rankText = "═══════════ 排行榜 ═══════════\n\n";

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int rank = 1;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(",");
            if (parts.size() >= 2) {
                int seconds = parts[0].toInt();
                int minutes = seconds / 60;
                int secs = seconds % 60;
                QString timeStr = QString("%1分%2秒").arg(minutes).arg(secs, 2, 10, QChar('0'));
                rankText += QString("第 %1 名: %2\n").arg(rank++).arg(timeStr);
            }
        }
        file.close();
    } else {
        rankText += "暂无记录\n";
    }

    rankText += "\n完成游戏即可上榜！";
    QMessageBox::information(this, "排行榜", rankText);
}

void StartMenu::onSettingsClicked()
{
    QDialog* settingsDlg = new QDialog(this);
    settingsDlg->setWindowTitle("设置");
    settingsDlg->setFixedSize(300, 180);
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

    saveBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; padding: 5px 20px; }");
    cancelBtn->setStyleSheet("QPushButton { background-color: #f44336; color: white; border-radius: 5px; padding: 5px 20px; }");

    btnLayout->addStretch();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    connect(saveBtn, &QPushButton::clicked, [this, timerCheckBox, settingsDlg](){
        static_cast<GameWindow*>(parent())->setTimerEnabled(timerCheckBox->isChecked());
        settingsDlg->accept();
        QMessageBox::information(this, "设置", "设置已保存！");
    });

    connect(cancelBtn, &QPushButton::clicked, settingsDlg, &QDialog::reject);

    settingsDlg->exec();
    delete settingsDlg;
}

void StartMenu::onHelpClicked()
{
    QString helpText =
        "═══════════ 欢乐连连看 ═══════════\n\n"
        "【游戏规则】\n"
        "• 点击两个相同花色的图片\n"
        "• 路径最多只能拐两个弯\n"
        "• 路径上不能有其他图片阻挡\n\n"
        "【游戏模式】\n"
        "• 基本模式：标准连连看玩法\n"
        "• 休闲模式：无限提示，轻松游玩\n"
        "• 闯关模式：逐步增加难度\n\n"
        "【操作说明】\n"
        "• 鼠标点击选择图片\n"
        "• 使用「提示」按钮获取帮助\n\n"
        "祝您游戏愉快！";

    QMessageBox::information(this, "帮助", helpText);
}