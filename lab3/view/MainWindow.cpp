#include "MainWindow.h"
#include "AddFileDialog.h"
#include "ImportWindow.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    model = new FileManagerModel(this);

    // Начальные данные
    model->addFile("document.txt", "2023.01.15", 1024);
    model->addFile("image.jpg", "2023.02.20", 204800);
    model->addFile("video.mp4", "2023.03.10", 5242880);

    setupUI();
    refreshTable();
    updateStats();

    setWindowTitle("Учет файлов - Система с обработкой ошибок");
    setMinimumSize(900, 600);

    connect(model, &FileManagerModel::dataChanged, this, &MainWindow::onModelDataChanged);
    connect(model, &FileManagerModel::errorOccurred, this, &MainWindow::onModelError);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    // Верхняя панель с кнопками
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Добавить файл", this);
    deleteButton = new QPushButton("Удалить выбранный", this);
    importButton = new QPushButton("Импорт из файла", this);
    totalButton = new QPushButton("Общий размер", this);
    showLogButton = new QPushButton("Показать лог ошибок", this);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(importButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(totalButton);
    buttonLayout->addWidget(showLogButton);

    // Статистика
    QHBoxLayout* statsLayout = new QHBoxLayout();
    countLabel = new QLabel("Файлов: 0", this);
    totalLabel = new QLabel("Общий размер: 0 байт", this);
    statsLayout->addWidget(countLabel);
    statsLayout->addStretch();
    statsLayout->addWidget(totalLabel);

    // Таблица и лог в сплитере
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(3);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Название файла" << "Дата создания" << "Размер (байт)");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    logWidget = new QTextEdit(this);
    logWidget->setReadOnly(true);
    logWidget->setMaximumHeight(150);
    logWidget->setPlaceholderText("Лог ошибок будет отображаться здесь...");
    logWidget->setStyleSheet("background-color: #2d2d2d; color: #f0f0f0; font-family: monospace;");

    splitter->addWidget(tableWidget);
    splitter->addWidget(logWidget);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(statsLayout);
    mainLayout->addWidget(splitter);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // Подключение сигналов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddFile);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteFile);
    connect(importButton, &QPushButton::clicked, this, &MainWindow::onImportData);
    connect(totalButton, &QPushButton::clicked, this, &MainWindow::onShowTotal);
    connect(showLogButton, &QPushButton::clicked, this, &MainWindow::onShowLog);

    // Стилизация
    setStyleSheet(R"(
        QMainWindow { background-color: #f5f5f5; }
        QPushButton {
            background-color: #007aff;
            border: none;
            border-radius: 5px;
            color: white;
            font-size: 12px;
            font-weight: bold;
            padding: 8px 16px;
        }
        QPushButton:hover { background-color: #0051d5; }
        QTableWidget {
            background-color: white;
            alternate-background-color: #f9f9f9;
            gridline-color: #e0e0e0;
        }
        QHeaderView::section {
            background-color: #e8e8e8;
            padding: 6px;
            font-weight: bold;
        }
        QLabel { font-size: 13px; font-weight: bold; color: #333; }
    )");
}

void MainWindow::refreshTable() {
    const auto& files = model->getFiles();
    tableWidget->setRowCount(files.size());

    for (size_t i = 0; i < files.size(); ++i) {
        QTableWidgetItem* filenameItem = new QTableWidgetItem(files[i].getFilename());
        filenameItem->setFlags(filenameItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 0, filenameItem);

        QTableWidgetItem* dateItem = new QTableWidgetItem(files[i].getDate());
        dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);
        tableWidget->setItem(i, 1, dateItem);

        QTableWidgetItem* sizeItem = new QTableWidgetItem(QString::number(files[i].getSize()));
        sizeItem->setFlags(sizeItem->flags() & ~Qt::ItemIsEditable);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(i, 2, sizeItem);
    }
}

void MainWindow::updateStats() {
    countLabel->setText(QString("Файлов: %1").arg(model->getFileCount()));

    long long totalSize = model->getTotalSize();
    QString totalText = QString("Общий размер: %1 байт").arg(totalSize);

    if (totalSize > 1024 * 1024) {
        totalText += QString(" (%1 МБ)").arg(totalSize / (1024.0 * 1024.0), 0, 'f', 2);
    } else if (totalSize > 1024) {
        totalText += QString(" (%1 КБ)").arg(totalSize / 1024.0, 0, 'f', 2);
    }

    totalLabel->setText(totalText);
}

void MainWindow::onModelDataChanged() {
    refreshTable();
    updateStats();
}

void MainWindow::onModelError(const QString& error) {
    logWidget->append(QString("[%1] %2").arg(QDateTime::currentDateTime().toString("hh:mm:ss")).arg(error));
}

void MainWindow::onAddFile() {
    AddFileDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        model->addFile(dialog.getFilename(), dialog.getDate(), dialog.getSize());
    }
}

void MainWindow::onDeleteFile() {
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите файл для удаления!");
        return;
    }

    const auto& files = model->getFiles();
    QString filename = files[currentRow].getFilename();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение",
        QString("Удалить файл \"%1\"?").arg(filename),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        model->removeFile(currentRow);
    }
}

void MainWindow::onShowTotal() {
    long long totalSize = model->getTotalSize();
    QString message = QString("Общий размер: %1 байт\nФайлов: %2")
                          .arg(totalSize).arg(model->getFileCount());

    if (totalSize > 1024 * 1024) {
        message += QString("\n(%1 МБ)").arg(totalSize / (1024.0 * 1024.0), 0, 'f', 2);
    } else if (totalSize > 1024) {
        message += QString("\n(%1 КБ)").arg(totalSize / 1024.0, 0, 'f', 2);
    }

    QMessageBox::information(this, "Статистика", message);
}

void MainWindow::onImportData() {
    ImportWindow importWin(this);
    connect(&importWin, &ImportWindow::dataProcessed,
            [this](const QStringList& lines) {
                int added = model->parseAndAddFromLines(lines);
                QMessageBox::information(this, "Импорт",
                                         QString("Добавлено: %1 из %2 строк\nОшибки записаны в лог")
                                             .arg(added).arg(lines.size()));
            });
    importWin.exec();
}

void MainWindow::onShowLog() {
    QString log = model->getErrorLogString();
    QMessageBox::information(this, "Лог ошибок", log);
}