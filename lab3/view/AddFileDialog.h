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
#include <QMessageBox>

class AddFileDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* filenameEdit;
    QDateEdit* dateEdit;
    QSpinBox* sizeSpin;
    QPushButton* okButton;
    QPushButton* cancelButton;

    void setupUI();
    void validateInput();

public:
    explicit AddFileDialog(QWidget* parent = nullptr);

    QString getFilename() const;
    QString getDate() const;
    int getSize() const;

private slots:
    void onOkClicked();
};

#endif // ADDFILEDIALOG_H