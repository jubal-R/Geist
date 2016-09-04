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
