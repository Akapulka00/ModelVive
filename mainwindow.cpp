#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlError>
#include <QFileDialog>
#include <QStyledItemDelegate>
#include <QPainter>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
   ui->setupUi(this);

}
class CustomDelegate : public QStyledItemDelegate
{
   public:
   void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
   {
      QStyledItemDelegate::paint(painter, option, index);

      if (index.row() > 0) {
         double currentValue = index.data().toDouble();
         double previousValue = index.sibling(index.row() - 1, index.column()).data().toDouble(); // Получаем предыдущее значение в том же столбце
         QRect adjustedRect = option.rect.adjusted(2, 2, -2, -2);// Смещаем координаты внутрь ячейки
         if (currentValue > previousValue) {
            painter->fillRect(option.rect, QBrush(QColor(120, 230, 0, 64)));

             painter->drawText(adjustedRect, Qt::AlignRight, "▲ " + QString::number(((currentValue - previousValue) / previousValue) * 100, 'f', 2) + "%");
         } else if (currentValue < previousValue) {
            painter->fillRect(option.rect, QBrush(QColor(255, 40, 0, 64)));
              // Смещаем координаты внутрь ячейки
             painter->drawText(adjustedRect, Qt::AlignRight, "▼ " + QString::number(((previousValue - currentValue) / previousValue) * 100, 'f', 2) + "%");

         }else{
            painter->drawText(adjustedRect, Qt::AlignRight, "  " + QString( "0%"));
         }
      }
   }
};

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::on_actionconnect_triggered()
{
   int flag=connectDB();
   if(flag==0){
      //Делаем лампочку красной
   }else{
      //Делаем лампочку зеленой

      QSqlQueryModel* model = new QSqlQueryModel(this);
      model->setQuery("SELECT * FROM gold");
      // Отображаем модель данных в таблице
      model->setHeaderData(0,Qt::Horizontal,"Дата");
      model->setHeaderData(1,Qt::Horizontal,"Цена покупки");
      model->setHeaderData(2,Qt::Horizontal,"Цена продажи");
      ui->tableView->setModel(model);
      ui->tableView->horizontalHeader()->setSectionResizeMode(0 , QHeaderView::Stretch);
      // Устанавливаем делегат для поля "Цена продажи"
      CustomDelegate *delegate = new CustomDelegate;
      ui->tableView->setItemDelegateForColumn(2, delegate);
      ui->tableView->setItemDelegateForColumn(1, delegate);
      ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
   }

}
int  MainWindow::connectDB()
{
   QString FileName=QFileDialog::getOpenFileName(this,"Выбирите файл БД",QDir::currentPath(),"*.sqlite");
   int flag=0;
   auto dbConnect = QSqlDatabase::addDatabase("QSQLITE");
   dbConnect.setDatabaseName(FileName);
   if( !dbConnect.open() ){
      qDebug()<<dbConnect.lastError().text();
      flag=0;}
   else
      flag=1;
   return flag;
}

