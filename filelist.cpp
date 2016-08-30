#include "filelist.h"
#include "highlighter.h"
#include <iostream>

FileList::FileList(){
    head = new node;
    head->next = NULL;
    head->highlighter = NULL;
    head->filepath = "";
    head->filetype = "";
    len = 0;
}

void FileList::insertNode(node * newNode){
    newNode->next = NULL;

    if(head->next == NULL){
        head->next = newNode;
    }else{
        node * n = head;

        while(n->next != NULL){
            n = n->next;
        }

        n->next = newNode;
    }
    len++;
}

void FileList::removeNode(std::string file){

    if(head->next != NULL && file != ""){

        node * n = head;
        node * p = head;

        while(file != n->filepath && n->next != NULL){
            p = n;
            n = n->next;
        }

        if(file == n->filepath){

            if(n->next == NULL){
                p->next = NULL;
            }else{
                p->next = n->next;
            }

            if(n->highlighter != NULL){
                delete n->highlighter;
            }
            delete n;
        }
        len--;
    }
}

void FileList::setFilepath(std::string oldfp, std::string newfp){
    if(head->next != NULL && oldfp != "" && newfp != ""){
        node * n = head;

        while(oldfp != n->filepath && n->next != NULL){
            n = n->next;
        }

        if(oldfp == n->filepath){
            n->filepath = newfp;
        }

    }

}

std::string FileList::getData(int index){
    if(index <= len){
        node * n = head;

        for(int i=0; i<=index; i++){
            n = n->next;
        }

        return n->filepath;

    }else{

        return "";
    }
}
