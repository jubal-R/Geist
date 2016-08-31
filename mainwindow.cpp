/*
By Jubal - Geist - All purpose text editor
    Copyright (C) 2016  Jubal
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filelist.h"
#include "files.h"
#include "conversion.h"
#include "snippets.h"
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


QPlainTextEdit *p;

Conversion conversion;
FileList filelist;
Files files;
Snippets snippets;
Runner runner;
Templates templates;

QString currentSearchTerm;
QStringList foundPositions;
QStringList settings;
QString currentDirectory = QString::fromStdString(files.getHomeDir());    //  Directory of last opened file. Users home folder by default.

QString licence = "Project Page: https://github.com/jubal-R/Geist\n\n"
        "By Jubal - Geist - All purpose text editor\n"
        "Copyright (C) 2016  Jubal\n\n";
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

    outputModeP = &outputMode; //  0 = ascii, 1 = hex, 2 = strings

    filename = "";  // Name of file in current tab

    // UI Setup
    ui->setupUi(this);

    //  CSS
    this->setWindowTitle("Geist");
    MainWindow::setStyleSheet("QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #303030;border-radius: 3px 3px 3px 3px;}"
                              "QMenuBar::item {background:#212121;} QMenuBar::item:selected {background: #232629;}"
                              "QMessageBox {color:white;}"
                              "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
                              "QScrollBar::sub-page:vertical {background: #333333;} QScrollBar::add-page:vertical {background: #333333;}"
                              "QScrollBar::sub-page:horizonal {background: #333333;} QScrollBar::add-page:horizontal {background: #333333;}"
                              "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:#1d1f21;border: 2px solid #333333;border-radius: 5px 5px 5px 5px; min-height: 30px;}"
                              "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:#1d1f21;border: 2px solid #333333;border-radius: 5px 5px 5px 5px; min-height: 30px;}");


    ui->tabWidget->setStyleSheet("QPlainTextEdit { background-color: #1d1f21; color:white; border: 0px; selection-background-color: #404f4f; font-family: \"Liberation Mono\"; font-size:10pt;} "
                                 "QScrollBar:vertical{background: #333333;} QScrollBar:horizontal{background: #333333;}"
                                 "QTabBar::tab:selected{color: white; background: #1d1f21; border-bottom: 2px solid #2F4F4F;}"
                                 "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; margin: 0 -2px;padding: 1px 5px; background-color: #212121;}");

    ui->menuBar->setStyleSheet("color:white; background-color:#212121; QMenuBar::item {background:black;}");
    ui->centralWidget->layout()->setContentsMargins(0,0,0,0);


    //  Setup window based on user's settings
    string set = files.read("settings.txt");
    settings = QString::fromStdString(set).split("\n");
    if (settings.size() > 3){
        MainWindow::resize(settings.at(0).toInt(), settings.at(1).toInt());
        ui->fileOverview->setMaximumWidth(settings.at(2).toInt());
    }

    //  Disbale manual scrolling for line numbers
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create and setup initial tab
    newTab();
    connect(p, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    p->setFocus();
//    highlighter = new Highlighter(ui->fileOverview->document());    // Set highlighter for file overview
    highlightCurrentLine();

    ui->listWidget->addItem("1");
    QListWidgetItem *item = ui->listWidget->item(numBlocks-1);
    item->setSizeHint(QSize(item->sizeHint().height(), 14));

}

MainWindow::~MainWindow()
{
    /*  Save settings
    *   Window size, overview toggle values
    */
    if (settings.at(0).toInt() != MainWindow::width() || settings.at(1).toInt() != MainWindow::height() || settings.at(2).toInt() != ui->fileOverview->maximumWidth()){
        ostringstream oss;
        oss << MainWindow::width() << "\n" << MainWindow::height() << "\n" << ui->fileOverview->width();
        files.write("settings.txt", oss.str());
    }

    delete ui;
}



//  Highlight Current Line
void MainWindow::highlightCurrentLine(){
   QList<QTextEdit::ExtraSelection> extraSelections;

   QTextEdit::ExtraSelection selections;
   QColor lineColor = QColor(37, 40, 43);
   selections.format.setBackground(lineColor);
   selections.format.setProperty(QTextFormat::FullWidthSelection, true);
   selections.cursor = p->textCursor();
   selections.cursor.clearSelection();
   extraSelections.append(selections);

   p->setExtraSelections(extraSelections);
   ui->listWidget->verticalScrollBar()->setValue(p->verticalScrollBar()->value());
}

//  Tabs

// Switch tab
void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (p != 0){
        disconnect(p, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
        disconnect(p, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    }

    p = qobject_cast<QPlainTextEdit *>(ui->tabWidget->widget(index));
    newNumBlocks = p->document()->blockCount();
    QObject::connect(p->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->listWidget->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(p->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollOverview(int)));
    connect(p, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    MainWindow::updateLineNums(newNumBlocks);
    connect(p, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
    connect(p, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    ui->fileOverview->setPlainText(p->toPlainText());
    filename = ui->tabWidget->tabToolTip(ui->tabWidget->currentIndex());
    ui->labelFileType->setText(getFileType(filename));
    foundPositions.clear();
    foundPosElement = 0;
}

//  Close tab
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (ui->tabWidget->count() > 1){

    }else{
        newTab();
    }

        // Set the new current tab
        if (index > 0){
            ui->tabWidget->setCurrentWidget(ui->tabWidget->widget(index - 1));
        }else{
            ui->tabWidget->setCurrentWidget(ui->tabWidget->widget(index + 1));
        }

        // Delete filelist data
        filelist.removeNode(ui->tabWidget->tabToolTip(index).toStdString());

        // Delete the tab
        delete ui->tabWidget->widget(index);

        // Update filename to correspond to new current tab
        filename = ui->tabWidget->tabToolTip(ui->tabWidget->currentIndex());

}

//  Create new tab
void MainWindow::newTab(){
    if (ui->tabWidget->count() < 50){
        ui->tabWidget->addTab(new QPlainTextEdit, "New File");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), "");
        filename = "";

        QFont font;
        font.setFamily("DejaVu Sans Mono");
        font.setStyleHint(QFont::Monospace);
        font.setFixedPitch(true);
        font.setPointSize(9);
        p->setFont(font);
        QFontMetrics metrics(font);

        p->setTabStopWidth(4 * metrics.width(' ') );
        p->setWordWrapMode(QTextOption::NoWrap);
    }
}
void MainWindow::on_actionNew_triggered()
{
    MainWindow::newTab();
}

QString MainWindow::getFileType(QString file){
    QStringList sl = file.split(".");
    if(sl.length() > 1)
        return sl.last();
    else
        return "";
}

/*  Open file
*   Opens in new tab unless current tab is empty
*   If file does not exist it will be created on save
*/
void MainWindow::open(){
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), currentDirectory, tr("All (*)"));

    if (file != ""){

        if(filename != "" || p->toPlainText() != ""){
            newTab();
        }

        filename = file;
        QString filetype = getFileType(file);
        p->setPlainText(QString::fromStdString(files.read(filename.toStdString())));

        if (filename.length() > 22){
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), filename.right(22));
        }else{
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), filename);
        }
        // Store file path in tool tip
        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), filename);
        ui->labelFileType->setText(filetype);

        // Add file info to filelist
        node * fileNode = new node;
        fileNode->filepath = file.toStdString();
        fileNode->filetype = filetype.toStdString();
        fileNode->highlighter = new Highlighter(filetype, p->document());
        filelist.insertNode(fileNode);

        ui->textBrowser->setText("");
        *outputModeP = 0;
        currentDirectory = QString::fromStdString(getDirectory());
    }
}
void MainWindow::on_actionOpen_triggered()
{
    open();
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


        // Add info to filelist
        QString currentFile = ui->tabWidget->tabToolTip(ui->tabWidget->currentIndex());
        if(currentFile == ""){
            // Add file info to filelist
            node * fileNode = new node;
            fileNode->filepath = filename.toStdString();
            fileNode->filetype = filetype.toStdString();
            fileNode->highlighter = new Highlighter(filetype, p->document());
            filelist.insertNode(fileNode);
        }else{
            filelist.setFilepath(currentFile.toStdString(), filename.toStdString());
        }

        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), filename.right(20));
        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), filename);
        ui->labelFileType->setText(filetype);

        ui->textBrowser->setText("Saved");
       }
}

/*  Run program (in gnome terminal)
*   If html opens in default browser instead
*   Supported languages: html, perl, python, ruby, sh
*/
void MainWindow::run(bool sudo){
    ostringstream oss;

    if (sudo){
        oss << "sudo ";
    }

    QStringList fileExtension = filename.split(".");
    int len = fileExtension.length();

    if (fileExtension[len - 1] == "py"){
        oss << filename.toStdString();
        runner.runGnomeTerminal(oss.str());

    }else if (fileExtension[len - 1] == "rb"){
        oss << "ruby " << filename.toStdString();
        runner.runGnomeTerminal(oss.str());
    }else if (fileExtension[len - 1] == "pl"){
        oss << "perl " << filename.toStdString();
        runner.runGnomeTerminal(oss.str());
    }else if (fileExtension[len - 1] == "sh"){
        oss << filename.toStdString();
        runner.runGnomeTerminal(oss.str());

    }else if (fileExtension[len - 1] == "html"){
        oss << "xdg-open " << filename.toStdString();
        ui->textBrowser->setPlainText(QString::fromStdString(runner.run(oss.str())));

    }else{

    }

}
void MainWindow::on_actionRun_in_xterm_triggered()
{
    MainWindow::run(false);
}
void MainWindow::on_actionRun_triggered()
{
    MainWindow::run(true);
}

//  Search find

// Toggle find bar
void MainWindow::on_actionFind_triggered()
{
    if (ui->findLineEdit->maximumWidth() < 300){
        ui->findReplaceBar->setMaximumHeight(30);
        ui->findLineEdit->setMaximumWidth(300);
        ui->findPrevButton->setMaximumWidth(150);
        ui->findButton->setMaximumWidth(150);

        if (ui->label_2->width() > 0){
            ui->label_2->setMaximumWidth(0);
            ui->label_3->setMaximumWidth(0);
            ui->replaceLineEdit->setMaximumWidth(0);
            ui->replaceButton->setMaximumWidth(0);
            ui->replaceAllButton->setMaximumWidth(0);
        }
        ui->findLineEdit->setFocus();
        ui->findLineEdit->selectAll();
    }else{
        ui->findLineEdit->setMaximumWidth(0);
        ui->findPrevButton->setMaximumWidth(0);
        ui->findButton->setMaximumWidth(0);
        ui->findReplaceBar->setMaximumHeight(0);
        p->setFocus();
    }
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
    int width = 0;

    if (ui->replaceLineEdit->width() == 0){
        width = 150;
        ui->findReplaceBar->setMaximumHeight(30);
        ui->findLineEdit->setFocus();
    }else{
        ui->findReplaceBar->setMaximumHeight(0);
        p->setFocus();
    }

    ui->findLineEdit->setMaximumWidth(width);
    ui->label_2->setMaximumWidth(width);
    ui->label_3->setMaximumWidth(width);
    ui->replaceLineEdit->setMaximumWidth(width);
    ui->replaceButton->setMaximumWidth(width);
    ui->replaceAllButton->setMaximumWidth(width);
    ui->findPrevButton->setMaximumWidth(width);
    ui->findButton->setMaximumWidth(width);
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

//  Run chmod +x on current file
void MainWindow::on_actionChmodx_triggered()
{
    ostringstream oss;
    oss << "chmod +x " << filename.toStdString();
    string output = runner.run(oss.str());

    if (output == ""){
        ui->textBrowser->setPlainText("+x permission granted.");
    }else{
        ui->textBrowser->setPlainText(QString::fromStdString(output));
       }
}

//  undo redo
void MainWindow::on_actionUndo_triggered()
{
    p->undo();
}
void MainWindow::on_actionRedo_triggered()
{
    p->redo();
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

//  Display about info
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("My App"), *licencePointer,QMessageBox::Close);
}

/*  File Templates
*   If current tab is empty fill it with the selected code template
*/
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
void MainWindow::on_actionPhp_triggered(){ MainWindow::getTemp(5);}
void MainWindow::on_actionCss_triggered(){ MainWindow::getTemp(6);}

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
    oss << " Lines: " << newBlockCount << "\t";
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


