#include "mytextedit.h"
#include <iostream>

MyTextEdit::MyTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    filepath = "";
    fileType = "";
    highlighter = NULL;
}

/*
 *****************************
 *        Editing            *
 *****************************
*/

// Selects current line where cursor is positioned
void MyTextEdit::selectLine(){
    QTextCursor cur = this->textCursor();
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    this->setTextCursor(cur);
}

// Selects current word where cursor is positioned
void MyTextEdit::selectWord(){
    QTextCursor cur = this->textCursor();
    cur.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    this->setTextCursor(cur);
}

// Deletes current line where cursor is positioned
void MyTextEdit::deleteLine(){
    selectLine();
    QTextCursor cur = this->textCursor();
    if(cur.selectedText() != ""){
        cur.removeSelectedText();
    }
    this->setTextCursor(cur);
}

// Deletes current word where cursor is positioned
void MyTextEdit::deleteWord(){
    selectWord();
    QTextCursor cur = this->textCursor();

    if (cur.selectedText() != ""){
        cur.removeSelectedText();
    }

    this->setTextCursor(cur);
}

// Joins line where cursor is currently positioned with the line below it
void MyTextEdit::joinLines(){
    QTextCursor cur = this->textCursor();

    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    cur.deleteChar();

    this->setTextCursor(cur);
}

// Swaps line where cursor is currently positioned with the line above it
void MyTextEdit::swapLineUp(){
    QTextCursor cur = this->textCursor();
    //  Select current line and store value
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString newTop = cur.selectedText();
    cur.removeSelectedText();
    // Select line above and store value
    cur.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString newBottom = cur.selectedText();
    cur.removeSelectedText();
    // Insert new values
    cur.insertText(newTop);
    cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    cur.insertText(newBottom);
    // Position cursor
    cur.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);

    this->setTextCursor(cur);
}

// Swaps line where cursor is currently positioned with the line below it
void MyTextEdit::swapLineDown(){
    QTextCursor cur = this->textCursor();
    //  Select current line and store value
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString newBottom = cur.selectedText();
    cur.removeSelectedText();
    // Select line below and store value
    cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString newTop = cur.selectedText();
    cur.removeSelectedText();
    // Insert new values
    cur.insertText(newBottom);
    cur.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    cur.insertText(newTop);
    // Position cursor
    cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);

    this->setTextCursor(cur);
}

/*  Toggles currently selected line(s) between commented out and uncommented
*   If no text is selected then it selects the current line
*/
void MyTextEdit::toggleComment(){
    QTextCursor cur = this->textCursor();
    if (!cur.hasSelection()){
        selectLine();
    }

    int endSelection = cur.selectionEnd();
    cur.setPosition(cur.selectionStart());
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString line = cur.selectedText();
    cur.clearSelection();

    if(line.startsWith("//")){
        cur.deleteChar();cur.deleteChar();
        endSelection -= 2;
        cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);

        while(cur.position() < endSelection){
            cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            cur.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            line = cur.selectedText();
            cur.clearSelection();

            if(line.startsWith("//")){
                cur.deleteChar();cur.deleteChar();
                endSelection -= 2;
            }

            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
        }

    }else{
        cur.insertText("//");
        endSelection += 2;
        cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);

        while(cur.position() < endSelection){
            cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            cur.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            line = cur.selectedText();
            cur.clearSelection();

            if(!line.startsWith("//")){
                cur.insertText("//");
                endSelection += 2;
            }

            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
        }
    }

    this->setTextCursor(cur);
}


/*
 *****************************
 *         File Info         *
 *****************************
*/
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

/*
 *****************************
 *         Virtual functions *
 *****************************
*/

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
