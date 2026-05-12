#include "mainwindow.h"
#include "../model/parser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextBrowser>
#include <QDialog>
#include <QStatusBar>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , model_(new FileModel())
    , input_text_edit_(nullptr)
    , result_line_edit_(nullptr)
    , parse_button_(nullptr)
    , clear_button_(nullptr)
    , load_file_button_(nullptr)
    , load_commands_button_(nullptr)
    , show_logs_button_(nullptr)
    , status_label_(nullptr)
{
    SetupUi();
    setWindowTitle("Парсер информации о файлах");
    resize(600, 400);
    UpdateStatusBar();
}

MainWindow::~MainWindow() {
    delete model_;
}

void MainWindow::SetupUi() {
    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);

    QGroupBox* input_group = new QGroupBox("Ввод информации о файле");
    QVBoxLayout* input_layout = new QVBoxLayout(input_group);

    input_text_edit_ = new QTextEdit();
    input_text_edit_->setPlaceholderText(
        "Введите информацию о файле в формате:\n"
        "ИмяФайла ГГГГ.ММ.ДД РазмерВБайтах\n\n"
        "Примеры:\n"
        "document.txt 2024.12.25 1024\n"
        "report.pdf 2024.11.15 2048576\n"
        "image.png 2024.10.30 512000");
    input_text_edit_->setMaximumHeight(150);
    input_layout->addWidget(input_text_edit_);
    main_layout->addWidget(input_group);

    QGroupBox* result_group = new QGroupBox("Результат парсинга");
    QVBoxLayout* result_layout = new QVBoxLayout(result_group);

    result_line_edit_ = new QTextEdit();
    result_line_edit_->setReadOnly(true);
    result_layout->addWidget(result_line_edit_);
    main_layout->addWidget(result_group);

    QHBoxLayout* button_layout = new QHBoxLayout();

    load_file_button_ = new QPushButton("Загрузить файл");
    parse_button_ = new QPushButton("Разобрать");
    clear_button_ = new QPushButton("Очистить");
    show_logs_button_ = new QPushButton("Показать логи");
    load_commands_button_ = new QPushButton("Выполнить команды");
    QPushButton* help_button = new QPushButton("Справка");

    button_layout->addStretch();
    button_layout->addWidget(load_file_button_);
    button_layout->addWidget(parse_button_);
    button_layout->addWidget(clear_button_);
    button_layout->addWidget(show_logs_button_);
    button_layout->addWidget(load_commands_button_);
    button_layout->addWidget(help_button);
    button_layout->addStretch();

    main_layout->addLayout(button_layout);

    status_label_ = new QLabel();
    statusBar()->addWidget(status_label_);

    connect(parse_button_, &QPushButton::clicked, this, &MainWindow::OnParseButtonClicked);
    connect(clear_button_, &QPushButton::clicked, this, &MainWindow::OnClearButtonClicked);
    connect(load_file_button_, &QPushButton::clicked, this, &MainWindow::OnLoadFileClicked);
    connect(show_logs_button_, &QPushButton::clicked, this, &MainWindow::OnShowLogsClicked);
    connect(load_commands_button_, &QPushButton::clicked, this, &MainWindow::OnLoadCommandsClicked);
    connect(help_button, &QPushButton::clicked, this, &MainWindow::OnHelpButtonClicked);
}

void MainWindow::OnParseButtonClicked() {
    QString input_text = input_text_edit_->toPlainText().trimmed();

    if (input_text.isEmpty()) {
        ShowError("Пожалуйста, введите информацию о файле");
        return;
    }

    try {
        Parser parser(input_text);
        FileInfo file = parser.Parse();
        UpdateDisplay(file);
    } catch (const ParseException& e) {
        ShowError(e.getMessage());
    }
}

void MainWindow::OnLoadFileClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Текстовые файлы (*.txt)");

    if (!path.isEmpty()) {
        model_->LoadFromFile(path);

        QStringList files = model_->GetFilesStringList();
        if (files.isEmpty()) {
            result_line_edit_->setPlainText("Файл загружен, но не содержит валидных записей о файлах.\n\n"
                                            "Ошибки загрузки:\n" +
                                            model_->GetErrorLog().join("\n"));
        } else {
            result_line_edit_->setPlainText(files.join("\n\n"));

            if (!model_->GetErrorLog().isEmpty()) {
                QMessageBox::warning(this, "Предупреждение",
                                     QString("Загружено %1 записей о файлах.\nБыло пропущено %2 строк с ошибками.\n\n"
                                             "Нажмите 'Показать логи' для деталей.")
                                         .arg(model_->Count())
                                         .arg(model_->GetErrorLog().size()));
            }
        }

        UpdateStatusBar();
    }
}

void MainWindow::OnShowLogsClicked() {
    QDialog* logDialog = new QDialog(this);
    logDialog->setWindowTitle("Лог ошибок");
    logDialog->resize(700, 500);

    QVBoxLayout* layout = new QVBoxLayout(logDialog);

    QLabel* titleLabel = new QLabel("<h3>Журнал ошибок при загрузке файлов</h3>");
    layout->addWidget(titleLabel);

    QTextEdit* logTextEdit = new QTextEdit();
    logTextEdit->setReadOnly(true);
    logTextEdit->setFont(QFont("Monospace", 10));

    QStringList errors = model_->GetErrorLog();
    if (errors.isEmpty()) {
        logTextEdit->setPlainText("Лог ошибок пуст.\n\n"
                                  "Ошибки появляются здесь при загрузке файлов с некорректными строками.");
    } else {
        QString logContent = QString("Всего ошибок: %1\n\n").arg(errors.size());
        logContent += "========================================\n";
        for (int i = 0; i < errors.size(); ++i) {
            logContent += QString("%1. %2\n").arg(i + 1).arg(errors[i]);
            logContent += "----------------------------------------\n";
        }
        logTextEdit->setPlainText(logContent);
    }

    layout->addWidget(logTextEdit);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* refreshButton = new QPushButton("Обновить");
    QPushButton* clearLogsButton = new QPushButton("Очистить лог");
    QPushButton* exportButton = new QPushButton("Экспорт в файл");
    QPushButton* closeButton = new QPushButton("Закрыть");

    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(clearLogsButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);

    connect(refreshButton, &QPushButton::clicked, [logTextEdit, this]() {
        QStringList errors = model_->GetErrorLog();
        if (errors.isEmpty()) {
            logTextEdit->setPlainText("Лог ошибок пуст.");
        } else {
            QString logContent = QString("Всего ошибок: %1\n\n").arg(errors.size());
            logContent += "========================================\n";
            for (int i = 0; i < errors.size(); ++i) {
                logContent += QString("%1. %2\n").arg(i + 1).arg(errors[i]);
                logContent += "----------------------------------------\n";
            }
            logTextEdit->setPlainText(logContent);
        }
    });

    connect(clearLogsButton, &QPushButton::clicked, [this, logTextEdit]() {
        model_->ClearErrorLog();
        logTextEdit->setPlainText("Лог ошибок очищен.\n\n"
                                  "Новые ошибки будут появляться здесь при загрузке файлов.");
        UpdateStatusBar();
    });

    connect(exportButton, &QPushButton::clicked, [this, logDialog]() {
        QString path = QFileDialog::getSaveFileName(logDialog, "Сохранить лог",
                                                    "error_log.txt",
                                                    "Текстовые файлы (*.txt)");
        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                QStringList errors = model_->GetErrorLog();
                stream << "Лог ошибок парсера файлов\n";
                stream << "Создан: " << QDateTime::currentDateTime().toString() << "\n";
                stream << "Всего ошибок: " << errors.size() << "\n";
                stream << "========================================\n\n";

                for (int i = 0; i < errors.size(); ++i) {
                    stream << QString("%1. %2\n").arg(i + 1).arg(errors[i]);
                    stream << "----------------------------------------\n";
                }
                file.close();

                QMessageBox::information(logDialog, "Успех",
                                         QString("Лог сохранен в файл:\n%1").arg(path));
            } else {
                QMessageBox::warning(logDialog, "Ошибка", "Не удалось сохранить файл");
            }
        }
    });

    connect(closeButton, &QPushButton::clicked, logDialog, &QDialog::close);

    logDialog->exec();
}

void MainWindow::OnLoadCommandsClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Выберите файл с командами", "",
                                                "Командные файлы (*.txt *.cmd);;Все файлы (*)");

    if (!path.isEmpty()) {
        ProcessCommandFile(path);
    }
}

void MainWindow::OnClearButtonClicked() {
    ClearDisplay();
    model_->Clear();
    model_->ClearErrorLog();
    UpdateStatusBar();
}

void MainWindow::OnHelpButtonClicked() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Справка");
    dialog->resize(500, 400);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QTextBrowser* browser = new QTextBrowser();
    browser->setHtml(
        "<h2>Справка</h2>"
        "<p><b>Формат ввода:</b> ИмяФайла ГГГГ.ММ.ДД РазмерВБайтах</p>"
        "<p><b>Примеры:</b></p>"
        "<ul>"
        "<li>document.txt 2024.12.25 1024</li>"
        "<li>report.pdf 2024.11.15 2048576</li>"
        "<li>image.png 2024.10.30 512000</li>"
        "</ul>"
        "<p><b>При загрузке файла:</b></p>"
        "<ul>"
        "<li>Валидные строки загружаются</li>"
        "<li>Невалидные строки пропускаются и логируются</li>"
        "</ul>"
        "<p><b>Командный файл:</b></p>"
        "<ul>"
        "<li>ADD имя; дата; размер - добавить запись о файле</li>"
        "<li>REM условие - удалить записи о файлах</li>"
        "<li>SAVE filename - сохранить данные</li>"
        "</ul>"
        "<p><b>Условия для REM:</b></p>"
        "<ul>"
        "<li>date == ГГГГ.ММ.ДД</li>"
        "<li>date < ГГГГ.ММ.ДД</li>"
        "<li>date > ГГГГ.ММ.ДД</li>"
        "<li>filename == значение</li>"
        "<li>filename contains значение</li>"
        "<li>size == значение</li>"
        "<li>size < значение</li>"
        "<li>size > значение</li>"
        "</ul>"
        "<p><b>Просмотр логов:</b></p>"
        "<ul>"
        "<li>Нажмите 'Показать логи' для просмотра ошибок</li>"
        "<li>Можно очистить лог или экспортировать в файл</li>"
        "</ul>"
        );
    layout->addWidget(browser);

    QPushButton* close_btn = new QPushButton("Закрыть");
    layout->addWidget(close_btn);
    connect(close_btn, &QPushButton::clicked, dialog, &QDialog::close);

    dialog->exec();
}

void MainWindow::ProcessMultipleLines(const QString& data) {
    QStringList lines = data.split("\n", Qt::SkipEmptyParts);
    int successCount = 0;
    int errorCount = 0;
    QStringList results;

    for (const QString& line : lines) {
        try {
            Parser parser(line.trimmed());
            FileInfo file = parser.Parse();
            results << file.ToString();
            successCount++;
        } catch (const ParseException& e) {
            results << QString("ОШИБКА: %1\n   %2").arg(line).arg(e.getMessage());
            errorCount++;
        }
    }

    result_line_edit_->setPlainText(results.join("\n\n"));
    setWindowTitle(QString("Парсер файлов - Успешно: %1, Ошибок: %2").arg(successCount).arg(errorCount));
}

void MainWindow::UpdateDisplay(const FileInfo& file) {
    result_line_edit_->setPlainText(file.ToString());
    result_line_edit_->setStyleSheet("");
}

void MainWindow::ShowError(const QString& error_message) {
    result_line_edit_->setPlainText(error_message);
    result_line_edit_->setStyleSheet("QTextEdit { color: red; }");
}

void MainWindow::ClearDisplay() {
    input_text_edit_->clear();
    result_line_edit_->clear();
    result_line_edit_->setStyleSheet("");
}

void MainWindow::UpdateStatusBar() {
    if (status_label_) {
        status_label_->setText(QString("Всего записей о файлах: %1 | Ошибок в логе: %2")
                                   .arg(model_->Count())
                                   .arg(model_->GetErrorLog().size()));
    }
}

void MainWindow::ProcessCommandFile(const QString& filename) {
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файл:\n%1").arg(filename));
        return;
    }

    QTextStream stream(&file);
    QList<FileModel::CommandResult> results;
    int lineNum = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lineNum++;

        if (line.trimmed().isEmpty() || line.trimmed().startsWith('#')) {
            continue;
        }

        FileModel::CommandResult result = model_->ExecuteCommand(line);
        results.append(result);

        qDebug() << QString("[Строка %1] %2: %3")
                        .arg(lineNum)
                        .arg(result.success ? "УСПЕХ" : "ОШИБКА")
                        .arg(result.message);
    }

    file.close();

    DisplayCommandResults(results);

    QString outputPath = QDir::homePath() + "/commands_result.txt";
    SaveResultsToFile(results, outputPath);

    UpdateStatusBar();

    QMessageBox::information(this, "Готово",
                             QString("Команды выполнены.\n"
                                     "Результат сохранен в домашнюю директорию:\n%1\n\n"
                                     "Успешно: %2\nОшибок: %3")
                                 .arg(outputPath)
                                 .arg(std::count_if(results.begin(), results.end(),
                                                    [](const FileModel::CommandResult& r) { return r.success; }))
                                 .arg(std::count_if(results.begin(), results.end(),
                                                    [](const FileModel::CommandResult& r) { return !r.success; })));
}

void MainWindow::DisplayCommandResults(const QList<FileModel::CommandResult>& results) {
    if (results.isEmpty()) {
        return;
    }

    int successCount = 0;
    int errorCount = 0;
    for (const auto& r : results) {
        if (r.success) successCount++;
        else errorCount++;
    }

    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Результаты выполнения команд");
    dialog->resize(700, 500);

    QVBoxLayout* layout = new QVBoxLayout(dialog);

    QLabel* titleLabel = new QLabel(QString("<h3>Выполнение команд</h3>"
                                            "<p>Всего команд: %1 | Успешно: %2 | Ошибок: %3</p>")
                                        .arg(results.size())
                                        .arg(successCount)
                                        .arg(errorCount));
    layout->addWidget(titleLabel);

    QTextEdit* textEdit = new QTextEdit();
    textEdit->setReadOnly(true);
    textEdit->setFont(QFont("Monospace", 10));

    QString content;
    for (int i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        content += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
        content += QString("Команда %1: %2\n").arg(i + 1).arg(r.success ? "✓ УСПЕХ" : "✗ ОШИБКА");
        content += QString("Сообщение: %1\n").arg(r.message);
        if (r.affectedRows > 0) {
            content += QString("Затронуто записей: %1\n").arg(r.affectedRows);
        }
    }
    content += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    content += QString("\nТекущее состояние модели:\n");
    content += QString("Всего записей о файлах: %1\n").arg(model_->Count());

    QStringList files = model_->GetFilesStringList();
    if (!files.isEmpty()) {
        content += QString("\nСписок файлов:\n");
        for (int i = 0; i < files.size(); ++i) {
            content += QString("%1. %2\n").arg(i + 1).arg(files[i]);
        }
    }

    textEdit->setPlainText(content);
    layout->addWidget(textEdit);

    QPushButton* closeButton = new QPushButton("Закрыть");
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);

    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::close);

    dialog->exec();
}

void MainWindow::SaveResultsToFile(const QList<FileModel::CommandResult>& results, const QString& filepath) {
    QFile file(filepath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось сохранить результат в:" << filepath;
        return;
    }

    QTextStream stream(&file);

    stream << "========================================\n";
    stream << "РЕЗУЛЬТАТЫ ВЫПОЛНЕНИЯ КОМАНД\n";
    stream << "========================================\n";
    stream << "Дата и время: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    stream << "========================================\n\n";

    int successCount = 0;
    int errorCount = 0;
    for (const auto& r : results) {
        if (r.success) successCount++;
        else errorCount++;
    }

    stream << QString("Всего команд: %1 | Успешно: %2 | Ошибок: %3\n\n").arg(results.size()).arg(successCount).arg(errorCount);

    for (int i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        stream << QString("Команда %1: %2\n").arg(i + 1).arg(r.success ? "УСПЕХ" : "ОШИБКА");
        stream << QString("Сообщение: %1\n").arg(r.message);
        if (r.affectedRows > 0) {
            stream << QString("Затронуто записей: %1\n").arg(r.affectedRows);
        }
        stream << "----------------------------------------\n";
    }

    stream << "\n========================================\n";
    stream << "ТЕКУЩЕЕ СОСТОЯНИЕ МОДЕЛИ\n";
    stream << "========================================\n";
    stream << QString("Всего записей о файлах: %1\n\n").arg(model_->Count());

    QStringList files = model_->GetFilesStringList();
    if (!files.isEmpty()) {
        stream << "Список файлов:\n";
        for (int i = 0; i < files.size(); ++i) {
            stream << QString("%1. %2\n").arg(i + 1).arg(files[i]);
        }
    } else {
        stream << "Список файлов пуст.\n";
    }

    file.close();
    qDebug() << "Результат сохранен в:" << filepath;
}