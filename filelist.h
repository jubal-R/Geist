#ifndef FILELIST_H
#define FILELIST_H
#include <string>
#include "highlighter.h"

struct node {
    node * next;
    std::string filepath;
    std::string filetype;
    Highlighter * highlighter;
};

class FileList
{
public:
    FileList();
    void insertNode(node * newNode);
    void removeNode(std::string file);
    void setFilepath(std::string oldfilepath, std::string newfilepath);
    std::string getData(int index);
    QStringList getFilesList();
private:
    node * head;
    int len;
};

#endif // FILELIST_H
