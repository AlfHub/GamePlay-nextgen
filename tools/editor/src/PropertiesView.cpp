#include "PropertiesView.h"
#include "ui_PropertiesView.h"

PropertiesView::PropertiesView(QWidget* parent) :
    QWidget(parent), _ui(new Ui::PropertiesView)
{
    _ui->setupUi(this);
}

PropertiesView::~PropertiesView()
{
    delete _ui;
}

