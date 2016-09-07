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

// Returns word under cursor
QString MyTextEdit::lineUnderCursor(){
    selectLine();
    QTextCursor cur = this->textCursor();
    QString line = cur.selectedText();
    cur.clearSelection();
    this->setTextCursor(cur);
    return line;
}

// Returns word under cursor
QString MyTextEdit::wordUnderCursor(){
    selectWord();
    QTextCursor cur = this->textCursor();
    QString word = cur.selectedText();
    cur.clearSelection();
    this->setTextCursor(cur);
    return word;
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
*   comments based on language of file type
*   If no text is selected then it selects the current line
*/
void MyTextEdit::toggleComment(){
    QTextCursor cur = this->textCursor();
    if (!cur.hasSelection()){
        selectLine();
    }

    int endSelection = cur.selectionEnd();
    cur.setPosition(cur.selectionStart());
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    this->setTextCursor(cur);

    QString line = lineUnderCursor();

    QString commentStart;
    QString commentEnd;

    if(fileType == "html" or fileType == "css"){

        if(fileType == "html"){
            commentStart = "<!--";
            commentEnd = "-->";
        }else{
            commentStart = "/*";
            commentEnd = "*/";
        }

        if(line.startsWith(commentStart)){
            for(int i=0; i < commentStart.length(); i++){
                cur.deleteChar();
            }
            endSelection -= commentStart.length();

            cur.setPosition(endSelection);
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            this->setTextCursor(cur);
            QString line = lineUnderCursor();

            if(line.endsWith(commentEnd)){
                for(int i=0; i < commentEnd.length(); i++){
                    cur.deletePreviousChar();
                }
            }
        }else{
            cur.insertText(commentStart);
            endSelection += commentStart.length();

            cur.setPosition(endSelection);
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            this->setTextCursor(cur);
            QString line = lineUnderCursor();

            if(!line.endsWith(commentEnd)){
                cur.insertText(commentEnd);
            }
        }

    }else{
        if(fileType == "py"){
            commentStart = "#";
        }else{
            commentStart = "//";
        }

        if(line.startsWith(commentStart)){
            for(int i=0; i < commentStart.length(); i++){
                cur.deleteChar();
            }
            endSelection -= commentStart.length();
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);

            while(cur.position() < endSelection){
                cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
                this->setTextCursor(cur);
                line = lineUnderCursor();

                if(line.startsWith(commentStart)){
                    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                    for(int i=0; i < commentStart.length(); i++){
                        cur.deleteChar();
                    }
                    endSelection -= commentStart.length();

                }
                cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);

            }

        }else{
            cur.insertText(commentStart);
            endSelection += commentStart.length();
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);

            while(cur.position() < endSelection){
                cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
                this->setTextCursor(cur);
                line = lineUnderCursor();

                if(!line.startsWith(commentStart)){
                    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                    cur.insertText(commentStart);
                    endSelection += commentStart.length();
                }

                cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            }
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
    // Indent selection on tab
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

    }else if(e->key() == 16777218){
        // Unindent Selection on backtab
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
        if(line.startsWith("\t")){
            cur.deleteChar();
            endSelection -= 1;
        }

        cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
        while(cur.position() < endSelection){
            cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
            cur.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString line = cur.selectedText();
            cur.clearSelection();
            if(line.startsWith("\t")){
                cur.deleteChar();
                endSelection -= 1;
            }
            cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
        }

        this->setTextCursor(cur);

    }else{
        QPlainTextEdit::keyPressEvent(e);
    }
}
