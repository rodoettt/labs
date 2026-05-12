#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>

class File {
private:
    QString filename;
    QString date;
    int size;

public:
    File(QString f, QString d, int s) : filename(f), date(d), size(s) {}

    QString getFilename() const { return filename; }
    QString getDate() const { return date; }
    int getSize() const { return size; }
};

class AddFileDialog;
class ImportWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QTableWidget* tableWidget;
    QPushButton* addButton;
    QPushButton* deleteButton;
    QPushButton* totalButton;
    QPushButton* importButton;
    QLabel* totalLabel;
    std::vector<File> files;

    void setupUI();
    void refreshTable();
    long long getTotalSize() const;
    void parseAndAddData(const QStringList& lines);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onAddFile();
    void onDeleteFile();
    void onShowTotal();
    void onImportData();
    void addFileToManager(const QString& filename, const QString& date, int size);
};

#endif // MAINWINDOW_H