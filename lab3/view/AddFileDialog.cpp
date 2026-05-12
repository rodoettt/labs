#include "AddFileDialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

AddFileDialog::AddFileDialog(QWidget* parent)
    : QDialog(parent) {
    setupUI();
    setWindowTitle("Добавить файл");
    setMinimumSize(400, 250);
    setModal(true);

    // Стилизация для macOS
    setStyleSheet(R"(
        QDialog {
            background-color: #f5f5f5;
        }
        QLabel {
            font-weight: bold;
            color: #333;
            font-size: 12px;
        }
        QLineEdit, QDateEdit, QSpinBox {
            padding: 6px;
            border: 1px solid #d0d0d0;
            border-radius: 4px;
            background-color: white;
            font-size: 12px;
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
}

void AddFileDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Форма для ввода данных
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(10);

    // Поле для названия файла
    filenameEdit = new QLineEdit(this);
    filenameEdit->setPlaceholderText("например: document.txt");
    filenameEdit->setMinimumWidth(250);
    connect(filenameEdit, &QLineEdit::textChanged, this, &AddFileDialog::validateInput);
    formLayout->addRow("Название файла:", filenameEdit);

    // Поле для даты
    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy.MM.dd");
    dateEdit->setMinimumDate(QDate(2000, 1, 1));
    dateEdit->setMaximumDate(QDate::currentDate());
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
    cancelButton->setObjectName("cancelButton");

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Подключение сигналов
    connect(okButton, &QPushButton::clicked, this, &AddFileDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    validateInput();
}

void AddFileDialog::validateInput() {
    QString filename = getFilename();

    if (filename.isEmpty()) {
        okButton->setEnabled(false);
        filenameEdit->setToolTip("Имя файла не может быть пустым");
        return;
    }

    // Проверка на допустимые символы в имени файла
    QRegularExpression invalidChars(R"([<>:\"/\\|?*])");
    if (invalidChars.match(filename).hasMatch()) {
        okButton->setEnabled(false);
        filenameEdit->setToolTip("Имя файла содержит недопустимые символы: < > : \" / \\ | ? *");
        return;
    }

    // Проверка длины имени файла
    if (filename.length() > 255) {
        okButton->setEnabled(false);
        filenameEdit->setToolTip("Имя файла слишком длинное (максимум 255 символов)");
        return;
    }

    filenameEdit->setToolTip("");
    okButton->setEnabled(true);
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

void AddFileDialog::onOkClicked() {
    QString filename = getFilename();

    // Дополнительная проверка перед добавлением
    if (filename.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название файла!");
        return;
    }

    // Проверка на зарезервированные имена Windows
    QStringList reservedNames = {"CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};
    QString upperFilename = filename.toUpper();
    for (const QString& reserved : reservedNames) {
        if (upperFilename == reserved || upperFilename.startsWith(reserved + ".")) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Имя файла '%1' является зарезервированным именем в системе!").arg(filename));
            return;
        }
    }

    accept();
}