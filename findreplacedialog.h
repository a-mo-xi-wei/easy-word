#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include<QTextEdit>
namespace Ui {
class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QTextEdit* edit ,QWidget *parent = nullptr);
    ~FindReplaceDialog();

private slots:
    void on_find_next_clicked();

    void on_replace_one_clicked();

    void on_replace_all_clicked();

private:
    QTextDocument::FindFlags getFindFlag();

private:
    Ui::FindReplaceDialog *ui;
    QTextEdit* m_edit;
};

#endif // FINDREPLACEDIALOG_H
