//Copyright (C) 2016  Jubal
#include "runner.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

using namespace std;

Runner::Runner()
{
}

//  Run program given filepath
string Runner::run(string path){
    ostringstream oss;
    ostringstream oss2;
    oss << path << " 2>&1";
    FILE *in;
    char buff[512];

    if(!(in = popen(oss.str().c_str() ,"r") )){
        return "Fail :(";
    }
    while(fgets(buff, sizeof(buff), in) !=NULL){
        oss2 << buff;
    }
    pclose(in);

    return oss2.str();
}

//  Run program in xterm given filepath
string Runner::runxterm(string path){
    ostringstream oss;
    ostringstream oss2;
    //oss << "xterm -T \"" << path << "\" -e \"(" << path << "; read -p '\npress enter')\" 2>&1";
    FILE *in;
    char buff[512];

    pid_t pid;
    pid = fork();

    oss << "\"" << path << "; read -p '\\npress enter'\"";

    if (pid == 0) {			/* child process */
        execl("/usr/bin/xterm", "xterm", "-e", oss.str().c_str(), "2>&1", (char *) 0);
        exit(0);
    }

//    if(!(in = popen(oss.str().c_str() ,"r") )){
//        return "Fail :(";
//    }
//    while(fgets(buff, sizeof(buff), in) !=NULL){
//        oss2 << buff;
//    }
//    pclose(in);

    return oss2.str();
}

//  Run in gnome terminal
string Runner::runGnomeTerminal(string path){
    ostringstream oss;
    ostringstream oss2;
    oss << "gnome-terminal -e \"" << path << "\" 2>&1";
    FILE *in;
    char buff[512];
    if(!(in = popen(oss.str().c_str() ,"r") )){
        return "Fail :(";
    }
    while(fgets(buff, sizeof(buff), in) !=NULL){
        oss2 << buff;
    }
    pclose(in);

    return oss2.str();
}

