#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <highlighter.h>
#include <QShortcut>
#include <QPlainTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionExit_triggered();

    void on_actionSave_as_triggered();

    void on_actionFind_triggered();

    void on_actionHex_triggered();

    void on_actionAscii_triggered();

    void on_actionChmodx_triggered();

    void on_actionRun_triggered();

    void on_actionStrings_triggered();

    void on_findLineEdit_returnPressed();

    void on_actionAbout_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_tabWidget_currentChanged(int index);

    void onBlockCountChanged(int newBlockCount);

    void updateLineNums(int newBlockCount);

    void onTextChanged();

    void scrollOverview(int scrollValue);

    void on_actionFullScreen_triggered();

    void on_actionGoTo_triggered();

    void save();

    void open();

    void newTab();

    void run(bool sudo);

    void on_actionAsm_triggered();

    void on_actionC_triggered();

    void on_actionCpluspluss_triggered();

    void on_actionHtml_triggered();

    void on_actionJava_triggered();

    void on_actionPhp_triggered();

    void getTemp(int num);

    void on_actionCss_triggered();

    std::string getDirectory();

    void selectText(int pos,int len);

    void on_findButton_clicked();

    void on_findPrevButton_clicked();

    void on_actionFind_Next_triggered();

    std::string find(std::string q, std::string content);

    void findNext();

    void findPrev();

    void on_actionFind_Previous_triggered();

    void on_actionReplace_triggered();

    void on_replaceButton_clicked();

    void selectLine();

    void on_actionDelete_line_triggered();

    void selectWord();

    void on_actionRemove_word_triggered();

    void on_replaceAllButton_clicked();

    void on_replaceLineEdit_returnPressed();

    void on_actionRun_in_xterm_triggered();

    void on_findLineEdit_textChanged(const QString &arg1);

    void on_actionToggle_comment_triggered();

    void on_actionOverview_triggered();

    void highlightCurrentLine();

    void on_actionOpen_containg_folder_triggered();

    QString getFileType(QString file);

    void on_actionDark_triggered();

    void on_actionSolarized_Dark_triggered();

    void on_actionJoin_Lines_triggered();

    void on_actionMove_Line_Up_triggered();

    void on_actionSwap_line_down_triggered();

    void on_actionMenubar_triggered();

private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;
    QShortcut *shortcut;

    int numBlocks;
    int newNumBlocks;
    int outputMode;
    int foundPosElement;
    int searchTermLen;
    int *outputModeP;

    QString filename;
    QString theme;

};

#endif // MAINWINDOW_H
