#include <string>
#include <regex>
#include <sstream>

#include <QDir>
#include <QtXML>
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

void getSeparatorString(const wstring& text, wstring* begin, wstring* end)
{
    wstring retBegin(L"[");
    wstring retEnd(L"]");
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

void MainWindow::on_action_PasteArticle_triggered()
{
    wstring text = QApplication::clipboard()->text().replace("\r","").replace("\n","").toStdWString();
    // wstring text = L"aaa（bbb（yyy）zzz）ccc";
    // wstring text = L"aaa（zzz）ccc（yyy）ttt";
    wstring prevtext;
    map<int,wstring> kakkos;

    wstring separatorBegin,separatorEnd;
    getSeparatorString(text, &separatorBegin, &separatorEnd);

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

    function<wstring(const wsmatch & match)> func = [&](const wsmatch& match) {
        wstring s = match.str();
        s = s.substr(1, s.length()-1);
        int previ = stoi(s);
        int newi = writeIndex++;
        {
            wstring prevs= wstring(L"[") + to_wstring(previ) + L"]";
            wstring news= wstring(L"[[") + to_wstring(newi) + L"]]";
            for(int i=1 ; ; ++i)
            {
                if(kakkos.end() == kakkos.find(i))
                    break;
                kakkos[i] = stdStringReplace(kakkos[i], prevs, news);
            }
        }
        {
            wstring prevs= wstring(L"[") + to_wstring(newi) + L"]";
            wstring news= wstring(L"[[") + to_wstring(previ) + L"]]";
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
                kakkos[i] = stdStringReplace(kakkos[i], L"[[", L"[");
                kakkos[i] = stdStringReplace(kakkos[i], L"]]", L"]");
            }
        }
        swap(kakkos[previ], kakkos[newi]);
        return wstring(L"[") + to_wstring(newi) + L"]";
    };

    text = stdRegexReplace(text, wregex(L"\\[\\d+\\]"), func);
    QString uiText;
    uiText += QString::fromStdWString(text);
    uiText += "<br/><br/>";

    for(int i=1 ; ; ++i)
    {
        if(kakkos.end() == kakkos.find(i))
            break;
        wstring postu = kakkos[i];
        uiText += QString("<b>") + "[" + QString::number(i) + "]</b> ";
        uiText += QString::fromStdWString(postu);
        uiText += "<br/>";
    }
    ui->textEditMain->setHtml(uiText);
    //Alert(this, QString::fromStdWString(text));
}
