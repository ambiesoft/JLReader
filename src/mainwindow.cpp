#include <string>
#include <regex>
#include <sstream>

#include <QDir>
#include <QtXml>
#include <QClipboard>

#include "../../lsMisc/stdQt/stdQt.h"
#include "../../lsMisc/stdosd/stdosd.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace AmbiesoftQt;
using namespace Ambiesoft::stdosd;
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_Test_triggered()
{
//    QString file=pathCombine(QDir::currentPath(),
//                             "../../src/xmlsample/340AC0000000033_20190701_431AC0000000006.xml");
    QString path=pathCombine(QDir::currentPath(),
                             "../src/xmlsample/sample01.xml");
    if(!QFile::exists(path))
    {
        Alert(this, tr("'%1' does not exist.").arg(path));
        return;
    }
    QFile file(path);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        Alert(this, tr("Failed to open'%1'").arg(path));
        return;
    }
    QDomDocument dom;
    if(!dom.setContent(&file))
    {
        Alert(this,"error");
        return;
    }
    QDomNodeList lawTitle = dom.elementsByTagName("LawTitle");
    Q_ASSERT(lawTitle.count()==1);
    QDomNode nodeTitle = lawTitle.at(0);
    Q_ASSERT(nodeTitle.isElement());
    QDomElement elemTitle = nodeTitle.toElement();
    QString title = elemTitle.text();
    Info(this,title);
}

size_t getCorrespodentKaishi(const wstring& text, const size_t maxindex)
{
    size_t found = wstring::npos;
    size_t kaishi = 0;
    do
    {
        kaishi = text.find_first_of(L'（', kaishi);
        if(kaishi == wstring::npos)
            return found;
        if(maxindex < kaishi)
            return found;
        found = kaishi;
        ++kaishi;
   } while(true);
}
wstring getPostu(int count)
{
    wstring ret;
    ret = L"[" + to_wstring(count) + L"]";
    return ret;
}

void getSeparatorString(const wstring& text, wstring* begin, wstring* end,
                        const wstring& kakkoStart, const wstring& kakkoEnd)
{
    wstring retBegin(kakkoStart);
    wstring retEnd(kakkoEnd);
    do
    {
        if(wstring::npos == text.find(retBegin) &&
                wstring::npos == text.find(retEnd))
        {
            *begin=retBegin;
            *end=retEnd;
            return;
        }
        retBegin += L"[";
        retEnd += L"]";
    } while(true);
    Q_ASSERT(false);
}

//std::wstring regex_replace(
//    const std::wstring& input,
//    const std::wregex& regex,
//    std::function<std::wstring(std::wsmatch const& match)> format)
//{
//    std::wostringstream output;
//    std::wsregex_iterator begin(input.begin(), input.end(), regex), end;
//    wsregex_iterator::difference_type lastPos = 0;
//    for(; begin != end; begin++)
//    {
//        output << begin->prefix() << format(*begin);
//        qDebug() << output.str();
//        lastPos = begin->position() + begin->length();
//    }
//    // output << input.substr(input.size() - begin->position());
//    output << input.substr(lastPos);
//    qDebug() << output.str();
//    return output.str();
//}

int getNumber(const wstring& s)
{
    const wchar_t* pNum = L"0123456789";
    size_t posStart = s.find_first_of(pNum);
    if(posStart == wstring::npos)
        return 0;

    wstring ns;
    for(auto&& c : s.substr(posStart))
    {
        if(!isdigit(c))
            break;

        ns += c;
    }
    return  stoi(ns);
}

const wchar_t* pKansuji = L"〇一二三四五六七八九十百千万億兆京";
wstring getKansujiRegexString()
{
    wstring ret;
    ret += L"[";
    for(const wchar_t* p = pKansuji; *p ; ++p)
    {
        ret += *p;
        ret += L"|";
    }
    ret = stdTrimEnd(ret, L"|");
    ret += L"]";
    return ret;
}
wregex getNengoRegex()
{
    wstring ret;

    ret += L"((明治)|(大正)|(昭和)|(平成))";
    ret += L"((";

    ret += getKansujiRegexString();

    ret += L")+)年";

    return wregex(ret);
}

int getNumFromKansuji(const wstring& kansuji)
{
    struct Remnant {
        wstring num_;
        bool juu=false;
        bool hyaku=false;
        bool sen=false;
        bool man=false;
        bool oku=false;
        bool tyou=false;
        bool kei=false;
        void clear() {
            juu=hyaku=sen=man=oku=tyou=kei=false;
        }

        wstring getNum(wchar_t c) {
            wstring ret;
            switch(c)
            {
            case L'〇': ret+=L"0"; break;
            case L'一': ret+=L"1"; break;
            case L'二': ret+=L"2"; break;
            case L'三': ret+=L"3"; break;
            case L'四': ret+=L"4"; break;
            case L'五': ret+=L"5"; break;
            case L'六': ret+=L"6"; break;
            case L'七': ret+=L"7"; break;
            case L'八': ret+=L"8"; break;
            case L'九': ret+=L"9"; break;
//            case L'十': num+=L"1"; juu=true; break;
//            case L'百': num+=L"1"; hyaku=true; break;
//            case L'千': num+=L"1"; sen=true; break;
//            case L'万': num+=L"1"; man=true; break;
//            case L'億': num+=L"1"; oku=true; break;
//            case L'兆': num+=L"1"; tyou=true; break;
//            case L'京': num+=L"1"; kei=true; break;
            }
            return ret;
        }

        bool isSuuji(wchar_t c) {
            switch(c)
            {
            case L'〇': return true;
            case L'一': return true;
            case L'二': return true;
            case L'三': return true;
            case L'四': return true;
            case L'五': return true;
            case L'六': return true;
            case L'七': return true;
            case L'八': return true;
            case L'九': return true;
            }
            return false;
        }
        wstring addZero(wchar_t c) {
            wstring ret;
            switch(c)
            {
            case L'十': ret+=L"0"; break;
            case L'百': ret+=L"00"; break;
            case L'千': ret+=L"000"; break;
            case L'万': ret+=L"0000"; break;
            case L'億': ret+=L"000000000"; break;
            case L'兆': ret+=L"0000000000000"; break;
            case L'京': ret+=L"00000000000000000"; break;
            default:
                Q_ASSERT(false);
            }
            return ret;
        }
        void parse(const wstring& t) {
            wchar_t last = 0;
            for(wchar_t c : t)
            {
                last = c;
                if(num_.empty()) {
                    if(!isSuuji(c))
                    {
                        num_ += L"1";
                        continue;
                    }
                }
                num_ += getNum(c);
            }
            if(last && !isSuuji(last))
            {
                num_ += addZero(last);
            }
        }

        int value() {
            num_ = stdTrimStart(num_, L"0");
            return stoi(num_);
        }
    } rem;

    rem.parse(kansuji);
    return rem.value();
}
wstring getADFromNengo(const wstring& nengo, const wstring& kansuji)
{
    int baseyear;
    if(nengo==L"明治")
        baseyear = 1867;
    else if(nengo==L"大正")
        baseyear = 1911;
    else if(nengo==L"昭和")
        baseyear = 1925;
    else if(nengo==L"平成")
        baseyear = 1988;
    else if(nengo==L"令和")
        baseyear = 2018;
    else
    {
        Q_ASSERT(false);
        return wstring();
    }

    int year = getNumFromKansuji(kansuji);
    return to_wstring(baseyear + year);
}
wstring getADedText(const wstring& text)
{
    // ((昭和)|(平成))([xxx])+年
    function<wstring(const wsmatch & match)> func = [&](const wsmatch& match) {
        wstring nengo = match[1];
        wstring kansuji = match[6];

        return getADFromNengo(nengo, kansuji) + L"年";
    };
    return stdRegexReplace(text, getNengoRegex(), func);
}

wregex getJoRegex()
{
    wstring t;
    t += L"第(";
    t += getKansujiRegexString();
    t += L"+)条";

    return wregex(t);
}
wstring getJoToNumText(const wstring& text)
{
    function<wstring(const wsmatch & match)> func = [&](const wsmatch& match) {
        wstring kansuji = match[1];

        return L"第" + to_wstring(getNumFromKansuji(kansuji)) + L"条";
    };
    return stdRegexReplace(text, getJoRegex(), func);
}
void MainWindow::on_action_PasteArticle_triggered()
{
    QString clipText = QApplication::clipboard()->text().replace("\r","").replace("\n","");
    ui->textEdit_Original->setText(clipText);

    doPaste(clipText);
}
wstring MainWindow::modifyLawText(const wstring& text)
{
    wstring ret(text);
    if(ui->action_AD->isChecked())
        ret = getADedText(ret);
    if(ui->action_Jo_To_Num->isChecked())
        ret = getJoToNumText(ret);

    return ret;
}
void MainWindow::doPaste(const QString& originalText)
{
    wstring text = originalText.toStdWString();
    // wstring text = L"aaa（bbb（yyy）zzz）ccc";
    // wstring text = L"aaa（zzz）ccc（yyy）ttt";

    ui->textEdit_Original->setText(QString::fromStdWString(text));



    wstring prevtext;
    map<int,wstring> kakkos;

    wstring separatorBegin,separatorEnd;
    getSeparatorString(text, &separatorBegin, &separatorEnd,
                       L"[", L"]");
    wstring separatorForReplaceBegin,separatorForReplaceEnd;
    getSeparatorString(text, &separatorForReplaceBegin, &separatorForReplaceEnd,
                       L"<", L">");

    int maxkakkokaishi = count(text.begin(),text.end(),L'）');
    int maxkakkotoji = count(text.begin(),text.end(),L'）');
    if(maxkakkokaishi==maxkakkotoji)
    {
        int potsuIndex = 1;
        do {
            prevtext = text;

            // find inner-most toji
            size_t toji = text.find(L'）');
            if(toji != wstring::npos)
            {
                // find correspond kaishi
                size_t kaishi = getCorrespodentKaishi(text, toji);
                Q_ASSERT(kaishi < toji);
                if(kaishi != wstring::npos)
                {
                    // wstring potsu = getPostu(potsuIndex);
                    // extract kakko
                    wstring kakko = text.substr(kaishi+1, toji-kaishi-1);
                    kakkos[potsuIndex]=kakko;

                    // put separator at the position of kakko
                    text = text.substr(0, kaishi) +
                            (separatorBegin + to_wstring(potsuIndex) + separatorEnd) +
                            text.substr(toji+1);
                    ++potsuIndex;
                }
            }
        } while(prevtext != text);

    }

    int writeIndex=1;

    function<wstring(const wsmatch & match)> funcRenumber = [&](const wsmatch& match) {
        int previ = getNumber(match.str());
        int newi = writeIndex++;
        {
            wstring prevs= separatorBegin + to_wstring(previ) + separatorEnd;
            wstring news= separatorForReplaceBegin + to_wstring(newi) + separatorForReplaceEnd;
            for(int i=1 ; ; ++i)
            {
                if(kakkos.end() == kakkos.find(i))
                    break;
                kakkos[i] = stdStringReplace(kakkos[i], prevs, news);
            }
        }
        {
            wstring prevs= separatorBegin + to_wstring(newi) + separatorEnd;
            wstring news= separatorForReplaceBegin + to_wstring(previ) + separatorForReplaceEnd;
            for(int i=1 ; ; ++i)
            {
                if(kakkos.end() == kakkos.find(i))
                    break;
                kakkos[i] = stdStringReplace(kakkos[i], prevs, news);
            }
        }
        {
            for(int i=1 ; ; ++i)
            {
                if(kakkos.end() == kakkos.find(i))
                    break;
                kakkos[i] = stdStringReplace(kakkos[i], separatorForReplaceBegin, separatorBegin);
                kakkos[i] = stdStringReplace(kakkos[i], separatorForReplaceEnd, separatorEnd);
            }
        }
        swap(kakkos[previ], kakkos[newi]);
        return separatorBegin + to_wstring(newi) + separatorEnd;
    };

    function<wstring(const wsmatch & match)> funcItalic = [&](const wsmatch& match) {
        return wstring(L"<i>") + match.str() + L"</i>";
    };

    // create function to get kakko
    function<wregex(const wstring& start, const wstring& end)> getRegex = [&](const wstring& start, const wstring& end) {
        wstring retStart;
        for(auto&& c : start) {
            if(c==L'[' || c==L']') {
                retStart += L"\\";
            }
            retStart += c;
        }
        wstring retEnd;
        for(auto&& c : end) {
            if(c==L'[' || c==L']') {
                retEnd += L"\\";
            }
            retEnd += c;
        }
        return wregex(retStart + L"\\d+" + retEnd);
    };

//    text = stdRegexReplace(text, wregex(L"\\[\\d+\\]"), func);
    wregex regexSeparator = getRegex(separatorBegin, separatorEnd);
    text = stdRegexReplace(text, regexSeparator, funcRenumber);
    text = modifyLawText(text);
    text = stdRegexReplace(text, regexSeparator, funcItalic);
    QString uiText;
    uiText += QString::fromStdWString(text);
    uiText += "<br/><br/>";

    // modify kakko order in kakkos
    bool doneKakkoModify=false;
    do
    {
        for(int i=1 ; ; ++i)
        {
            if(kakkos.end() == kakkos.find(i))
            {
                doneKakkoModify=true;
                break;
            }
            wstring text = kakkos[i];
            stdRegexReplace(text, regexSeparator, funcRenumber);
        }
    } while(!doneKakkoModify);


    for(int i=1 ; ; ++i)
    {
        if(kakkos.end() == kakkos.find(i))
            break;

        uiText += QString("<i>") + "[" + QString::number(i) + "]</i> ";

        wstring t = kakkos[i];
        t = modifyLawText(t);
        t = stdRegexReplace(t, regexSeparator, funcItalic);
        uiText += QString::fromStdWString(t);
        uiText += "<br/>";
    }
    ui->textEditMain->setHtml(uiText);
    //Alert(this, QString::fromStdWString(text));
}

void MainWindow::on_action_AD_triggered(bool checked)
{
    Q_UNUSED(checked)
    doPaste(ui->textEdit_Original->toPlainText());
}

void MainWindow::on_action_Jo_To_Num_triggered(bool checked)
{
    Q_UNUSED(checked)
    doPaste(ui->textEdit_Original->toPlainText());
}
