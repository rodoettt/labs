#ifndef ADDFILEDIALOG_H
#define ADDFILEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDateEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

class AddFileDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* filenameEdit;
    QDateEdit* dateEdit;
    QSpinBox* sizeSpin;
    QPushButton* okButton;
    QPushButton* cancelButton;

public:
    AddFileDialog(QWidget* parent = nullptr);

    QString getFilename() const;
    QString getDate() const;
    int getSize() const;

private slots:
    void validateInput();
};

#endif // ADDFILEDIALOG_H