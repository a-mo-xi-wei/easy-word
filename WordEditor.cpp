#include "WordEditor.h"
#include "./ui_wordeditor.h"

#include<QFileDialog>
#include<QFile>
#include<QStandardPaths>
#include<QDir>
#include<QMessageBox>
#include<QTextStream>
#include<QSaveFile>
#include<QColorDialog>
#include<QToolButton>
#include<QComboBox>
#include<QGraphicsColorizeEffect>
#include<QFontComboBox>
#include<QFontDatabase>
#include<QCloseEvent>

WordEditor::WordEditor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WordEditor)
    , m_colorEffect(new QGraphicsColorizeEffect(this))
{
    ui->setupUi(this);
    init();
    initAction();
}

void WordEditor::init()
{

    this->resize(820,480);

    m_colorEffect->setColor(Qt::black);


    //将窗口标题设置为空，不这样做的话，会导致后面的setWindowFilePath不成功
    this->setWindowTitle(QString());

    setCurFileName(QString());

    this->statusBar()->showMessage("准备就绪",3000);

    connect(ui->textEdit->document(),&QTextDocument::contentsChanged,this,&WordEditor::documentWasModified);
}

WordEditor::~WordEditor()
{
    delete ui;
}

void WordEditor::closeEvent(QCloseEvent *ev)
{
    if(maybeSave())return;
    ev->ignore();
}

void WordEditor::openFile()
{
    if(!maybeSave()){
        return;
    }
    ui->textEdit->clear();
    QString docPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    docPath = docPath.isEmpty()?QDir::currentPath():docPath;
    QString path = QFileDialog::getOpenFileName(this,"打开文件",docPath);
    if(path.isEmpty()){
        return;
    }
    loadFile(path);
}

void WordEditor::newFile()
{
    if(!maybeSave()){
        return;
    }
    ui->textEdit->clear();
    setCurFileName(QString());
    this->statusBar()->showMessage("新建成功",2000);

}

bool WordEditor::saveFile()
{
    if(this->m_curFilePath.isEmpty()){
        return saveAsFile();
    }
    return save(this->m_curFilePath);
}

bool WordEditor::saveAsFile()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first());
    if(dialog.exec() != QDialog::Accepted){
        return false;
    }
    //qDebug()<<"dialog.selectedFiles() : "<<dialog.selectedFiles().first();
    return save(dialog.selectedFiles().first());
}

void WordEditor::documentWasModified()
{
    this->setWindowModified(ui->textEdit->document()->isModified());
}

void WordEditor::initAction()
{

    this->m_textColor_toolBtn = new QToolButton(this);
    this->m_textColor_toolBtn->setIcon(QIcon("://icons/textColor.png"));
    ui->toolBar->addWidget(m_textColor_toolBtn);
    this->m_textColor_toolBtn->setGraphicsEffect(this->m_colorEffect);
    connect(this->m_textColor_toolBtn,&QToolButton::clicked,this,[this](){
        this->setTextStyle(ActionType::TextColor);
    });

    //分隔符
    ui->toolBar->addSeparator();
    this->m_fontFamily_cbx = new QFontComboBox(this);
    this->m_fontFamily_cbx->setFixedWidth(180);
    //qDebug()<<"增加分隔符";
    ui->toolBar->addWidget(this->m_fontFamily_cbx);
    connect(this->m_fontFamily_cbx,&QFontComboBox::currentFontChanged,[this](){
        this->setTextStyle(ActionType::Font);
    });
    this->m_fontSize_cbx = new QComboBox(this);
    this->m_fontSize_cbx->setFixedWidth(80);
    auto list = QFontDatabase::standardSizes();
    for(auto& size : list ){
        this->m_fontSize_cbx->addItem(QString::number(size));
    }
    QFont font = this->m_fontFamily_cbx->currentFont();
    this->m_fontSize_cbx->setCurrentText(QString::number(font.pointSize()));
    ui->toolBar->addWidget(this->m_fontSize_cbx);
    connect(this->m_fontSize_cbx,&QComboBox::currentIndexChanged,[this](){
        this->setTextStyle(ActionType::FontSize);
    });


    {
        //文件操作
        ui->open_action->setData(QVariant(static_cast<int>(ActionType::OpenFile)));
        ui->new_action->setData(QVariant(static_cast<int>(ActionType::NewFile)));
        ui->save_action->setData(QVariant(static_cast<int>(ActionType::SaveFile)));
        ui->saveAs_action->setData(QVariant(static_cast<int>(ActionType::SaveAsFile)));
        //字体操作
        ui->bold_action->setData(QVariant(static_cast<int>(ActionType::Bold)));
        ui->italic_action->setData(QVariant(static_cast<int>(ActionType::Italic)));
        ui->under_line_action->setData(QVariant(static_cast<int>(ActionType::UnderLine)));
        //排版操作
        ui->left_align_action->setData(QVariant(static_cast<int>(ActionType::LeftAlign)));
        ui->mid_align_action->setData(QVariant(static_cast<int>(ActionType::MidAlign)));
        ui->right_align_action->setData(QVariant(static_cast<int>(ActionType::RightAlign)));
        ui->just_align_action->setData(QVariant(static_cast<int>(ActionType::JustAlign)));
        //文本操作
        ui->cut_action->setData(QVariant(static_cast<int>(ActionType::Cut)));
        ui->copy_action->setData(QVariant(static_cast<int>(ActionType::Copy)));
        ui->paste_action->setData(QVariant(static_cast<int>(ActionType::Paste)));
        ui->undo_action->setData(QVariant(static_cast<int>(ActionType::Undo)));
        ui->redo_action->setData(QVariant(static_cast<int>(ActionType::Redo)));
        ui->find_replace_action->setData(QVariant(static_cast<int>(ActionType::FindAndRePlace)));
        //插入操作
        ui->image_action->setData(QVariant(static_cast<int>(ActionType::Image)));

        //connect(ui->toolBar,&QToolBar::actionTriggered,this,[this](QAction* action){
        //    on_file_menu_triggered(action);
        //});
    }

}

bool WordEditor::maybeSave()
{
    if(!ui->textEdit->document()->isModified())return true;
    const QMessageBox::StandardButton btn = QMessageBox::warning(this,
            "警告","当前文件没有保存。\n是否保存？",QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);
    switch(btn){
        case QMessageBox::Save:
            return saveFile();
        case QMessageBox::Cancel:
            return false;
        default :return true;
    }
    return true;
}

void WordEditor::loadFile(const QString &path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(this,"错误","文件打开失败");
        return;
    }
    QTextStream in(&file);
    //ui->textEdit->insertHtml(in.readAll());
    ui->textEdit->insertPlainText(in.readAll());
}

void WordEditor::setCurFileName(const QString &fileName)
{
    this->m_curFilePath = fileName;
    this->setWindowModified(false);
    ui->textEdit->document()->setModified(false);
    if(this->m_curFilePath.isEmpty()){
        this->setWindowFilePath("untitled");
        //它将文件路径与窗口相关联。如果您设置了文件路径，但还没有设置窗口标题，
        //Qt会将窗口标题设置为指定路径的文件名。
    }
    else {
        this->setWindowFilePath(this->m_curFilePath);
    }
}

bool WordEditor::save(const QString &filePath)
{
    QSaveFile file(filePath);
    if(file.open(QSaveFile::WriteOnly|QSaveFile::Text)){
        QTextStream out(&file);
        out<<ui->textEdit->toHtml();
        if(!file.commit()){
            QMessageBox::critical(this,"错误","保存失败");
            return false;
        }
    }
    setCurFileName(filePath);
    this->statusBar()->showMessage("保存成功",3000);
    return true;
}

void WordEditor::setTextStyle(ActionType style)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if(cursor.isNull()){
        return;
    }
    QFont font = cursor.charFormat().font();
    switch (style) {
    case ActionType::Bold:
        font.setBold(!font.bold());
        break;
    case ActionType::Italic:
        font.setItalic(!font.italic());
        break;
    case ActionType::UnderLine:
        font.setUnderline(!font.underline());
        break;
    case ActionType::Font:
    {
        QFont oldFont = font;//用于还原字体以外的属性
        font.setFamily(this->m_fontFamily_cbx->currentFont().family());
        font.setPointSize(oldFont.pointSize());
        font.setBold(oldFont.bold());
        font.setItalic(oldFont.italic());
        font.setUnderline(oldFont.underline());
        break;
    }
    case ActionType::FontSize:
        font.setPointSize(this->m_fontSize_cbx->currentText().toInt());
        break;
    default:
        break;
    }
    QTextCharFormat format;
    format.setFont(font);
    if(style == ActionType::TextColor){
        QColor color = QColorDialog::getColor(Qt::black,this,"<b>选择颜色</b>");
        if(color.isValid()){
            format.setForeground(color);//将前景画笔设置为指定的画笔。前景画笔主要用于渲染文本。
            this->m_colorEffect->setColor(color);
        }
    }
    if(!cursor.hasSelection()){
        cursor.select(QTextCursor::SelectionType::WordUnderCursor);
    }
    cursor.setCharFormat(format);
}

void WordEditor::setTextAlignment(Qt::Alignment align)
{
    QTextBlockFormat format;
    format.setAlignment(align);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.mergeBlockFormat(format);

}

void WordEditor::insertImage()
{
    QString path = QFileDialog::getOpenFileName(this,"打开文件",QDir::homePath(),"Images (*.png *.png *.jpg)");
    if(path.isEmpty()){
        return;
    }
    QImage img(path);
    ui->textEdit->document()->addResource(QTextDocument::ImageResource,QUrl::fromLocalFile(path),QVariant(img));
    QTextCursor cursor = ui->textEdit->textCursor();
    /*此处后面记得优化看看*/
    QTextImageFormat format;
    format.setHeight(img.height());
    format.setWidth(img.width());
    format.setName(path);
    cursor.insertImage(format);
}

void WordEditor::on_file_menu_triggered(QAction *action)
{
    ActionType type = static_cast<ActionType>(action->data().toInt());
    switch (type) {
    case ActionType::OpenFile:
        qDebug()<<"打开文件";
        openFile();
        break;
    case ActionType::SaveFile:
        qDebug()<<"保存文件";
        saveFile();
        break;
    case ActionType::SaveAsFile:
        qDebug()<<"另存为文件";
        saveAsFile();
        break;
    case ActionType::NewFile:
        qDebug()<<"新建文件";
        newFile();
    default:
        break;
    }

}

void WordEditor::on_edit_menu_triggered(QAction *action)
{
    ActionType type = static_cast<ActionType>(action->data().toInt());
    switch (type) {
    case ActionType::Cut:
        ui->textEdit->cut();
        break;
    case ActionType::Copy:
        ui->textEdit->copy();
        break;
    case ActionType::Paste:
        ui->textEdit->paste();
        break;
    case ActionType::Undo:
        ui->textEdit->undo();
        break;
    case ActionType::Redo:
        ui->textEdit->redo();
        break;
    default:
        break;
    }
}

void WordEditor::on_insert_menu_triggered(QAction *action)
{
    ActionType type = static_cast<ActionType>(action->data().toInt());
    switch (type) {
    case ActionType::Image:
        insertImage();
    default:
        break;
    }
}

void WordEditor::on_toolBar_actionTriggered(QAction *action)
{
    ActionType type = static_cast<ActionType>(action->data().toInt());
    switch (type) {
    case ActionType::Bold:
        setTextStyle(ActionType::Bold);
        break;
    case ActionType::Italic:
        setTextStyle(ActionType::Italic);
        break;
    case ActionType::UnderLine:
        setTextStyle(ActionType::UnderLine);
        break;
    case ActionType::LeftAlign:
        setTextAlignment(Qt::AlignLeft);
        break;
    case ActionType::MidAlign:
        setTextAlignment(Qt::AlignCenter);
        break;
    case ActionType::RightAlign:
        setTextAlignment(Qt::AlignRight);
        break;
    case ActionType::JustAlign:
       setTextAlignment(Qt::AlignJustify);
        break;
    default:
        break;
    }
}
