#include <string>

#include <QDir>
#include <QtXML>
#include <QClipboard>

#include "../../lsMisc/stdQt/stdQt.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace AmbiesoftQt;
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

int getCorrespodentKaishi(const wstring& text, const size_t maxindex)
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
void MainWindow::on_action_PasteArticle_triggered()
{
    wstring text = QApplication::clipboard()->text().replace("\r","").replace("\n","").toStdWString();
    // wstring text = L"aaa（bbb（yyy）zzz）ccc";
    wstring prevtext;
    map<int,wstring> kakkos;

    int maxkakkokaishi = count(text.begin(),text.end(),L'）');
    int maxkakkotoji = count(text.begin(),text.end(),L'）');
    if(maxkakkokaishi==maxkakkotoji)
    {
        int potsuIndex = maxkakkotoji;
        do {
            prevtext = text;
            size_t toji = text.find_first_of(L'）');
            if(toji != wstring::npos)
            {
                size_t kaishi = getCorrespodentKaishi(text, toji);
                Q_ASSERT(kaishi < toji);
                if(kaishi != wstring::npos)
                {
                    wstring potsu = getPostu(potsuIndex);
                    wstring kakko = text.substr(kaishi+1, toji-kaishi-1);
                    kakkos[potsuIndex]=kakko;
                    --potsuIndex;

                    text = text.substr(0, kaishi) + potsu + text.substr(toji+1);
                }
            }
        } while(prevtext != text);

    }
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
