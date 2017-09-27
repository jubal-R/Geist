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
#include "search.h"
#include "conversion.h"
#include "geisttextedit.h"
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


// Pointer To GeistTextEdit Widget
GeistTextEdit *p = NULL;

Conversion conversion;
Files files;
Search searcher;
Templates templates;

QString currentSearchTerm;  // Current Value Of Search Term
QStringList foundPositions; // Positions Of Substrings Matching Search Term
QString currentDirectory = files.getHomeDir();  //  Directory of last opened file. Users home folder by default.

QString licence = "Project Page: https://github.com/jubal-R/Geist\n\n"
        "Geist - All purpose text/code editor\n"
        "Version: 1.0\n"
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
    /* Initialize Variables */
    numBlocks = 1;
    newNumBlocks = 0;
    outputMode = 0;
    foundPosElement = 0;
    searchTermLen = 0;

    outputModeP = &outputMode;  //  0 = ascii, 1 = hex, 2 = strings

    filename = "";              // Name Of File In Current Tab
    lineColor = QColor(7, 54, 66);

    // UI Setup
    ui->setupUi(this);

    // Style Sheet
    this->setWindowTitle("Geist");
    ui->menuBar->setStyleSheet("color:white; background-color:#262626; QMenuBar::item {background:black;}");
    ui->centralWidget->layout()->setContentsMargins(0,0,0,0);

    // Disbale manual scrolling for line numbers
    ui->listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Hide Search Bar By Default
    ui->findReplaceBar->hide();
    ui->replaceLineEdit->hide();
    ui->replaceButton->hide();
    ui->replaceAllButton->hide();
    ui->label_3->hide();

    // Set Theme From Settings
    theme = settings.value("theme", "monokai").toString();
    if (theme == "monokai"){
        on_actionDark_triggered();
    }else if (theme == "solarized"){
        on_actionSolarized_triggered();
    }else if (theme == "solarizedDark"){
        on_actionSolarized_Dark_triggered();
    }else if (theme == "tomorrow"){
        on_actionTommorrow_triggered();
    }else if (theme == "tomorrowNight"){
        on_actionTommorrow_Night_triggered();
    }

    // Create And Setup Initial Tab
    newTab();
    connect(p, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    p->setFocus();
    highlightCurrentLine();

    ui->listWidget->addItem("1");
    QListWidgetItem *item = ui->listWidget->item(numBlocks-1);
    item->setSizeHint(QSize(item->sizeHint().height(), 14));

    // Setup Window Based On User's Settings

    // Set Window Size
    int windowWidth = settings.value("windowWidth", 1000).toInt();
    int windowHeight = settings.value("windowHeight", 600).toInt();
    MainWindow::resize(windowWidth, windowHeight);

    // Show/Hide Overview
    if(!settings.value("showOverview", true).toBool()){
        ui->fileOverview->hide();
    }

    // Open Files From List Saved In Settings
    QStringList openFiles;
    openFiles = settings.value("openFilesList").toStringList();

    if (!openFiles.isEmpty()){
        for (int i = 0; i < openFiles.length(); i++){
            open(openFiles.at(i));
        }
    }

}

MainWindow::~MainWindow()
{
    // Get Window Size
    QRect windowRect = MainWindow::normalGeometry();
    settings.setValue("windowWidth", windowRect.width());
    settings.setValue("windowHeight", windowRect.height());


    // Get List Of Files Open In Editor
    QStringList fileList;
    int numOpenFiles = ui->tabWidget->count();
    for (int i = 0; i < numOpenFiles; i++){
        fileList.append(ui->tabWidget->tabToolTip(i));
    }
    settings.setValue("openFilesList", fileList);

    delete ui;
}

/*
 *****************************
 *         Tabs              *
 *****************************
*/

// Switch Tab
void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (p != 0){
        disconnect(p, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
        disconnect(p, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    }

    p = qobject_cast<GeistTextEdit *>(ui->tabWidget->widget(index));
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

// Close Tab
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

// Create New Tab
void MainWindow::newTab(){
    if (ui->tabWidget->count() < 50){
        ui->tabWidget->addTab(new GeistTextEdit, "New File");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
        ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(), "");
        filename = "";

        setTabWidth(settings.value("tabWidth", 4).toInt());
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

/*
 *  Open file
 *  Opens in new tab unless current tab is empty
 *  If file does not exist it will be created on save
*/
void MainWindow::open(QString file){

    if (file != ""){

        if(filename != "" || p->toPlainText() != ""){
            newTab();
        }

        filename = file;
        QString filetype = getFileType(file);
        p->setPlainText(files.read(filename));

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
        currentDirectory = getDirectory();
    }
}
void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), currentDirectory, tr("All (*)"));
    open(file);
}

// Save File
void MainWindow::save(){
    if (filename == "" || *outputModeP != 0){
        on_actionSave_as_triggered();
    }else{
        files.write(filename, p->toPlainText());
        ui->textBrowser->setText("Saved");
    }
}
void MainWindow::on_actionSave_triggered()
{
    MainWindow::save();
}
void MainWindow::on_actionSave_as_triggered()
{
    filename = QFileDialog::getSaveFileName(this, tr("Save As"), currentDirectory, tr("All (*)"));
    QString filetype = getFileType(filename);

    if (filename != ""){
        files.write(filename, p->toPlainText());

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

// Toggle Find Bar
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

// Find All Instances Of Search Term And Cycles Through Them
void MainWindow::on_findLineEdit_returnPressed()
{
    if (ui->findLineEdit->text() == currentSearchTerm){
            findNext();

    }else{
        QString searchTerm = ui->findLineEdit->text();
        currentSearchTerm = searchTerm;
        searchTermLen = searchTerm.length();

        if (searchTermLen > 0){
            QString found = QString::fromStdString(searcher.findAll(searchTerm.toStdString(), p->toPlainText().toStdString()));
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

// Selects Text Given the Position Of the First Char And It's Length
void MainWindow::selectText(int pos, int len){
    if (foundPositions.length() > 1){
        QTextCursor cur = p->textCursor();
        cur.setPosition(pos , QTextCursor::MoveAnchor);
        cur.setPosition(pos + len, QTextCursor::KeepAnchor);
        p->setTextCursor(cur);
        ui->listWidget->verticalScrollBar()->setValue(p->verticalScrollBar()->value());
    }
}

//  Find Next Instance Of Search Term
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

// Find Previous Instance Of Search Term
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

//  Toggle Replace Bar
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

// Replace Currently Selected Found Instance Of Text To Be Replaced
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
        QString found = QString::fromStdString(searcher.findAll(searchTerm.toStdString(), p->toPlainText().toStdString()));
        foundPositions = found.split(" ");
        foundPosElement = 0;
        selectText(foundPositions[foundPosElement].toInt(), searchTermLen);
    }
}
void MainWindow::on_replaceLineEdit_returnPressed()
{
    MainWindow::on_replaceButton_clicked();
}

//  Replace All Found Instances Of Text To Be Replaced
void MainWindow::on_replaceAllButton_clicked()
{
    QString searchTerm = ui->findLineEdit->text();
    currentSearchTerm = searchTerm;
    do{
        searchTermLen = searchTerm.length();

        if (searchTermLen > 0){
            QString found = QString::fromStdString(searcher.findAll(searchTerm.toStdString(), p->toPlainText().toStdString()));
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

// Go To Line Number
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

//  Undo/Redo
void MainWindow::on_actionUndo_triggered()
{
    p->undo();
}
void MainWindow::on_actionRedo_triggered()
{
    p->redo();
}

//  Delete Line Where Cusor Currently Resides
void MainWindow::on_actionDelete_line_triggered()
{
    p->deleteLine();
}

//  Delete Word Where Cusor Currently Resides
void MainWindow::on_actionRemove_word_triggered()
{
    p->deleteWord();
}

/*
 *  Toggles currently selected line(s) between commented out and uncommented
 *  If no text is selected then it selects the current line
*/
void MainWindow::on_actionToggle_comment_triggered()
{
    p->toggleComment();
}

// Join Lines
void MainWindow::on_actionJoin_Lines_triggered()
{
    p->joinLines();
}

// Swap Line Up
void MainWindow::on_actionMove_Line_Up_triggered()
{
    p->swapLineUp();
}

// Swap Line Down
void MainWindow::on_actionSwap_line_down_triggered()
{
    p->swapLineDown();
}

/*
 *****************************
 *         Tools             *
 *****************************
*/

// Strips All Nonprintable ascii Characters From Contents Of Current Tab
void MainWindow::on_actionStrings_triggered()
{
    if (*outputModeP != 1){
        *outputModeP = 2;
        p->setPlainText( QString::fromStdString(conversion.getStrings(p->toPlainText().toStdString())) );
    }
}
//  Converts ascii Values Of Content Of Current Tab Into Hexidecimal
void MainWindow::on_actionHex_triggered()
{
    if (*outputModeP == 0 || *outputModeP == 2){
        *outputModeP = 1;
        p->setPlainText( QString::fromStdString( conversion.hex(p->toPlainText().toStdString() ) ) );
    }
}
//  Converts Hexidecimal Values Of Content Of Current Tab Into ascii
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

// Apply Code Template Given Template Id
void MainWindow::getTemp(int num){

    // Check If Tab Contents Empty
    if (p->toPlainText() == ""){
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

// Get Directory Of File Open In Current Tab
QString MainWindow::getDirectory(){
    QString filepath = filename;
    int lastIndex = filepath.lastIndexOf("/");
    filepath.chop(filepath.length() - lastIndex);

    return filepath;
}

/*
 *****************************
 *         View              *
 *****************************
*/

// Toggle FullScreen
void MainWindow::on_actionFullScreen_triggered()
{
    if(MainWindow::isFullScreen())
    {
        MainWindow::showNormal();
    }else{
        MainWindow::showFullScreen();
    }
}

// Toggle File Overview
void MainWindow::on_actionOverview_triggered()
{
    if(ui->fileOverview->isHidden()){
        ui->fileOverview->show();
        settings.setValue("showOverview", true);
    }else{
        ui->fileOverview->hide();
        settings.setValue("showOverview", false);
    }
}

// Toggle Menubar
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

void MainWindow::setMainWindowStyle(QString backgroundColor, QString lineColor){
    string bgc = backgroundColor.toStdString();
    string lc = lineColor.toStdString();
    ostringstream style;
    style << "QMenu {background-color: rgb(48, 47, 54); color:white; selection-background-color: #404f4f;border: 1px solid #404f4f; border-radius: 3px 3px 3px 3px;}"
             << "QMenuBar::item {background:#262626;} QMenuBar::item:selected {background: #232629;}"
             << "QMessageBox {color:white;}"
             << "QLineEdit {border: 1px solid #676767; border-radius: 5px 5px 5px 5px;}"
             << "QScrollBar::sub-page:vertical {background: "<< bgc <<";} QScrollBar::add-page:vertical {background: "<< bgc <<";}"
             << "QScrollBar::sub-page:horizonal {background: "<< bgc <<";} QScrollBar::add-page:horizontal {background: "<< bgc <<";}"
             << "QScrollBar:vertical{background: white;width:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:vertical {background:"<< lc <<";border: 2px solid "<< bgc <<";border-radius: 5px 5px 5px 5px; min-height: 30px;}"
             << "QScrollBar:horizontal{background: white;height:12px;margin: 0px 0px 0px 0px;} QScrollBar::handle:horizontal {background:"<< lc <<";border: 2px solid "<< bgc <<";border-radius: 5px 5px 5px 5px; min-height: 30px;}";

    MainWindow::setStyleSheet(QString::fromStdString(style.str()));

}

void MainWindow::setTabWidgetStyle(QString foregroundColor, QString backgroundColor){
    string fgc = foregroundColor.toStdString();
    string bgc = backgroundColor.toStdString();
    ostringstream style;
    style << "QPlainTextEdit { background-color: "<< bgc <<"; color:"<< fgc <<"; border: 0px; selection-background-color: #404f4f; font-family: \"Anonymous Pro\"; font-size:11pt;} "
             << "QScrollBar:vertical{background: "<< bgc <<";} QScrollBar:horizontal{background: "<< bgc <<";}"
             << "QTabBar::tab:selected{color: white; border-bottom: 3px solid #2F4F4F;}"
             << "QTabBar::tab {height: 22px; width: 160px; color: #676767; font-size:9pt; margin: 0 -2px;padding: 1px 5px; background-color: #262626; border-bottom: 3px solid #212121;}";
   ui->tabWidget->setStyleSheet(QString::fromStdString(style.str()));

}

void MainWindow::setLineNumStyle(QString lineColor, QString foregroundColor){
    string lc = lineColor.toStdString();
    string fgc = foregroundColor.toStdString();
    ostringstream style;
    style << "background-color: "<< lc <<"; margin-top: 28px; padding-left: 5px; color: "<< fgc <<"; border: none; font: 11pt \"Anonymous Pro\"; ";
    ui->listWidget->setStyleSheet(QString::fromStdString(style.str()));
}

void MainWindow::setOverViewStyle(QString lineColor, QString foregroundColor){
    string lc = lineColor.toStdString();
    string fgc = foregroundColor.toStdString();
    ostringstream style;
    style << "font-family: \"Anonymous Pro\"; font-size: 1pt; color:"<< fgc <<"; margin-top: 28px; background-color:"<< lc <<"; border: 0px;";
    ui->fileOverview->setStyleSheet(QString::fromStdString(style.str()));
}

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
    settings.setValue("theme", "monokai");
    QString fgc = "#e0e0e0";
    QString bgc = "#272822";
    QString lc = "#32332c";

    setMainWindowStyle(bgc, lc);
    setTabWidgetStyle(fgc, bgc);
    setLineNumStyle(lc, fgc);
    setOverViewStyle(lc, fgc);

    updateHighlighterTheme();

    lineColor = QColor(50, 51, 44);
    highlightCurrentLine();

}

void MainWindow::on_actionSolarized_triggered()
{
    settings.setValue("theme", "solarized");
    QString fgc = "#839496";
    QString bgc = "#fdf6e3";
    QString lc = "#eee7d5";

    setMainWindowStyle(bgc, lc);
    setTabWidgetStyle(fgc, bgc);
    setLineNumStyle(lc, fgc);
    setOverViewStyle(lc, fgc);

    updateHighlighterTheme();

    lineColor = QColor(238, 232, 213);
    highlightCurrentLine();
}

void MainWindow::on_actionSolarized_Dark_triggered()
{
    settings.setValue("theme", "solarizedDark");
    QString fgc = "#839496";
    QString bgc = "#002b36";
    QString lc = "#073642";

    setMainWindowStyle(bgc, lc);
    setTabWidgetStyle(fgc, bgc);
    setLineNumStyle(lc, fgc);
    setOverViewStyle(lc, fgc);

    updateHighlighterTheme();

    lineColor = QColor(7, 54, 66);
    highlightCurrentLine();
}

void MainWindow::on_actionTommorrow_triggered()
{
    settings.setValue("theme", "tomorrow");
    QString fgc = "#4d4d4c";
    QString bgc = "#ffffff";
    QString lc = "#efefef";

    setMainWindowStyle(bgc, lc);
    setTabWidgetStyle(fgc, bgc);
    setLineNumStyle(lc, fgc);
    setOverViewStyle(lc, fgc);

    updateHighlighterTheme();

    lineColor = QColor(237, 237, 237);
    highlightCurrentLine();
}

void MainWindow::on_actionTommorrow_Night_triggered()
{
    settings.setValue("theme", "tomorrowNight");
    QString fgc = "#c5c8c6";
    QString bgc = "#1d1f21";
    QString lc = "#282a2e";

    setMainWindowStyle(bgc, lc);
    setTabWidgetStyle(fgc, bgc);
    setLineNumStyle(lc, fgc);
    setOverViewStyle(lc, fgc);

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
   if(p != NULL){
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
}

//  Update Line Numbers
void MainWindow::updateLineNums(int newBlockCount){

    if(newBlockCount > numBlocks){
        //  Add Line Numbers
        while(numBlocks < newBlockCount){
            numBlocks++;
            ostringstream oss;
            oss << numBlocks;
            ui->listWidget->addItem( QString::fromStdString(oss.str()) );
            QListWidgetItem *item = ui->listWidget->item(numBlocks-1);
            item->setSizeHint(QSize(item->sizeHint().height(), 14));
        }

    }else{
        //  Remove Line Numbers
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

    /*  Auto-Indent New Lines   */

    //  If Block Count Only Increased By One
    if(newBlockCount == numBlocks + 1){

        QString data = p->toPlainText();
        int cursorPosition = p->textCursor().position();

        // If Beginning Cursor At Beginning Of New Line
        if (data.mid(cursorPosition-1, 1) == "\n"){
            int pos;

            // Get Position Of Start Of New Line
            for (pos=cursorPosition-2; pos>=0; pos--){

                if (data.mid(pos, 1) == "\n") {
                    break;
                    }
                }

                //  For Each Tab Char At Beggining Of Previous Line: Add Tab To New Line
                while (data.mid(pos + 1, 1) == "\t"){
                    p->textCursor().insertText("\t");
                    pos++;
                }
         }
    }

    // Update Line Numbers
    MainWindow::updateLineNums(newBlockCount);
}

//  Text Changed
void MainWindow::onTextChanged(){

    ui->textBrowser->setText("");
    ui->fileOverview->setPlainText(p->toPlainText());
    currentSearchTerm = "";
}

// Sync Overview Scrolling With Editor Scrolling
void MainWindow::scrollOverview(int scrollValue){
    ui->fileOverview->verticalScrollBar()->setValue(scrollValue * 2.5);
}

//  Close Program
void MainWindow::on_actionExit_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Test", "Quit?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
      QApplication::quit();
    } else {

    }
}

// Close All Tabs
void MainWindow::on_actionClose_All_triggered()
{
    while(ui->tabWidget->count() > 1){
        on_tabWidget_tabCloseRequested(0);
    }
    on_tabWidget_tabCloseRequested(0);
}

//  Display About Info
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About Geist"), *licencePointer,QMessageBox::Close);
}

// Set Tab Stop Width
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
    settings.setValue("tabWidth", 8);
    setTabWidth(8);
}

void MainWindow::on_action4_triggered()
{
    settings.setValue("tabWidth", 4);
    setTabWidth(4);
}

void MainWindow::on_action2_triggered()
{
    settings.setValue("tabWidth", 2);
    setTabWidth(2);
}

void MainWindow::openWith(QString file){
    open(file);
}
