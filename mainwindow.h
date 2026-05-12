#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include "../model/file_model.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void ProcessMultipleLines(const QString& data);

private slots:
    void OnParseButtonClicked();
    void OnClearButtonClicked();
    void OnHelpButtonClicked();
    void OnLoadFileClicked();
    void OnShowLogsClicked();
    void OnLoadCommandsClicked();

private:
    void SetupUi();
    void UpdateDisplay(const FileInfo& file);
    void ShowError(const QString& error_message);
    void ClearDisplay();
    void UpdateStatusBar();
    void ProcessCommandFile(const QString& filename);
    void DisplayCommandResults(const QList<FileModel::CommandResult>& results);
    void SaveResultsToFile(const QList<FileModel::CommandResult>& results, const QString& filepath);

    FileModel* model_;

    QTextEdit* input_text_edit_;
    QTextEdit* result_line_edit_;
    QPushButton* parse_button_;
    QPushButton* clear_button_;
    QPushButton* load_file_button_;
    QPushButton* load_commands_button_;
    QPushButton* show_logs_button_;
    QLabel* status_label_;
};

#endif