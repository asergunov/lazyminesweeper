#ifndef CELLS_HPP
#define CELLS_HPP

#include <QAbstractListModel>

class Cells : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        BombNearRole = Qt::UserRole,
        ProbalityRole,
        FlaggedRole,
        OpenedRole,
        SafeRole,
        UserRole
    };

public:
    explicit Cells(QObject *parent = 0);

signals:

public slots:

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
};

#endif // CELLS_HPP
