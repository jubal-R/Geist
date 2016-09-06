#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QPlainTextEdit>
#include "highlighter.h"

class MyTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    MyTextEdit(QWidget *parent = 0);
    QString getFilepath();
    QString getFileType();
    void setFilePath(QString newFilepath);
    void setFileType(QString newFileType);
    void setHighlighter(Highlighter * h);
    void selectLine();
    void selectWord();
    void deleteLine();
    void deleteWord();
    void joinLines();
    void swapLineUp();
    void swapLineDown();
    void toggleComment();

protected:
    virtual void keyPressEvent(QKeyEvent * e);

public slots:

private:
    QString filepath;
    QString fileType;
    Highlighter * highlighter;

};

#endif // MYTEXTEDIT_H
