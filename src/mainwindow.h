#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Test_triggered();

    void on_action_PasteArticle_triggered();

    void on_action_AD_triggered(bool checked);

    void on_action_Jo_To_Num_triggered(bool checked);

private:
    Ui::MainWindow *ui;

    std::wstring modifyLawText(const std::wstring& text);
    void doPaste(const QString& originalText);
};
#endif // MAINWINDOW_H
