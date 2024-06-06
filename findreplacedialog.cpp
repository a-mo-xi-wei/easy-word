#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"
#include <QMessageBox>
#include<QTextBlock>
FindReplaceDialog::FindReplaceDialog(QTextEdit* edit ,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindReplaceDialog)
    , m_edit(edit)
{
    ui->setupUi(this);
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::on_find_next_clicked()
{
    QString str = ui->find_lineEdit->text();
    if(str.isEmpty())return;
    static QTextCursor cursor;
    cursor = this->m_edit->document()->find(str,cursor,getFindFlag());
    if(!cursor.isNull()){
        this->m_edit->setTextCursor(cursor);
    }
    else{
        QMessageBox::information(this,"提示",QString("没有找到%1").arg(str));
        if(ui->find_up->isChecked()){
            cursor = QTextCursor(this->m_edit->document()->lastBlock());
        }
    }
}


void FindReplaceDialog::on_replace_one_clicked()
{
    QString findStr = ui->find_lineEdit->text();
    QString replaceStr = ui->replace_lineEdit->text();

    if(findStr.isEmpty() || replaceStr.isEmpty())return;

    static bool state = false;
    static QTextCursor cursor;
    if(!state)
        cursor = this->m_edit->document()->find(findStr,cursor,getFindFlag());
    if(!cursor.isNull()){
        if(state){
            cursor.removeSelectedText();
            cursor.insertText(replaceStr);
        }
        this->m_edit->setTextCursor(cursor);
        state = !state;
    }
    else{
        QMessageBox::information(this,"提示","没有找到");
        if(ui->find_up->isChecked()){
            cursor = QTextCursor(this->m_edit->document()->lastBlock());
        }
    }


}


void FindReplaceDialog::on_replace_all_clicked()
{
    QString findStr = ui->find_lineEdit->text();
    QString replaceStr = ui->replace_lineEdit->text();

    if(findStr.isEmpty() || replaceStr.isEmpty())return;
    QTextCursor cursor;
    while(true){
        cursor = this->m_edit->document()->find(findStr,cursor,getFindFlag());
        if(cursor.isNull()){
            break;
        }
        cursor.removeSelectedText();
        cursor.insertText(replaceStr);
    }
}

QTextDocument::FindFlags FindReplaceDialog::getFindFlag()
{
    QTextDocument::FindFlags flag(QTextDocument::FindWholeWords);
    if(ui->checkBox->isChecked()){
        flag.setFlag(QTextDocument::FindCaseSensitively);
    }
    else{
        flag.setFlag(QTextDocument::FindCaseSensitively,false);
    }
    if(ui->find_up->isChecked()){
        flag.setFlag(QTextDocument::FindBackward);
    }
    else{
        flag.setFlag(QTextDocument::FindBackward,false);
    }
    return flag;
}

