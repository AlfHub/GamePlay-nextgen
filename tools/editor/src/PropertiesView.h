#ifndef PROPERTIESVIEW_H
#define PROPERTIESVIEW_H

#include <QWidget>

namespace Ui {
class PropertiesView;
}

/**
 * Defines a view for diplaying and editing scene objects.
 */
class PropertiesView : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent The parent widget.
     */
    explicit PropertiesView(QWidget* parent = 0);

    /**
     * Destructor.
     */
    ~PropertiesView();

private:
    Ui::PropertiesView* _ui;
};

#endif
