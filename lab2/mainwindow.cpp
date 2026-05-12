#include "mainwindow.h"
#include "addfiledialog.h"
#include "ImportWindow.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    // Инициализация начальных данных
    files.push_back(File("document.txt", "2023.01.15", 1024));
    files.push_back(File("image.jpg", "2023.02.20", 204800));
    files.push_back(File("video.mp4", "2023.03.10", 5242880));

    setupUI();
    refreshTable();

    setWindowTitle("Учет файлов");
    setMinimumSize(800, 500);

    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QPushButton {
            background-color: #007aff;
            border: none;
            border-radius: 5px;
            color: white;
            font-size: 12px;
            font-weight: bold;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #0051d5;
        }
        QTableWidget {
            background-color: white;
            alternate-background-color: #f9f9f9;
            gridline-color: #e0e0e0;
            font-size: 12px;
        }
        QHeaderView::section {
            background-color: #e8e8e8;
            padding: 6px;
            border: none;
            font-weight: bold;
        }
        QLabel {
            font-size: 14px;
            font-weight: bold;
            color: #333;
        }
    )");
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout* buttonLayout = new QHBoxLayout();

    addButton = new QPushButton("Добавить файл", this);
    deleteButton = new QPushButton("Удалить выбранный", this);
    importButton = new QPushButton("Импорт из файла", this);
    totalButton = new QPushButton("Общий размер", this);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(importButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(totalButton);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Название файла" << "Дата создания" << "Размер (байт)");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* infoLayout = new QHBoxLayout();
    totalLabel = new QLabel("Общий размер: 0 байт", this);
    infoLayout->addWidget(totalLabel);
    infoLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(infoLayout);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddFile);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteFile);
    connect(importButton, &QPushButton::clicked, this, &MainWindow::onImportData);
    connect(totalButton, &QPushButton::clicked, this, &MainWindow::onShowTotal);
}

void MainWindow::refreshTable() {
    tableWidget->setRowCount(files.size());

    for (size_t i = 0; i < files.size(); ++i) {
        QTableWidgetItem* filenameItem = new QTableWidgetItem(files[i].getFilename());
        filenameItem->setFlags(filenameItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 0, filenameItem);

        QTableWidgetItem* dateItem = new QTableWidgetItem(files[i].getDate());
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 1, dateItem);

        QString sizeStr = QString::number(files[i].getSize());
        QTableWidgetItem* sizeItem = new QTableWidgetItem(sizeStr);
        sizeItem->setFlags(sizeItem->flags() & ~Qt::ItemIsEditable);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(i, 2, sizeItem);
    }

    long long totalSize = getTotalSize();
    QString totalText = QString("Общий размер: %1 байт").arg(totalSize);

    if (totalSize > 1024 * 1024) {
        totalText += QString(" (%1 МБ)").arg(totalSize / (1024.0 * 1024.0), 0, 'f', 2);
    } else if (totalSize > 1024) {
        totalText += QString(" (%1 КБ)").arg(totalSize / 1024.0, 0, 'f', 2);
    }

    totalLabel->setText(totalText);
}

long long MainWindow::getTotalSize() const {
    long long total = 0;
    for (const auto& file : files) {
        total += file.getSize();
    }
    return total;
}

void MainWindow::onAddFile() {
    AddFileDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString filename = dialog.getFilename();
        QString date = dialog.getDate();
        int size = dialog.getSize();

        addFileToManager(filename, date, size);
    }
}

void MainWindow::addFileToManager(const QString& filename, const QString& date, int size) {
    files.push_back(File(filename, date, size));
    refreshTable();

    QMessageBox::information(this, "Успех",
                             QString("Файл \"%1\" успешно добавлен!").arg(filename));
}

void MainWindow::onDeleteFile() {
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите файл для удаления!");
        return;
    }

    QString filename = files[currentRow].getFilename();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение удаления",
        QString("Вы уверены, что хотите удалить файл \"%1\"?").arg(filename),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        files.erase(files.begin() + currentRow);
        refreshTable();
        QMessageBox::information(this, "Успех", "Файл успешно удален!");
    }
}

void MainWindow::onShowTotal() {
    long long totalSize = getTotalSize();
    QString message = QString("Общий размер всех файлов: %1 байт").arg(totalSize);

    if (totalSize > 1024 * 1024) {
        message += QString("\n(%1 МБ)").arg(totalSize / (1024.0 * 1024.0), 0, 'f', 2);
    } else if (totalSize > 1024) {
        message += QString("\n(%1 КБ)").arg(totalSize / 1024.0, 0, 'f', 2);
    }

    message += QString("\n\nВсего файлов: %1").arg(files.size());

    QMessageBox::information(this, "Общая информация", message);
}

void MainWindow::onImportData() {
    ImportWindow importWin(this);
    connect(&importWin, &ImportWindow::dataProcessed, this, &MainWindow::parseAndAddData);
    importWin.exec();
}

void MainWindow::parseAndAddData(const QStringList& lines) {
    int addedCount = 0;
    QRegularExpression regex(R"(^([^|]+)\|(\d{4}\.\d{2}\.\d{2})\|(\d+)$)");

    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty()) continue;

        QRegularExpressionMatch match = regex.match(trimmedLine);
        if (match.hasMatch()) {
            QString filename = match.captured(1).trimmed();
            QString date = match.captured(2);
            int size = match.captured(3).toInt();

            files.push_back(File(filename, date, size));
            addedCount++;
        }
    }

    if (addedCount > 0) {
        refreshTable();
        QMessageBox::information(this, "Импорт завершен",
                                 QString("Успешно импортировано %1 файлов!").arg(addedCount));
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не найдено корректных данных!\n"
                             "Ожидаемый формат: Название файла|ГГГГ.ММ.ДД|Размер");
    }
}