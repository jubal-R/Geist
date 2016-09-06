/*
    Geist - All purpose text/code editor
    Copyright (C) 2016  Jubal Rosenbarker

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "files.h"
#include "conversion.h"
#include "snippets.h"
#include "mytextedit.h"
#include "runner.h"
#include "templates.h"
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QStringListModel>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollBar>
#include <QShortcut>
#include <QSyntaxHighlighter>
#include <QTabWidget>
#include <QTextCursor>
#include <QPlainTextEdit>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;


MyTextEdit *p;

Conversion conversion;
Files files;
Snippets snippets;
Runner runner;
Templates templates;

QString currentSearchTerm;
QStringList foundPositions;
QStringList settings;
QString currentDirectory = QString::fromStdString(files.getHomeDir());    //  Directory of last opened file. Users home folder by default.

QString licence = "Project Page: https://github.com/jubal-R/Geist\n\n"
        "Geist - All purpose text/code editor\n"
        "Copyright (C) 2016  Jubal Rosenbarker\n\n"

        "This program is free software: you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation, either version 3 of the License, or "
        "(at your option) any later version.\n\n"

        "This program is distributed in the hope that it will be useful,"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
        "GNU General Public License for more details.\n\n"

        "You should have received a copy of the GNU General Public License "
        "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";

QString *licencePointer = &licence;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Init vars
    numBlocks = 1;
    newNumBlocks = 0;
    outputMode = 0;
    foundPosElement = 0;
    searchTermLen = 0;
    tabWidth = 4;

    outputModeP = &outputMode; //  0 = ascii, 1 = hex, 2 = strings

    filename = "";  // Name of file in current tab
    theme = "solarizedDark"; // Default theme
    lineColor = QColor(7, 54, 66);

    // UI Setup
    ui->setupUi(this);

    //  Style Sheet
    this->setWindowTitle("Geist");
    MainWindow::setStyleSheet("QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #404f4f; border-radius: 3px 3px 3px 3px;}"
                              "QMenuBar::item {background:#262626;} QMenuBar::item:selected {background: #232629;}"
                              "QMessageBox {color:white;}"
                              "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
                              "QScrollBar::sub-page:vertical {background: #002b36;} QScrollBar::add-page:vertical {background: #002b36;}"
                              "QScrollBar::sub-page:horizonal {background: #002b36;} QScrollBar::add-page:horizontal {background: #002b36;}"
                              "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:#073642;border: 2px solid #002b36;border-radius: 5px 5px 5px 5px; min-height: 30px;}"
                              "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:#073642;border: 2px solid #002b36;border-radius: 5px 5px 5px 5px; min-height: 30px;}");


    ui->tabWidget->setStyleSheet("QPlainTextEdit { background-color: #002b36; color:#839496; border: 0px; selection-background-color: #404f4f; font-family: \"Anonymous Pro\"; font-size:11pt;} "
                                 "QScrollBar:vertical{background: #002b36;} QScrollBar:horizontal{background: #002b36;}"
                                 "QTabBar::tab:selected{color: white; border-bottom: 3px solid #2F4F4F;}"
                                 "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; padding: 1px 5px; background-color: #262626; border-bottom: 3px solid #212121;}");

    ui->menuBar->setStyleSheet("color:white; background-color:#262626; QMenuBar::item {background:black;}");
    ui->centralWidget->layout()->setContentsMargins(0,0,0,0);

    //  Disbale manual scrolling for line numbers
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Hide find/replace bar by default
    ui->findReplaceBar->hide();
    ui->replaceLineEdit->hide();
    ui->replaceButton->hide();
    ui->replaceAllButton->hide();
    ui->label_3->hide();

    // Read settings file
    string set = files.read("settings.txt");
    settings = QString::fromStdString(set).split("\n");

    if(settings.size() > 4){
        tabWidth = settings.at(4).toInt();  // After first tab
    }

    // Create and setup initial tab
    newTab();
    connect(p, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    p->setFocus();
    highlightCurrentLine();

    ui->listWidget->addItem("1");
    QListWidgetItem *item = ui->listWidget->item(numBlocks-1);
    item->setSizeHint(QSize(item->sizeHint().height(), 14));

    //  Setup window based on user's settings
    if (settings.size() > 4){
        MainWindow::resize(settings.at(0).toInt(), settings.at(1).toInt());
        ui->fileOverview->setMaximumWidth(settings.at(2).toInt());
        // Set theme if not default
        if(settings.at(3) != theme){
            if(settings.at(3) == "monokai"){
                on_actionDark_triggered();
            }else if(settings.at(3) == "solarized"){
                on_actionSolarized_triggered();
            }else if(settings.at(3) == "tomorrow"){
                on_actionTommorrow_triggered();
            }else if(settings.at(3) == "tomorrowNight"){
                on_actionTommorrow_Night_triggered();
            }
        }
        for(int i = 5; i < settings.size(); i++){
            open(settings.at(i));
        }
    }

}

MainWindow::~MainWindow()
{
    /*  Save settings
    *   Window size, overview toggle values
    */
    ostringstream oss;
    oss << MainWindow::width() << "\n" << MainWindow::height() << "\n" << ui->fileOverview->width() << "\n" << theme.toStdString() << "\n";
    oss << tabWidth << "\n";

    int numOpenFiles = ui->tabWidget->count();
    for(int i = 0; i < numOpenFiles; i++){
        oss << ui->tabWidget->tabToolTip(i).toStdString() << "\n";
    }

    files.write("settings.txt", oss.str());

    delete ui;
}

/*
 *****************************
 *         Tabs              *
 *****************************
*/

// Switch tab
void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (p != 0){
        disconnect(p, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
        disconnect(p, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    }

    p = qobject_cast<MyTextEdit *>(ui->tabWidget->widget(index));
    newNumBlocks = p->document()->blockCount();
    QObject::connect(p->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->listWidget->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(p->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollOverview(int)));
    connect(p, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    MainWindow::updateLineNums(newNumBlocks);
    connect(p, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
    connect(p, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    ui->fileOverview->setPlainText(p->toPlainText());
    filename = p->getFilepath();
    ui->labelFileType->setText(p->getFileType().toUpper());
    foundPositions.clear();
    foundPosElement = 0;
    highlightCurrentLine();

}

//  Close tab
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (ui->tabWidget->count() < 2){
        newTab();
    }

    // delete highlighter
    ui->tabWidget->setCurrentIndex(index);
    p->setHighlighter(NULL);

        // Set the new current tab
        if (index > 0){
            ui->tabWidget->setCurrentWidget(ui->tabWidget->widget(index - 1));
        }else{
            ui->tabWidget->setCurrentWidget(ui->tabWidget->widget(index + 1));
        }


        // Delete the tab
        delete ui->tabWidget->widget(index);

        // Update filename to correspond to new current tab
        filename = ui->tabWidget->tabToolTip(ui->tabWidget->currentIndex());

}

//  Create new tab
void MainWindow::newTab(){
    if (ui->tabWidget->count() < 50){
        ui->tabWidget->addTab(new MyTextEdit, "New File");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), "");
        filename = "";

        setTabWidth(tabWidth);
        p->setWordWrapMode(QTextOption::NoWrap);
    }
}
void MainWindow::on_actionNew_triggered()
{
    MainWindow::newTab();
}

/*
 *****************************
 *         Files             *
 *****************************
*/

QString MainWindow::getFileType(QString file){
    QStringList sl = file.split(".");
    if(sl.length() > 1)
        return sl.last();
    else
        return "";
}

/*  Open directory of currently active file in default file manager
*   If current tab has no file open then open directory of last active file
*   Else open users home directory
*/
void MainWindow::on_actionOpen_containg_folder_triggered()
{
    ostringstream oss;
    oss << "xdg-open " << getDirectory();
    ui->textBrowser->setPlainText(QString::fromStdString(runner.run(oss.str())));
    p->setFocus();
}

/*  Open file
*   Opens in new tab unless current tab is empty
*   If file does not exist it will be created on save
*/
void MainWindow::open(QString file){

    if (file != ""){

        if(filename != "" || p->toPlainText() != ""){
            newTab();
        }

        filename = file;
        QString filetype = getFileType(file);
        p->setPlainText(QString::fromStdString(files.read(filename.toStdString())));

        p->setFilePath(filename);
        p->setFileType(filetype);
        p->setHighlighter(new Highlighter(filetype, theme, p->document() ));

        if (filename.length() >= 21){
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), filename.right(21));
        }else{
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), filename);
        }
        // Store file path in tool tip
        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), filename);
        ui->labelFileType->setText(filetype.toUpper());

        ui->textBrowser->setText("");
        *outputModeP = 0;
        currentDirectory = QString::fromStdString(getDirectory());
    }
}
void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), currentDirectory, tr("All (*)"));
    open(file);
}

//  Save file
void MainWindow::save(){
    if (filename == "" || *outputModeP != 0){
        on_actionSave_as_triggered();
    }else{
        if(files.write(filename.toStdString(), p->toPlainText().toStdString())){
            ui->textBrowser->setText("Saved");
        }

    }
}
void MainWindow::on_actionSave_triggered()
{
    MainWindow::save();
}
void MainWindow::on_actionSave_as_triggered()
{
    filename = QFileDialog::getSaveFileName(this, tr("Open File"), currentDirectory, tr("All (*)"));
    QString filetype = getFileType(filename);

    if (filename != ""){
        files.write(filename.toStdString(), p->toPlainText().toStdString());

        p->setFilePath(filename);
        p->setFileType(filetype);
        p->setHighlighter(new Highlighter(filetype, theme, p->document() ));

        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), filename.right(21));
        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), filename);
        ui->labelFileType->setText(filetype.toUpper());

        ui->textBrowser->setText("Saved");
       }
}

/*
 *****************************
 *         Searching         *
 *****************************
*/

// Selects current line where cursor is positioned
void MainWindow::selectLine(){
    QTextCursor cur = p->textCursor();
    cur.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    p->setTextCursor(cur);
}

// Selects current word
void MainWindow::selectWord(){
    QTextCursor cur = p->textCursor();
    cur.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    p->setTextCursor(cur);
}

// Toggle find bar
void MainWindow::on_actionFind_triggered()
{
    if(ui->findReplaceBar->isHidden()){
        ui->findReplaceBar->show();
        ui->findLineEdit->setFocus();
    }else{
        if(ui->replaceLineEdit->isHidden()){
            ui->findReplaceBar->hide();
            p->setFocus();
        }else{
            ui->replaceLineEdit->hide();
            ui->replaceButton->hide();
            ui->replaceAllButton->hide();
            ui->label_3->hide();
        }

    }
}

/*  Returns all positions of first char in search term where matches are found
 *  as a string with positions seperated by spaces */
string MainWindow::find(string searchTerm, string content){
    ostringstream oss;
    string line = "";
    string *l = &line;
    bool found = false;
    bool *f = &found;
    int qLen = searchTerm.length();
    char altFirst[1];   // Alternate case(uppercase/lowercase) version of first character

    //  Set altFirst
    if(searchTerm[0] > 90){
        altFirst[0] = searchTerm[0] - 32;
    }else{
        altFirst[0] = searchTerm[0] + 32;
    }

    for(unsigned int i = 0; i < content.length(); i++){
        *l += content[i];

        if(content[i] == searchTerm[0] || content[i] == altFirst[0] ){
            int lineNum = 1;
            char altCase[1];

            while(lineNum < qLen){

                if(searchTerm[lineNum] > 90){
                    altCase[0] = searchTerm[lineNum]-32;
                }else{
                    altCase[0] = searchTerm[lineNum]+32;
                }

                if(content[i+lineNum] == searchTerm[lineNum] || content[i+lineNum] == altCase[0]){
                    if(lineNum == qLen-1){
                        *f = true;
                        oss << i << " ";
                    }

                }else{
                    break;
                }

                lineNum++;
            }

        }
    }
    return oss.str();
}

// Find all instances of search term and cycles through them
void MainWindow::on_findLineEdit_returnPressed()
{
    if (ui->findLineEdit->text() == currentSearchTerm){
            findNext();

    }else{
        QString searchTerm = ui->findLineEdit->text();
        currentSearchTerm = searchTerm;
        searchTermLen = searchTerm.length();

        if (searchTermLen > 0){
            QString found = QString::fromStdString(find(searchTerm.toStdString(), p->toPlainText().toStdString()));
            foundPositions = found.split(" ");
            foundPosElement = 0;
            selectText(foundPositions[foundPosElement].toInt(), searchTermLen);
        }
    }
}
void MainWindow::on_findLineEdit_textChanged(const QString &arg1)
{
    on_findLineEdit_returnPressed();
}

// Selects text given the position of the first char and it's length
void MainWindow::selectText(int pos, int len){
    if (foundPositions.length() > 1){
        QTextCursor cur = p->textCursor();
        cur.setPosition(pos , QTextCursor::MoveAnchor);
        cur.setPosition(pos + len, QTextCursor::KeepAnchor);
        p->setTextCursor(cur);
        ui->listWidget->verticalScrollBar()->setValue(p->verticalScrollBar()->value());
    }
}

//  Find next instance of search term
void MainWindow::findNext(){
    if (ui->findLineEdit->text() == currentSearchTerm){

        if (foundPositions.length() > 1){

            if (foundPosElement < foundPositions.length() - 2){
                foundPosElement++;
            }else{
                foundPosElement = 0;
            }

            selectText(foundPositions[foundPosElement].toInt(), searchTermLen);
        }

    }else{
        on_findLineEdit_returnPressed();
    }
}

// Find previous instance of search term
void MainWindow::findPrev(){

    if (foundPositions.length() > 1){

        if (foundPosElement > 0){
            foundPosElement--;
        }else{
            foundPosElement = foundPositions.length() - 2;
        }

        selectText(foundPositions[foundPosElement].toInt(), searchTermLen);
    }
}

void MainWindow::on_findButton_clicked()
{
    findNext();
    p->setFocus();
}
void MainWindow::on_actionFind_Next_triggered()
{
    findNext();
}
void MainWindow::on_findPrevButton_clicked()
{
    findPrev();
    p->setFocus();
}
void MainWindow::on_actionFind_Previous_triggered()
{
    findPrev();
}

//  Toggle replace bar
void MainWindow::on_actionReplace_triggered()
{
    if(ui->findReplaceBar->isHidden()){
        ui->findReplaceBar->show();
        ui->findLineEdit->setFocus();
        if(ui->replaceLineEdit->isHidden()){
            ui->replaceLineEdit->show();
            ui->replaceButton->show();
            ui->replaceAllButton->show();
            ui->label_3->show();
        }
    }else{

        if(ui->replaceLineEdit->isHidden()){
            ui->replaceLineEdit->show();
            ui->replaceButton->show();
            ui->replaceAllButton->show();
            ui->label_3->show();
        }else{
            ui->replaceLineEdit->hide();
            ui->replaceButton->hide();
            ui->replaceAllButton->hide();
            ui->findReplaceBar->hide();
            ui->label_3->hide();
            p->setFocus();
        }

    }
}

// Replace currently selected found instance of text to be replaced
void MainWindow::on_replaceButton_clicked()
{
    QTextCursor cur = p->textCursor();

    if (cur.selectedText() != ""){
        cur.removeSelectedText();
        cur.insertText(ui->replaceLineEdit->text());
    }

    p->setTextCursor(cur);
    QString searchTerm = ui->findLineEdit->text();
    currentSearchTerm = searchTerm;
    searchTermLen = searchTerm.length();

    if (searchTermLen > 0){
        QString found = QString::fromStdString(find(searchTerm.toStdString(), p->toPlainText().toStdString()));
        foundPositions = found.split(" ");
        foundPosElement = 0;
        selectText(foundPositions[foundPosElement].toInt(), searchTermLen);
    }
}
void MainWindow::on_replaceLineEdit_returnPressed()
{
    MainWindow::on_replaceButton_clicked();
}

//  Replace all found instances of text to be replaced
void MainWindow::on_replaceAllButton_clicked()
{
    QString searchTerm = ui->findLineEdit->text();
    currentSearchTerm = searchTerm;
    do{
        searchTermLen = searchTerm.length();

        if (searchTermLen > 0){
            QString found = QString::fromStdString(find(searchTerm.toStdString(), p->toPlainText().toStdString()));
            foundPositions = found.split(" ");
            foundPosElement = 0;
            selectText(foundPositions[foundPosElement].toInt(), searchTermLen);
        }

        QTextCursor cur = p->textCursor();

        if (cur.selectedText() != ""){
            cur.removeSelectedText();
            cur.insertText(ui->replaceLineEdit->text());
        }

        p->setTextCursor(cur);

    }while(foundPositions.length() > 1);
}

// Go To (Line number)
void MainWindow::on_actionGoTo_triggered()
{
    bool ok = 0;

    if (int line = QInputDialog::getText(this, tr("Enter"),tr("Go to:"), QLineEdit::Normal,"", &ok).toInt()){
        QTextCursor cur = p->textCursor();
        cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
        cur.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line-1);
        p->setTextCursor(cur);
        ui->listWidget->verticalScrollBar()->setValue(p->verticalScrollBar()->value());
    }
}

/*
 *****************************
 *         Editing           *
 *****************************
*/

//  undo/redo
void MainWindow::on_actionUndo_triggered()
{
    p->undo();
}
void MainWindow::on_actionRedo_triggered()
{
    p->redo();
}

//  Delete line where cusor currently resides
void MainWindow::on_actionDelete_line_triggered()
{
    selectLine();
    QTextCursor cur = p->textCursor();
    if(cur.selectedText() != ""){
        cur.removeSelectedText();
        cur.insertText(ui->replaceLineEdit->text());
    }
    p->setTextCursor(cur);
}

//  Delete word where cusor currently resides
void MainWindow::on_actionRemove_word_triggered()
{
    selectWord();
    QTextCursor cur = p->textCursor();

    if (cur.selectedText() != ""){
        cur.removeSelectedText();
        cur.insertText(ui->replaceLineEdit->text());
    }

    p->setTextCursor(cur);
}

/*  Toggles currently selected line(s) between commented out and uncommented
*   If no text is selected then it selects the current line
*/
void MainWindow::on_actionToggle_comment_triggered()
{
    QTextCursor cur = p->textCursor();

    if (cur.hasSelection() == false){
        selectLine();
    }

    int scrollLocation = p->verticalScrollBar()->value();
    int end = cur.selectionEnd();
    QString content = p->toPlainText();
    cur.setPosition(cur.selectionStart());
    cur.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
    int start = cur.position();

    for(int pos = start; pos < end; pos++){
        if(content.at(pos - 1) == '\n'|| pos == start){
            if(content.at(pos) == '/' && content.at(pos + 1) == '/'){
                content.remove(pos, 2);
                end -= 2;
            }else{
                content.insert(pos, "//");
                pos += 2;
                end += 2;
            }
        }
    }

    p->setPlainText(content);
    p->verticalScrollBar()->setValue(scrollLocation);
}

// Join lines
void MainWindow::on_actionJoin_Lines_triggered()
{
    QTextCursor cur = p->textCursor();

    cur.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    cur.deleteChar();

    p->setTextCursor(cur);
}

// Swap line up
void MainWindow::on_actionMove_Line_Up_triggered()
{
    QTextCursor cur = p->textCursor();
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

    p->setTextCursor(cur);
}

// Swap line down
void MainWindow::on_actionSwap_line_down_triggered()
{
    QTextCursor cur = p->textCursor();
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

    p->setTextCursor(cur);
}

/*
 *****************************
 *         Tools             *
 *****************************
*/

/*  Strings
*   Strips all nonprintable ascii characters from contents of current tab
*/
void MainWindow::on_actionStrings_triggered()
{
    if (*outputModeP != 1){
        *outputModeP = 2;
        p->setPlainText( QString::fromStdString(conversion.getStrings(p->toPlainText().toStdString())) );
    }
}
//  Converts ascii values of content of current tab into hexidecimal
void MainWindow::on_actionHex_triggered()
{
    if (*outputModeP == 0 || *outputModeP == 2){
        *outputModeP = 1;
        p->setPlainText( QString::fromStdString( conversion.hex(p->toPlainText().toStdString() ) ) );
    }
}
//  Converts hexidecimal values of content of current tab into ascii
void MainWindow::on_actionAscii_triggered()
{
    if (*outputModeP == 1){
        *outputModeP = 0;
        p->setPlainText( QString::fromStdString( conversion.ascii(p->toPlainText().toStdString() ) ) );
    }
}

/*
 *****************************
 *         Templates         *
 *****************************
*/

// If current tab is empty fill it with the selected code template
void MainWindow::getTemp(int num){

    if(p->toPlainText() == ""){
        p->setPlainText(QString::fromStdString(templates.getTemplate(num)));
    }else{
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Test", "Are you sure you want to apply a template?\nAll unsaved work will be lost.", QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            p->setPlainText(QString::fromStdString(templates.getTemplate(num)));
        }

    }
}
void MainWindow::on_actionAsm_triggered(){ MainWindow::getTemp(0);}
void MainWindow::on_actionC_triggered(){ MainWindow::getTemp(1);}
void MainWindow::on_actionCpluspluss_triggered(){ MainWindow::getTemp(2);}
void MainWindow::on_actionHtml_triggered(){ MainWindow::getTemp(3);}
void MainWindow::on_actionJava_triggered(){ MainWindow::getTemp(4);}
void MainWindow::on_actionCss_triggered(){ MainWindow::getTemp(5);}

//  Get directory of current file
string MainWindow::getDirectory(){
   ostringstream oss;

    if (filename != ""){
        QStringList path = filename.split("/");
        int len = path.length();

        for(int i = 0; i < len - 1; i++){
            oss << path[i].toStdString() << "/";
        }

    }else{
        oss << currentDirectory.toStdString();
    }

    return oss.str();
}

/*
 *****************************
 *         View              *
 *****************************
*/

//  Toggle fullScreen
void MainWindow::on_actionFullScreen_triggered()
{
    if(MainWindow::isFullScreen())
    {
        MainWindow::showNormal();
    }else{
        MainWindow::showFullScreen();
    }
}

//  Toggle file overview
void MainWindow::on_actionOverview_triggered()
{
    if(ui->fileOverview->width() == 0){
        ui->fileOverview->setMaximumWidth(110);
    }else{
        ui->fileOverview->setMaximumWidth(0);
    }
}

//  Toggle Menubar
void MainWindow::on_actionMenubar_triggered()
{
    if(ui->menuBar->height() > 0){
        ui->menuBar->setMaximumHeight(0);
    }else{
        ui->menuBar->setMaximumHeight(100);
    }
}

/*
 *****************************
 *         Themes            *
 *****************************
*/

void MainWindow::updateHighlighterTheme(){
    int current = ui->tabWidget->currentIndex();
    int numOpenTabs = ui->tabWidget->count();
    for(int i = 0; i < numOpenTabs; i++){
        ui->tabWidget->setCurrentIndex(i);
        p->setHighlighter(new Highlighter(p->getFileType(), theme, p->document() ));
    }
    ui->tabWidget->setCurrentIndex(current);
}

void MainWindow::on_actionDark_triggered()
{
    theme = "monokai";
    MainWindow::setStyleSheet("QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #404f4f; border-radius: 3px 3px 3px 3px;}"
                              "QMenuBar::item {background:#262626;} QMenuBar::item:selected {background: #232629;}"
                              "QMessageBox {color:white;}"
                              "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
                              "QScrollBar::sub-page:vertical {background: #272822;} QScrollBar::add-page:vertical {background: #272822;}"
                              "QScrollBar::sub-page:horizonal {background: #272822;} QScrollBar::add-page:horizontal {background: #272822;}"
                              "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:#32332c;border: 2px solid #272822;border-radius: 5px 5px 5px 5px; min-height: 30px;}"
                              "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:#32332c;border: 2px solid #272822;border-radius: 5px 5px 5px 5px; min-height: 30px;}");

    ui->tabWidget->setStyleSheet("QPlainTextEdit { background-color: #272822; color:#e0e0e0; border: 0px; selection-background-color: #404f4f; font-family: \"Anonymous Pro\"; font-size:11pt;} "
            "QScrollBar:vertical{background: #272822;} QScrollBar:horizontal{background: #272822;}"
            "QTabBar::tab:selected{color: white; border-bottom: 3px solid #2F4F4F;}"
            "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; margin: 0 -2px;padding: 1px 5px; background-color: #262626; border-bottom: 3px solid #212121;}");
    ui->listWidget->setStyleSheet("background-color: #32332c; margin-top: 28px; padding-left: 5px; color: #839496; border: none; font: 11pt \"Anonymous Pro\";");
    ui->fileOverview->setStyleSheet("font-family: \"Anonymous Pro\"; font-size: 1pt; color: rgb(255, 255, 255); margin-top: 28px; background-color: #32332c; border: 0px;");

    updateHighlighterTheme();

    lineColor = QColor(50, 51, 44);
    highlightCurrentLine();

}

void MainWindow::on_actionSolarized_triggered()
{
    theme = "solarized";
    MainWindow::setStyleSheet("QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #404f4f; border-radius: 3px 3px 3px 3px;}"
                              "QMenuBar::item {background:#262626;} QMenuBar::item:selected {background: #232629;}"
                              "QMessageBox {color:white;}"
                              "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
                              "QScrollBar::sub-page:vertical {background: #fdf6e3;} QScrollBar::add-page:vertical {background: #fdf6e3;}"
                              "QScrollBar::sub-page:horizonal {background: #fdf6e3;} QScrollBar::add-page:horizontal {background: #fdf6e3;}"
                              "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:#ede7d5;border: 2px solid #fdf6e3;border-radius: 5px 5px 5px 5px; min-height: 30px;}"
                              "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:#ede7d5;border: 2px solid #fdf6e3;border-radius: 5px 5px 5px 5px; min-height: 30px;}");

    ui->tabWidget->setStyleSheet("QPlainTextEdit { background-color: #fdf6e3; color:#839496; border: 0px; selection-background-color: #404f4f; font-family: \"Anonymous Pro\"; font-size:11pt;} "
                                 "QScrollBar:vertical{background: #fdf6e3;} QScrollBar:horizontal{background: #fdf6e3;}"
                                 "QTabBar::tab:selected{color: white; border-bottom: 3px solid #2F4F4F;}"
                                 "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; margin: 0 -2px;padding: 1px 5px; background-color: #262626; border-bottom: 3px solid #212121;}");
    ui->listWidget->setStyleSheet("background-color: #eee8d5; margin-top: 28px; padding-left: 5px; color: #839496; border: none; font: 11pt \"Anonymous Pro\";");
    ui->fileOverview->setStyleSheet("font-family: \"Anonymous Pro\"; font-size: 1pt; color: rgb(255, 255, 255); margin-top: 28px; background-color: #eee8d5; border: 0px; color:#212121");

    updateHighlighterTheme();

    lineColor = QColor(238, 232, 213);
    highlightCurrentLine();
}

void MainWindow::on_actionSolarized_Dark_triggered()
{
    theme = "solarizedDark";
    MainWindow::setStyleSheet("QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #404f4f; border-radius: 3px 3px 3px 3px;}"
                              "QMenuBar::item {background:#262626;} QMenuBar::item:selected {background: #232629;}"
                              "QMessageBox {color:white;}"
                              "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
                              "QScrollBar::sub-page:vertical {background: #002b36;} QScrollBar::add-page:vertical {background: #002b36;}"
                              "QScrollBar::sub-page:horizonal {background: #002b36;} QScrollBar::add-page:horizontal {background: #002b36;}"
                              "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:#073642;border: 2px solid #002b36;border-radius: 5px 5px 5px 5px; min-height: 30px;}"
                              "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:#073642;border: 2px solid #002b36;border-radius: 5px 5px 5px 5px; min-height: 30px;}");

    ui->tabWidget->setStyleSheet("QPlainTextEdit { background-color: #002b36; color:#839496; border: 0px; selection-background-color: #404f4f; font-family: \"Anonymous Pro\"; font-size:11pt;} "
                                 "QScrollBar:vertical{background: #002b36;} QScrollBar:horizontal{background: #002b36;}"
                                 "QTabBar::tab:selected{color: white; border-bottom: 3px solid #2F4F4F;}"
                                 "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; margin: 0 -2px;padding: 1px 5px; background-color: #262626; border-bottom: 3px solid #212121;}");
    ui->listWidget->setStyleSheet("background-color: #073642; margin-top: 28px; padding-left: 5px; color: #839496; border: none; font: 11pt \"Anonymous Pro\";");
    ui->fileOverview->setStyleSheet("font-family: \"Anonymous Pro\"; font-size: 1pt; color: rgb(255, 255, 255); margin-top: 28px; background-color: #073642; border: 0px;");

    updateHighlighterTheme();

    lineColor = QColor(7, 54, 66);
    highlightCurrentLine();
}

void MainWindow::on_actionTommorrow_triggered()
{
    theme = "tomorrow";
    MainWindow::setStyleSheet("QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #404f4f; border-radius: 3px 3px 3px 3px;}"
                              "QMenuBar::item {background:#262626;} QMenuBar::item:selected {background: #232629;}"
                              "QMessageBox {color:white;}"
                              "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
                              "QScrollBar::sub-page:vertical {background: #ffffff;} QScrollBar::add-page:vertical {background: #ffffff;}"
                              "QScrollBar::sub-page:horizonal {background: #ffffff;} QScrollBar::add-page:horizontal {background: #ffffff;}"
                              "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:#efefef;border: 2px solid #ffffff;border-radius: 5px 5px 5px 5px; min-height: 30px;}"
                              "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:#efefef;border: 2px solid #ffffff;border-radius: 5px 5px 5px 5px; min-height: 30px;}");

    ui->tabWidget->setStyleSheet("QPlainTextEdit { background-color: #ffffff; color:#4d4d4c; border: 0px; selection-background-color: #404f4f; font-family: \"Anonymous Pro\"; font-size:11pt;} "
                                 "QScrollBar:vertical{background: #ffffff;} QScrollBar:horizontal{background: #ffffff;}"
                                 "QTabBar::tab:selected{color: white; border-bottom: 3px solid #2F4F4F;}"
                                 "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; margin: 0 -2px;padding: 1px 5px; background-color: #262626; border-bottom: 3px solid #212121;}");
    ui->listWidget->setStyleSheet("background-color: #efefef; margin-top: 28px; padding-left: 5px; color: #4d4d4c; border: none; font: 11pt \"Anonymous Pro\";");
    ui->fileOverview->setStyleSheet("font-family: \"Anonymous Pro\"; font-size: 1pt; color: #4d4d4c; margin-top: 28px; background-color: #efefef; border: 0px;");

    updateHighlighterTheme();

    lineColor = QColor(237, 237, 237);
    highlightCurrentLine();
}

void MainWindow::on_actionTommorrow_Night_triggered()
{
    theme = "tomorrowNight";
    MainWindow::setStyleSheet("QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #404f4f; border-radius: 3px 3px 3px 3px;}"
                              "QMenuBar::item {background:#262626;} QMenuBar::item:selected {background: #232629;}"
                              "QMessageBox {color:white;}"
                              "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
                              "QScrollBar::sub-page:vertical {background: #1d1f21;} QScrollBar::add-page:vertical {background: #1d1f21;}"
                              "QScrollBar::sub-page:horizonal {background: #1d1f21;} QScrollBar::add-page:horizontal {background: #1d1f21;}"
                              "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:#262626;border: 2px solid #1d1f21;border-radius: 5px 5px 5px 5px; min-height: 30px;}"
                              "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:#262626;border: 2px solid #1d1f21;border-radius: 5px 5px 5px 5px; min-height: 30px;}");

    ui->tabWidget->setStyleSheet("QPlainTextEdit { background-color: #1d1f21; color:#c5c8c6; border: 0px; selection-background-color: #404f4f; font-family: \"Anonymous Pro\"; font-size:11pt;} "
                                 "QScrollBar:vertical{background: #1d1f21;} QScrollBar:horizontal{background: #1d1f21;}"
                                 "QTabBar::tab:selected{color: white; border-bottom: 3px solid #2F4F4F;}"
                                 "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; margin: 0 -2px;padding: 1px 5px; background-color: #262626; border-bottom: 3px solid #212121;}");
    ui->listWidget->setStyleSheet("background-color: #282a2e; margin-top: 28px; padding-left: 5px; color: #c5c8c6; border: none; font: 11pt \"Anonymous Pro\";");
    ui->fileOverview->setStyleSheet("font-family: \"Anonymous Pro\"; font-size: 1pt; color: #c5c8c6; margin-top: 28px; background-color: #282a2e; border: 0px;");

    updateHighlighterTheme();

    lineColor = QColor(40, 42, 46);
    highlightCurrentLine();
}

/*
 *****************************
 *         Window            *
 *****************************
*/

//  Highlight Current Line
void MainWindow::highlightCurrentLine(){
   QList<QTextEdit::ExtraSelection> extraSelections;

   QTextEdit::ExtraSelection selections;
   selections.format.setBackground(lineColor);
   selections.format.setProperty(QTextFormat::FullWidthSelection, true);
   selections.cursor = p->textCursor();
   selections.cursor.clearSelection();
   extraSelections.append(selections);

   p->setExtraSelections(extraSelections);
   ui->listWidget->verticalScrollBar()->setValue(p->verticalScrollBar()->value());
}

//  Line nums Block count
void MainWindow::updateLineNums(int newBlockCount){

    //  add remove line numbers
    if(newBlockCount > numBlocks){

        while(numBlocks < newBlockCount){
            numBlocks++;
            ostringstream oss;
            oss << numBlocks;
            ui->listWidget->addItem( QString::fromStdString(oss.str()) );
            QListWidgetItem *item = ui->listWidget->item(numBlocks-1);
            item->setSizeHint(QSize(item->sizeHint().height(), 14));
        }

    }else{

        while(numBlocks > newBlockCount){
            ostringstream oss;
            oss << numBlocks;
            ui->listWidget->takeItem(numBlocks - 1);
            numBlocks--;
        }
    }

    ostringstream oss;
    oss << newBlockCount << " Lines";
    ui->label->setText(QString::fromStdString(oss.str()));
}
void MainWindow::onBlockCountChanged(int newBlockCount)
{
    MainWindow::updateLineNums(newBlockCount);

    //  auto-indent
    QString data = p->toPlainText();
    int cursorPosition = p->textCursor().position();
    int i;

    for (i=cursorPosition-2; i>=0; i--){

        if (data.mid(i,1) == "\n") {
            break;
            }
        }

        while (data.mid(i+1,1) == "\t"){
            p->textCursor().insertText("\t");
            i++;
        }
}

//  Text Changed
void MainWindow::onTextChanged(){

    ui->textBrowser->setText("");
    ui->fileOverview->setPlainText(p->toPlainText());
    currentSearchTerm = "";
}

// Sync overview scrolling with editor scrolling
void MainWindow::scrollOverview(int scrollValue){
    ui->fileOverview->verticalScrollBar()->setValue(scrollValue * 2.5);
}

//  Close program
void MainWindow::on_actionExit_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Test", "Quit?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
      QApplication::quit();
    } else {

    }
}

// Close all tabs
void MainWindow::on_actionClose_All_triggered()
{
    while(ui->tabWidget->count() > 1){
        on_tabWidget_tabCloseRequested(0);
    }
    on_tabWidget_tabCloseRequested(0);
}

//  Display about info
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About Geist"), *licencePointer,QMessageBox::Close);
}

// Set tab stop width
void MainWindow::setTabWidth(int width){
    QFont font;
    font.setFamily("Anonymous Pro");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    QFontMetrics metrics(font);

    int current = ui->tabWidget->currentIndex();

    for(int i = 0; i < ui->tabWidget->count(); i++){
        ui->tabWidget->setCurrentIndex(i);
        p->setTabStopWidth(width * metrics.width(' ') );
    }

    ui->tabWidget->setCurrentIndex(current);

}

void MainWindow::on_action8_triggered()
{
    tabWidth = 8;
    setTabWidth(8);
}

void MainWindow::on_action4_triggered()
{
    tabWidth = 4;
    setTabWidth(4);
}

void MainWindow::on_action2_triggered()
{
    tabWidth = 2;
    setTabWidth(2);
}

void MainWindow::openWith(QString file){
    open(file);
}
