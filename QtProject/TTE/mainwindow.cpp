#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Создание виджетов
    textEditor = new QTextEdit(this);
    tableWidget = new QTableWidget(5, 3, this);  // 5 строк и 3 колонки
    tabWidget = new QTabWidget(this);

    // Добавление вкладок
    tabWidget->addTab(textEditor, "Текстовый редактор");
    tabWidget->addTab(tableWidget, "Редактор таблиц");

    // Установка главного виджета
    setCentralWidget(tabWidget);

    // Настройка меню
    setupMenu();
}

void MainWindow::setupMenu() {
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    QAction *exitAction = new QAction("Выход", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
}
