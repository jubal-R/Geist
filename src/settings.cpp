#include "settings.h"
#include "files.h"
#include <string>
#include <sstream>

Settings::Settings()
{
    Files fileReader;
    std::string settingsStr = fileReader.read("settings.txt");
    QStringList settingsList = QString::fromStdString(settingsStr).split("\n", QString::SkipEmptyParts);

    if (settingsList.length() > 4){
        // Set values to saved settings
        // Note: toInt returns 0 if conversion fails
        windowWidth = settingsList.at(0).toInt();
        windowHeight = settingsList.at(1).toInt();
        showOverview = settingsList.at(2).toInt();
        theme = settingsList.at(3);
        tabWidth = settingsList.at(4).toInt();

        for (int i = 5; i < settingsList.size(); i++){
            openFiles.append(settingsList.at(i));
        }

    } else {
        // Defaults values
        windowWidth = 1000;
        windowHeight = 600;
        tabWidth = 4;
        showOverview = true;
        theme = "solarized";
    }

}

// Set Values
void Settings::setWindowWidth(int width){
    windowWidth = width;
}

void Settings::setWindowHeight(int height){
    windowHeight = height;
}

void Settings::setTabWidth(int width){
    tabWidth = width;
}

void Settings::setShowOverview(bool isShown){
    showOverview = isShown;
}

void Settings::setTheme(QString newTheme){
    theme = newTheme;
}

void Settings::setOpenFiles(QStringList files){
    openFiles = files;
}

// Get Values
int Settings::getWindowWidth(){
    return windowWidth;
}

int Settings::getWindowHeight(){
    return windowHeight;
}

int Settings::getTabWidth(){
    return tabWidth;
}

bool Settings::getShowOverview(){
    return showOverview;
}

QString Settings::getTheme(){
    return theme;
}

QStringList Settings::getOpenFiles(){
    return openFiles;
}

void Settings::saveSettings(){
    // Build settings file string
    std::ostringstream oss;
    oss << getWindowWidth() << "\n";
    oss << getWindowHeight() << "\n";
    oss << getShowOverview() << "\n";
    oss << getTheme().toStdString() << "\n";
    oss << getTabWidth() << "\n";

    for(int i = 0; i < openFiles.length(); i++){
        oss << openFiles.at(i).toStdString() << "\n";
    }

    // Write Settings To File
    Files fileWriter;
    fileWriter.write("settings.txt", oss.str());

}
