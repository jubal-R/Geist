#ifndef FILELIST_H
#define FILELIST_H
#include <string>
#include "highlighter.h"

struct node {
    node * next;
    std::string filepath;
    Highlighter * highlighter;
};

class FileList
{
public:
    FileList();
    void insertNode(node * newNode);
    void removeNode(std::string file);
    std::string getData(int index);
private:
    node * head;
    int len;
};

#endif // FILELIST_H
