#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include "../model/FileManagerModel.h"

class AddFileDialog;
class ImportWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    FileManagerModel* model;
    QTableWidget* tableWidget;
    QTextEdit* logWidget;
    QPushButton* addButton;
    QPushButton* deleteButton;
    QPushButton* totalButton;
    QPushButton* importButton;
    QPushButton* showLogButton;
    QLabel* totalLabel;
    QLabel* countLabel;

    void setupUI();
    void refreshTable();
    void updateStats();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onAddFile();
    void onDeleteFile();
    void onShowTotal();
    void onImportData();
    void onShowLog();
    void onModelDataChanged();
    void onModelError(const QString& error);
};

#endif // MAINWINDOW_H