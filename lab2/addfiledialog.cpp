#include "addfiledialog.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QRegularExpression>

AddFileDialog::AddFileDialog(QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Добавить новый файл");
    setMinimumSize(400, 250);
    setModal(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Форма для ввода данных
    QFormLayout* formLayout = new QFormLayout();

    // Поле для названия файла
    filenameEdit = new QLineEdit(this);
    filenameEdit->setPlaceholderText("например: document.txt");
    filenameEdit->setMinimumWidth(250);
    formLayout->addRow("Название файла:", filenameEdit);

    // Поле для даты
    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy.MM.dd");
    formLayout->addRow("Дата создания:", dateEdit);

    // Поле для размера
    sizeSpin = new QSpinBox(this);
    sizeSpin->setRange(0, 1000000000); // до 1 ГБ
    sizeSpin->setSuffix(" байт");
    sizeSpin->setSingleStep(1024);
    sizeSpin->setValue(1024);
    formLayout->addRow("Размер:", sizeSpin);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("Добавить", this);
    cancelButton = new QPushButton("Отмена", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Подключение сигналов
    connect(okButton, &QPushButton::clicked, this, &AddFileDialog::validateInput);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(filenameEdit, &QLineEdit::textChanged, this, &AddFileDialog::validateInput);

    // Стилизация для macOS
    setStyleSheet(R"(
        QDialog {
            background-color: #f5f5f5;
        }
        QLabel {
            font-weight: bold;
            color: #333;
        }
        QLineEdit, QDateEdit, QSpinBox {
            padding: 6px;
            border: 1px solid #d0d0d0;
            border-radius: 4px;
            background-color: white;
        }
        QLineEdit:focus, QDateEdit:focus, QSpinBox:focus {
            border-color: #007aff;
        }
        QPushButton {
            background-color: #007aff;
            border: none;
            border-radius: 5px;
            color: white;
            font-size: 12px;
            font-weight: bold;
            padding: 8px 16px;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #0051d5;
        }
        QPushButton:pressed {
            background-color: #0040a8;
        }
        QPushButton#cancelButton {
            background-color: #8e8e93;
        }
        QPushButton#cancelButton:hover {
            background-color: #6c6c70;
        }
    )");

    cancelButton->setObjectName("cancelButton");

    validateInput();
}

QString AddFileDialog::getFilename() const {
    return filenameEdit->text().trimmed();
}

QString AddFileDialog::getDate() const {
    return dateEdit->date().toString("yyyy.MM.dd");
}

int AddFileDialog::getSize() const {
    return sizeSpin->value();
}

void AddFileDialog::validateInput() {
    QString filename = getFilename();

    if (filename.isEmpty()) {
        okButton->setEnabled(false);
        return;
    }

    // Проверка на допустимые символы в имени файла
    QRegularExpression validChars("^[^<>:\"/\\\\|?*]*$");
    if (!validChars.match(filename).hasMatch()) {
        okButton->setEnabled(false);
        filenameEdit->setToolTip("Имя файла содержит недопустимые символы");
        return;
    }

    filenameEdit->setToolTip("");
    okButton->setEnabled(true);
}