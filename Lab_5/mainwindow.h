#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QTemporaryFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Search_triggered();

private:
    Ui::MainWindow *ui;
    int pageIndex;
    QTextEdit *editor;
    QTableWidget *tableWidget;
    QFont currentFont;
    QColor textColor;
    QColor backgroundColor;
    QString appDir = "Laboratory_5";
    bool tableModified = true;
    static QTemporaryFile tempFile;
};

#endif // MAINWINDOW_H