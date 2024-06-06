#ifndef WORDEDITOR_H
#define WORDEDITOR_H

#include "findreplacedialog.h"
#include <QMainWindow>

class QToolButton;
class QComboBox;
class QGraphicsColorizeEffect;
class QFontComboBox;


QT_BEGIN_NAMESPACE
namespace Ui {
class WordEditor;
}
QT_END_NAMESPACE

enum class ActionType{
    UnKnow,
    NewFile,
    OpenFile,
    SaveFile,
    SaveAsFile,
    Copy,
    Cut,
    Paste,
    Undo,
    Redo,
    Bold,
    Italic,
    UnderLine,
    LeftAlign,
    MidAlign,
    RightAlign,
    JustAlign,
    TextColor,
    Font,
    FontSize,
    Image,
    FindAndRePlace,
    Quit
};

class WordEditor : public QMainWindow
{
    Q_OBJECT

public:
    WordEditor(QWidget *parent = nullptr);
    WordEditor(const WordEditor &) = delete;
    WordEditor(WordEditor &&) = delete;
    WordEditor &operator=(const WordEditor &) = delete;
    WordEditor &operator=(WordEditor &&) = delete;
    void init();
    ~WordEditor();
protected:
    void closeEvent(QCloseEvent* ev)override;
public slots:
    void openFile();
    void newFile();
    bool saveFile();
    bool saveAsFile();
    void documentWasModified();
private:
    void initAction();
    bool maybeSave();   //正在编辑中的文件可能会保存
    void loadFile(const QString& path);
    void setCurFileName(const QString& fileName);
    bool save(const QString& filePath);
    void setTextStyle(ActionType style);
    void setTextAlignment(Qt::Alignment align);
    void insertImage();
private slots:
    //on_xxx_signal这种形式的函数会自动被调用
    void on_file_menu_triggered(QAction* action);
    void on_edit_menu_triggered(QAction* action);
    void on_insert_menu_triggered(QAction* action);
    void on_toolBar_actionTriggered(QAction *action);
private:
    Ui::WordEditor *ui;
    QString m_curFilePath;
    QToolButton* m_textColor_toolBtn;
    QComboBox* m_fontSize_cbx;
    QFontComboBox* m_fontFamily_cbx;
    QGraphicsColorizeEffect* m_colorEffect;
    FindReplaceDialog* m_dialog;
};
#endif // WORDEDITOR_H
