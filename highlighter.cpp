/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "highlighter.h"

Highlighter::Highlighter(QString filetype, QString theme, QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    compiledLanguages = QString::fromStdString("adb abs asm c cpp cs dart go h java m rs rlib swift").split(" ");
    scriptingLanguages = QString::fromStdString("coffee hs js php pl ps1 py rb sh").split(" ");
    markupLanguages = QString::fromStdString("html ui xml").split(" ");

    if(theme == "monokai"){
        // monokai
        keywordColor = QColor(102, 217, 239);
        keyword2Color = QColor(249, 38, 114);
        functionsColor = QColor(166, 226, 46);
        valueColor = QColor(230, 218, 117);
        numColor = QColor(174, 130, 255);
        operatorColor = QColor(249, 38, 114);
        formatStringColor = QColor(174, 130, 255);
        commentColor = QColor(178, 179, 191);
        varColor = QColor(102, 217, 239);
        tagColor = QColor(249, 38, 114);
        htmlAttributesColor = QColor(166, 226, 46);
        cssClassesIDsColor = QColor(166, 226, 46);
        cssAttributesColor = QColor(102, 217, 239);
    }else if(theme == "tomorrow"){
        // tomorrow
        keywordColor = QColor(135, 88, 166);
        keyword2Color = QColor(135, 88, 166);
        functionsColor = QColor(66, 114, 173);
        valueColor = QColor(112, 138, 0);
        numColor = QColor(245, 135, 32);
        operatorColor = QColor(77, 77, 76);
        formatStringColor = QColor(199, 40, 40);
        commentColor = QColor(144, 143, 140);
        varColor = QColor(199, 40, 40);
        tagColor = QColor(199, 40, 40);
        htmlAttributesColor = QColor(245, 135, 32);
        cssClassesIDsColor = QColor(62, 153, 158);
        cssAttributesColor = QColor(77, 77, 76);
    }else if(theme == "tomorrowNight"){
        // tomorrow night
        keywordColor = QColor(177, 149, 186);
        keyword2Color = QColor(177, 149, 186);
        functionsColor = QColor(128, 162, 189);
        valueColor = QColor(182, 189, 106);
        numColor = QColor(222, 146, 95);
        operatorColor = QColor(197, 199, 198);
        formatStringColor = QColor(222, 146, 95);
        commentColor = QColor(149, 150, 149);
        varColor = QColor(204, 102, 102);
        tagColor = QColor(204, 102, 102);
        htmlAttributesColor = QColor(222, 146, 95);
        cssClassesIDsColor = QColor(138, 189, 181);
        cssAttributesColor = QColor(197, 199, 198);
    }else{
        // Solarized
        keywordColor = QColor(181, 137, 0);
        keyword2Color = QColor(133, 153, 0);
        functionsColor = QColor(88, 110, 117);
        valueColor = QColor(42, 161, 152);
        numColor = QColor(42, 161, 152);
        operatorColor = QColor(181, 137, 0);
        formatStringColor = QColor(220, 50, 47);
        commentColor = QColor(178, 179, 191);
        varColor = QColor(38, 139, 210);
        tagColor = QColor(38, 139, 210);
        htmlAttributesColor = QColor(181, 137, 0);
        cssClassesIDsColor = QColor(133, 153, 0);
        cssAttributesColor = QColor(77,171,171);
    }

    if(compiledLanguages.contains(filetype)){
        //Functions
        functionFormat.setForeground(functionsColor);
        rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
        rule.format = functionFormat;
        highlightingRules.append(rule);

        keywordFormat.setForeground(keywordColor);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bchar\\b" << "\\bint\\b" << "\\bfloat\\b" << "\\bdouble\\b"
                        << "\\bstruct\\b" << "\\benum\\b" << "\\bvoid\\b" << "\\bshort\\b"
                        << "\\blong\\b" << "\\btrue\\b" << "\\bfalse\\b" << "\\bboolean\\b"
                        << "\\bnull\\b" << "\\bthis\\b" << "\\bfinal\\b"
                        << "\\band\\b" << "\\bor\\b" << "\\bxor\\b"
                        << "\\bconst\\b" << "\\bstatic\\b" << "\\bsigned\\b" << "\\bunsigned\\b"
                        << "\\bimport\\b" << "\\bnamespace\\b" << "\\breturn\\b" << "\\busing\\b"
                        << "\\bfor\\b" << "\\bwhile\\b" << "\\bif\\b" << "\\belse\\b"
                        << "\\bcase\\b" << "\\bswitch\\b" << "\\bdo\\b" << "\\bunion\\b"
                        << "\\bnew\\b" << "\\bclass\\b" << "\\bprivate\\b" << "\\bprotected\\b"
                        << "\\bpublic\\b" << "\\bvirtual\\b" << "\\bslots\\b" << "\\bvolatile\\b"
                        << "\\babstract\\b" << "\\bextends\\b" << "\\bimplements\\b" << "\\bsuper\\b"
                        << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                        << "\\btry\\b" << "\\bcatch\\b" << "\\bthrow\\b" << "\\bbreak\\b";

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        //Nums :D
        classFormat.setForeground(numColor);
        rule.pattern = QRegExp("\\b[0-9\\.]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Quotes
        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("'.*'");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        //Format strings
        classFormat.setForeground(formatStringColor);
        rule.pattern = QRegExp("%[sdifFuoxXeEgGaAcpn]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Comments
        singleLineCommentFormat.setFontItalic(true);
        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp("#[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setFontItalic(true);
        multiLineCommentFormat.setForeground(commentColor);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");

    }else if(scriptingLanguages.contains(filetype)){

        //Functions
        functionFormat.setFontItalic(true);
        functionFormat.setForeground(functionsColor);
        rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
        rule.format = functionFormat;
        highlightingRules.append(rule);

        keywordFormat.setForeground(keyword2Color);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bdef\\b" << "\\bnull\\b" << "\\bimport\\b" << "\\breturn\\b"
                        << "\\bbreak\\b" << "\\bdel\\b" << "\\brequire\\b"
                        << "\\bfor\\b" << "\\bforeach\\b" << "\\bif\\b" << "\\belse\\b"
                        << "\\bin\\b" << "\\bdo\\b" << "\\bwhile\\b" << "\\bnot\\b"
                        << "\\band\\b" << "\\bor\\b" << "\\bwith\\b" << "\\bas\\b"
                        << "\\bclass\\b" << "\\bprivate\\b" << "\\bpublic\\b" << "\\bnew\\b"
                        << "\\bprint\\b" << "\\becho\\b" << "\\btry\\b" << "\\bexcept\\b"
                        << "\\bend\\b";
        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        //Operators :D
        classFormat.setForeground(operatorColor);
        rule.pattern = QRegExp("\\b[+-*/=<>]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        classFormat.setForeground(operatorColor);
        rule.pattern = QRegExp("\\s[+-*/=<>]+\\s");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //php pl vars
        classFormat.setForeground(varColor);
        rule.pattern = QRegExp("\\$\\w+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Ruby class/instance vars
        classFormat.setForeground(varColor);
        rule.pattern = QRegExp("@\\b\\w+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        classFormat.setForeground(varColor);
        rule.pattern = QRegExp("@@\\b\\w+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Nums :D
        classFormat.setForeground(numColor);
        rule.pattern = QRegExp("\\b[0-9\\.]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Quotes
        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("'.*'");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        //Comments
        singleLineCommentFormat.setFontItalic(true);
        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp("#[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setFontItalic(true);
        multiLineCommentFormat.setForeground(commentColor);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");

    }else if(markupLanguages.contains(filetype)){

        //Functions
        functionFormat.setFontItalic(true);
        functionFormat.setForeground(functionsColor);
        rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
        rule.format = functionFormat;
        highlightingRules.append(rule);

        // Tags
        classFormat.setForeground(tagColor);
        rule.pattern = QRegExp("<\\w+\\s+[^>]*>");
        rule.format = classFormat;
        highlightingRules.append(rule);

        classFormat.setForeground(tagColor);
        rule.pattern = QRegExp("<\\w+>");
        rule.format = classFormat;
        highlightingRules.append(rule);

        classFormat.setForeground(tagColor);
        rule.pattern = QRegExp("</\\b\\w+\\b>");
        rule.format = classFormat;
        highlightingRules.append(rule);

        keywordFormat.setForeground(htmlAttributesColor);
        QStringList keywordPatterns;
        keywordPatterns << "\\bclass\\b" << "\\bid\\b" << "\\bhref\\b" << "\\bsrc\\b"
                        << "\\blang\\b" << "\\bcharset\\b" << "\\bname\\b" << "\\bcontent\\b"
                        << "\\brel\\b" << "\\btype\\b";
        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        //classFormat.setFontWeight(QFont::Bold);
        classFormat.setForeground(QColor(209, 84, 84));
        rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Quotes
        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("'.*'");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        //Comments
        multiLineCommentFormat.setFontItalic(true);
        multiLineCommentFormat.setForeground(commentColor);

        commentStartExpression = QRegExp("<!--");
        commentEndExpression = QRegExp("-->");

    }else if(filetype == "css"){

        keywordFormat.setForeground(keywordColor);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bhtml\\b" << "\\bbody\\b" << "\\bp\\b" << "\\ba\\b" << "\\bhr\\b"
                        << "\\bimg\\b" << "\\bheader\\b" << "\\bfooter\\b" << "\\bh1\\b"
                        << "\\bh2\\b" << "\\bh3\\b" << "\\bh4\\b" << "\\bh5\\b" << "\\bh6\\b"
                        << "\\bul\\b" << "\\bol\\b" << "\\bli\\b" << "\\bmain\\b" << "\\bnav\\b"
                        << "\\bmenu\\b" << "\\bmenuitem\\b" << "\\bq\\b" << "\\btable\\b" << "\\btd\\b";
        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        //Nums :D
        classFormat.setForeground(numColor);
        rule.pattern = QRegExp("\\b[0-9\\.]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        classFormat.setForeground(valueColor);
        rule.pattern = QRegExp("\\b[0-9\\.]+[pt|px]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        // Attributes
        classFormat.setForeground(cssAttributesColor);
        rule.pattern = QRegExp("\\w+:");
        rule.format = classFormat;
        highlightingRules.append(rule);

        classFormat.setForeground(cssAttributesColor);
        rule.pattern = QRegExp("\\w+-\\w+:");
        rule.format = classFormat;
        highlightingRules.append(rule);

        // IDs
        classFormat.setForeground(cssClassesIDsColor);
        rule.pattern = QRegExp("#\\w+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        // classes
        classFormat.setForeground(cssClassesIDsColor);
        rule.pattern = QRegExp("\\.\\w+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Quotes
        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("'.*'");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");


    }else if(filetype == "sql"){

        keywordFormat.setForeground(keywordColor);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bSELECT\\b" << "\\bFROM\\b" << "\\bWHERE\\b" << "\\bAND\\b" << "\\bOR\\b"
                        << "\\bDELETE\\b" << "\\bORDER\\b" << "\\bUNION\\b" << "\\bUPDATE\\b"
                        << "\\bINSERT\\b" << "\\bINTO\\b" << "\\bVALUES\\b" << "\\bCREATE\\b"
                        << "\\bTABLE\\b" << "\\bAS\\b" << "\\bBY\\b" << "\\bFOR\\b"
                        << "\\bIF\\b" << "\\bNOT\\b" << "\\bEXISTS\\b";
        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        //Quotes
        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("'.*'");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");

    }else{

        //Nums :D
        classFormat.setFontWeight(QFont::Normal);
        rule.pattern = QRegExp("\\b[0-9\\.]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");
    }

}


void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
