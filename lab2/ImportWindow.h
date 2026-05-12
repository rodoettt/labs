#ifndef IMPORTWINDOW_H
#define IMPORTWINDOW_H

#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ImportWindow : public QDialog {
    Q_OBJECT

private:
    QPushButton* openFileButton;
    QPushButton* processButton;
    QTextEdit* contentEdit;
    QString currentFileContent;

    void setupUI();

public:
    ImportWindow(QWidget* parent = nullptr);

signals:
    void dataProcessed(const QStringList& lines);

private slots:
    void onOpenFile();
    void onProcessData();
};

#endif // IMPORTWINDOW_H