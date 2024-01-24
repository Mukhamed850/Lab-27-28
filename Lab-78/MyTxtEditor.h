#ifndef TXTEDITOR_H
#define TXTEDITOR_H

#include <QWidget>
#include <QMap>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QMenuBar>
#include <QPainter>
#include <QTextEdit>
#include <QLabel>

class MyTxtEditor : public QMainWindow {
    Q_OBJECT
    QString filename;
    QMap<QString, QString>* settings;
    QTextEdit* mTextEdit;
    void updateTextSize();
    void updateTextColor();
    void updateBackgroundColor();
private slots:
    void openSettings();
    void saveSettings();
    void closeProgram();
    void changeSize();
    void changeColor();
    void changeBackgroundColor();
public:
    MyTxtEditor(QWidget *parent = nullptr);
    ~MyTxtEditor();
};
#endif // TXTEDITOR_H
