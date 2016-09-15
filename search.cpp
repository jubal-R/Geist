#include "search.h"
#include <string>
#include <sstream>
#include <iostream>

Search::Search()
{

}

/* Returns positions within content where matches to searchTerm are found.
*  searchTerm must not contain non ascii characters
*  Searchs using boyer-moore alg
*/
std::string Search::findAll(std::string searchTerm, std::string content){
    std::ostringstream positions;

    if(isAsciiStr(searchTerm)){

       int strLen = searchTerm.length();

       int charTable[256];
       int shiftTable[strLen];

       // Setup charTable
       for(int i=0; i < 256; i++){
           charTable[i] = strLen;
       }
       for(int i=0; i < strLen; i++){
           charTable[(unsigned char)searchTerm[i]] = (strLen-1) - i;
       }

       //Setup shiftTable
       int lastPrefixIndex = strLen - 1;

       // Set default shift value and shift value for prefix
       for(int i = strLen-1; i >= 0; i--){
           if(isPrefix(searchTerm, strLen, i+1)){
               lastPrefixIndex = i+1;
           }
           shiftTable[i] = lastPrefixIndex + (strLen-1 - i);
       }

       // Set shift value to next instance of suffix
       for(int i=0; i < strLen-1; i++){
           int suffLen = suffixLen(searchTerm, strLen, i);
           // If not a prefix
           if(searchTerm[i - suffLen] != searchTerm[strLen-1 - suffLen]){
               shiftTable[strLen-1-suffLen] = (strLen-1 - i) + suffLen;
           }

       }

       // Search for matches
       unsigned int i = strLen-1;

       while(i < content.length()){

           int j = strLen-1;

           while(j >= 0 && content[i] == searchTerm[j]){
               i--;
               j--;
           }

           // If found match add to list of found positions
           if(j < 0){
               positions << i+1 << " ";
               i += strLen * 2;

           }else{
               // Shift to next possible match
               if(charTable[(unsigned char)content[i]] < 256)
                   i += std::max(charTable[(unsigned char)content[i]], shiftTable[j]);
               else
                   i += strLen;
           }


       }


   }

   return positions.str();
}

// True if the end of the word starting at pos is a prefix of the word
bool Search::isPrefix(std::string word, int len, int pos){
    int endLen = len - pos;

    if(word.substr(0, endLen) == word.substr(pos)){
        return true;
    }

    return false;
}

// returns length of the longest suffix of the word from pos
int Search::suffixLen(std::string word, int len, int pos){
    int i = 0;
    while(word[pos-i] == word[len-1-i] && i < pos){
        i++;
    }

    return i;
}

bool Search::isAsciiStr(std::string s){
    for(unsigned int i = 0; i < s.length(); i++){
        if(s[i] > 256 || s[i] < 0)
            return false;
    }
    return true;
}
