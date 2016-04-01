#ifndef PROJECTTREEVIEW_H
#define PROJECTTREEVIEW_H

#include <QTreeView>
class ProjectView;

/**
 * The project tree view.
 */
class ProjectTreeView : public QTreeView
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent The parent widget.
     */
    explicit ProjectTreeView(QWidget* parent = 0);

public slots:
    /**
     * Slot for an open file is triggered.
     */
    void openFileTriggered();

    /**
     * Slot for an item being double clicked.
     * @param index The index of the item double clicked.
     */
    void itemDoubleClicked(const QModelIndex& index);

protected:
    /**
     * Handler fo context menu events.
     * @param event The context menu event.
     */
    void contextMenuEvent(QContextMenuEvent* event);

private:
    void openSelectedItem();

    ProjectView* _projectView;
    
};

#endif
