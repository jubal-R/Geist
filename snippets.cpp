#include "snippets.h"
#include <string>
#include <map>



Snippets::Snippets()
{

}

std::map<char,char> insChars;
char endChars[] = {'(','{','['};

std::string Snippets::insertSnippets(std::string c){
    insChars.insert(std::pair<char, char>('(',')'));
    insChars['{'] = '}';
    insChars['['] = ']';

    std::string toReturn = "";
    for(int i = 0; i < 3; i++){
        if(endChars[i] == c[0]){
            toReturn = insChars.at(c[0]);
        }
    }

    return toReturn;
}
