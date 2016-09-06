#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QPlainTextEdit>

class MyTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    MyTextEdit(QWidget *parent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent * e);

public slots:

};

#endif // MYTEXTEDIT_H
