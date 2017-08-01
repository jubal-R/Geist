#ifndef SETTINGS_H
#define SETTINGS_H

#include "QString"
#include "QStringList"

class Settings
{
public:
    Settings();
    void saveSettings();
    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setTabWidth(int tabWidth);
    void setShowOverview(bool isShown);
    void setTheme(QString theme);
    void setOpenFiles(QStringList files);
    int getWindowWidth();
    int getWindowHeight();
    int getTabWidth();
    bool getShowOverview();
    QString getTheme();
    QStringList getOpenFiles();

private:
    int windowWidth;
    int windowHeight;
    int tabWidth;
    bool showOverview;
    QString theme;
    QStringList openFiles;

};

#endif // SETTINGS_H
