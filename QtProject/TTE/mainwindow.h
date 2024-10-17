#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QMenuBar>
#include <QAction>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    QTextEdit *textEditor;
    QTableWidget *tableWidget;
    QTabWidget *tabWidget;
    void setupMenu();
};

#endif // MAINWINDOW_H
