#include "mytextedit.h"
#include <iostream>

MyTextEdit::MyTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    filepath = "";
    fileType = "";
    highlighter = NULL;
}

QString MyTextEdit::getFilepath(){
    return filepath;
}

QString MyTextEdit::getFileType(){
    return fileType;
}

void MyTextEdit::setFilePath(QString newFilepath){
    filepath = newFilepath;
}
void MyTextEdit::setFileType(QString newFileType){
    fileType = newFileType;
}

void MyTextEdit::setHighlighter(Highlighter * h){
    if(highlighter != NULL){
        delete highlighter;
    }

    highlighter = h;
}



void MyTextEdit::keyPressEvent ( QKeyEvent * e ){
    // Indent selection
    if(e->key() == 16777217){
        QTextCursor cur = this->textCursor();
        if (cur.hasSelection()){
            int endSelection = cur.selectionEnd();
            cur.setPosition(cur.selectionStart());
            cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            cur.insertText("\t");
            endSelection++;
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            while(cur.position() < endSelection){
                cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
                cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                cur.insertText("\t");
                endSelection++;
                cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            }

            this->setTextCursor(cur);

        }else{
            QPlainTextEdit::keyPressEvent(e);
        }

    }else{
        QPlainTextEdit::keyPressEvent(e);
    }
}
