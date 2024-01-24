#include "MyTxtEditor.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QString>
#include <QInputDialog>
#include <QLabel>
#include <QFont>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QColor>

MyTxtEditor::MyTxtEditor(QWidget *parent) : QMainWindow(parent) {
    // Создание основного окна приложения
    QWidget *ui_area = new QWidget;
    setCentralWidget(ui_area);

    // Создание действий (actions) для меню
    QAction* openSettings = new QAction("Открыть настройки");
    QAction* saveSettings = new QAction("Сохранить настройки");
    QAction* exit = new QAction("Выход");

    // Инициализация QMap для хранения настроек
    this->settings = new QMap<QString, QString>;
    settings->insert("Размер шрифта", "14");
    settings->insert("Цвет шрифта", "Black");
    settings->insert("Цвет фона", "White");

    // Создание меню "Файл" и добавление действий в него
    auto fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(openSettings);
    fileMenu->addAction(saveSettings);
    fileMenu->addAction(exit);

    // Подключение слотов к сигналам
    connect(openSettings, SIGNAL(triggered()), this, SLOT(openSettings()));
    connect(saveSettings, SIGNAL(triggered()), this, SLOT(saveSettings()));
    connect(exit, SIGNAL(triggered()), this, SLOT(closeProgram()));

    // Создание меню "Настройки" и добавление действий в него
    auto helpMenu = menuBar()->addMenu(tr("Настройки"));
    QAction* changeFontSize = new QAction("Изменить размер");
    QAction* changeTextColor = new QAction("Изменить цвет");
    QAction* changeBackgroundColorColor = new QAction("Изменить фон");
    helpMenu->addAction(changeBackgroundColorColor);
    helpMenu->addAction(changeFontSize);
    helpMenu->addAction(changeTextColor);

    // Подключение слотов к сигналам
    connect(changeBackgroundColorColor, SIGNAL(triggered()), this, SLOT(changeBackgroundColor()));
    connect(changeFontSize, SIGNAL(triggered()), this, SLOT(changeSize()));
    connect(changeTextColor, SIGNAL(triggered()), this, SLOT(changeColor()));

    // Инициализация QTextEdit и его добавление в главное окно
    filename = "";
    QVBoxLayout *l = new QVBoxLayout(this);
    mTextEdit = new QTextEdit;
    l->addWidget(mTextEdit);
    ui_area->setLayout(l);
    setMinimumSize(500,300);
}

// Обновление размера шрифта текстового редактора
void MyTxtEditor::updateTextSize() {
    QFont font = mTextEdit->currentFont();
    font.setPointSize(settings->value("FontSize").toInt());
    mTextEdit->setFont(font);
}

// Открытие файла с настройками
void MyTxtEditor::openSettings() {
    // Выбор файла и его открытие
    filename = QFileDialog::getOpenFileName(this, tr("Открыть настройки"), "./", tr("Настройки (*.xml)"));
    QFile* file = new QFile(filename);

    // Попытка открытия файла, если не удалось - возврат
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    // Чтение XML-данных
    QXmlStreamReader xml(file);
    xml.readNext();

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::Invalid)
            qDebug() << xml.errorString();
        if (token == QXmlStreamReader::StartElement) {
            // Обработка элементов XML и обновление соответствующих настроек
            if (xml.name() == "Цвет фона") {
                xml.readNext();
                settings->insert("Цвет фона", xml.text().toString());
            }
            else if (xml.name() == "Цвет") {
                xml.readNext();
                settings->insert("Цвет", xml.text().toString());
            }
            else if (xml.name() == "Размер шрифта") {
                xml.readNext();
                settings->insert("Размер шрифта", xml.text().toString());
            }
            xml.readNext();
        }
    }

    // Обновление размера шрифта и цветов
    updateTextSize();
    updateTextColor();
    updateBackgroundColor();
}

// Сохранение текущих настроек в файл
void MyTxtEditor::saveSettings() {
    // Создание файла для записи
    QFile* file = new QFile("qtNotepadSettings.xml");
    file->open(QIODevice::Text|QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(file);
    xmlWriter.setAutoFormatting(true);
    QMap<QString, QString>::iterator it;

    // Запись в XML-формате
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Настройки");

    for (it = settings->begin(); it != settings->end(); ++it)
        xmlWriter.writeTextElement(it.key(), it.value());
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file->close();
}

// Обновление цвета текста в текстовом редакторе
void MyTxtEditor::updateTextColor() {
    QString v = mTextEdit->toPlainText();
    mTextEdit->clear();
    mTextEdit->setTextColor(QColor(settings->value("Color")));
    mTextEdit->insertPlainText(v);
}

// Изменение цвета фона текстового редактора
void MyTxtEditor::changeBackgroundColor() {
    // Создание диалогового окна для выбора цвета фона
    QInputDialog qDialog;
    QStringList items;
    items << QString("White");
    items << QString("Black");
    items << QString("Red");
    items << QString("Green");
    items << QString("Blue");
    items << QString("Yellow");
    items << QString("Gray");
    qDialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
    qDialog.setComboBoxItems(items);
    qDialog.setWindowTitle("Изменить цвет фона");

    // Отображение диалогового окна и обновление цвета фона
    if(qDialog.exec()) {
        settings->insert("BackgroundColor", qDialog.textValue());
        updateBackgroundColor();
    }
}

// Обновление цвета фона текстового редактора
void MyTxtEditor::updateBackgroundColor() {
    QPalette p = mTextEdit->palette();
    p.setColor(QPalette::Base, settings->value("BackgroundColor"));
    mTextEdit->setPalette(p);
}
// Изменение размера шрифта
void MyTxtEditor::changeSize() {
    // Создание диалогового окна для ввода нового размера шрифта
    bool ok;
    QString new_size = QInputDialog::getText(this, tr("Изменить размер"), tr("Размер шрифта"), QLineEdit::Normal, settings->value("FontSize"), &ok);
    QRegExp re("\\d*");
    // Проверка корректности ввода и обновление размера шрифта
    if(ok && !new_size.isEmpty() && re.exactMatch(new_size)) {
        settings->insert("FontSize", new_size);
        updateTextSize();
    }
}
// Изменение цвета текста
void MyTxtEditor::changeColor() {
    // Создание диалогового окна для выбора цвета текста
    QInputDialog qDialog;
    QStringList items;
    items << QString("White");
    items << QString("Black");
    items << QString("Red");
    items << QString("Green");
    items << QString("Blue");
    items << QString("Yellow");
    qDialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
    qDialog.setComboBoxItems(items);
    qDialog.setWindowTitle("Изменить цвет");
    // Отображение диалогового окна и обновление цвета текста
    if(qDialog.exec()) {
        settings->insert("Color", qDialog.textValue());
        updateTextColor();
    }
}
// Закрытие приложения
void MyTxtEditor::closeProgram(){ this->close(); }
// Деструктор
MyTxtEditor::~MyTxtEditor(){}
