#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "UndoManager.h"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QItemSelection >
#include <QUrl>

class TreeItem;
class FbsfConfiguration;
class FbsfConfigSequence;
class FbsfConfigNode;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum TreeModelRoles
    {
        RoleName = Qt::UserRole + 1,
        RoleType,
        RoleCategory,
        RoleModuleType,
        RoleSelected,
        RoleHasError,
        RoleParamList
    };

    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel() Q_DECL_OVERRIDE;

    QUrl newModelData(QString aName);
    void setupModelData(QString aFilename);
    void clearModelData();
    const QString checkModelData();
    void checkItemParams(TreeItem* item,QString& aReport);
    void checkItemParams(QModelIndex aIndex);

    TreeItem* readSimulation(FbsfConfiguration &aXmlConfig);
    void readSequence(TreeItem* parent, FbsfConfigSequence &aSeq, bool isSub=false);
    void readPlugins(FbsfConfigSequence &aPluginList);
    void readModules(TreeItem* parent,
                     QList<QMap<QString, QString> > &aModels,
                     QList<FbsfConfigNode> &aNodes);
    // get the tree model as xml formatted string
    void getXmlConfig(QString &aXmlConfig);
    void getXmlSubtree(QString& aXmlConfig,TreeItem* item,int level=0);
    TreeItem* getItem(const QModelIndex &index) const;

    // Cpp interface

    void forkItem(const QModelIndex aParentIndex, int position);
    void insertSequence(const QModelIndex aParentIndex);
    void insertItem(const QModelIndex aParentIndex,
                    TreeItem* item, int position);// generic

    // QML interface
    Q_PROPERTY(bool modified READ modified WRITE modified NOTIFY modifiedChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(bool hasPluginList READ hasPluginList NOTIFY hasPluginListChanged)
    Q_PROPERTY(bool hasError READ hasError NOTIFY hasErrorChanged)
    Q_PROPERTY(int  canCutOrCopy READ canCutOrCopy NOTIFY canCutOrCopyChanged)
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY canRedoChanged)
    Q_PROPERTY(QString configName READ configName NOTIFY configNameChanged)
    Q_PROPERTY(QUrl configUrl READ configUrl NOTIFY configUrlChanged)

    void removeItem(const QModelIndex &index);
    void insertModule(const QModelIndex &index);
    void forkItem(const QModelIndex &index);
    void addPluginList(const QModelIndex &index);
    void removePluginList();
    void moveItem(const QModelIndex &index, int dir);

    int setItemName(const QModelIndex &aIndex,const QVariant& aName);
    void setModuleType(const QModelIndex &aIndex,
                       const QVariant& aCategory,
                       const QVariant &aModuleType);

    bool canCutOrCopy();
    bool canPaste();
    void removeSelection(bool aKeepClipboard=false);
    void cutSelection();
    void copySelection();
    void pasteSelection(const QModelIndex &index);
    void setSelection(QModelIndexList aSelection, QItemSelection  aSelected, QItemSelection  aDeselected);

    //~~~~~~~~~~~~~~~~~~~ undo/redo functions ~~~~~~~~~~~~~~~~~~~~~~
    void undo();
    void redo();
    void beginMacro();
    void endMacro() ;
    bool canUndo();
    bool canRedo();

    //~~~~~~~~~~~~~~~~~~ QAbstractItemModel interface ~~~~~~~~~~~~~~
    QVariant        data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    bool            setData(const QModelIndex &index, const QVariant &value,
                            int role = Qt::EditRole) Q_DECL_OVERRIDE;

    QVariant        headerData(int section, Qt::Orientation orientation,
                               int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool            setHeaderData(int section, Qt::Orientation orientation,
                                  const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    Qt::ItemFlags   flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex     index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex     parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int             rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int             columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    bool            insertRows(int position, int rows,
                               const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool            removeRows(int position, int rows,
                               const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    //~~~~~~~~~~~~~~~~~~~ helper functions ~~~~~~~~~~~~~~~~~~~~~~

    QModelIndex     getIndexByName(QString aName) const;
    TreeItem *      cloneItem(const QModelIndex &index) const;
    TreeItem *      cloneItem(const TreeItem * aItem) const;
    void            setUniqueName(TreeItem* item);
    bool            nameExist(QString aName, TreeItem *newItem, TreeItem *aLookupItem);
    bool            modified() {return mModified;}
    void            modified(bool aStatus) {mModified=aStatus;emit modifiedChanged(mModified);}
    bool            loaded() {return mLoaded;}
    void            loaded(bool aStatus) {mLoaded=aStatus;emit loadedChanged(mLoaded);}
    bool            hasPluginList() {return mHasPluginList;}
    void            hasPluginList(bool aStatus) {mHasPluginList=aStatus;emit hasPluginListChanged(mLoaded);}
    bool            hasError() const {return mHasError; }
    void            hasError(bool aFlag) {mHasError=aFlag;emit hasErrorChanged(aFlag);}

    // get/set the config name property for QML
    QString         configName();
    void            setConfigName(const QString& aName);

    // store the file name
    void            setConfigUrl(const QUrl& aFileUrl);
    QUrl            configUrl(){return mConfigUrl;}
    // helpers
    void            print(TreeItem* item, int level=0);

private:

    TreeItem *              rootItem=nullptr;
    TreeItem *              configItem=nullptr;
    QUrl                    mConfigUrl;
    QString                 mConfigName;
    QHash<int, QByteArray>  m_roleNameMapping;
    unsigned int            moduleAutoIndex=0;
    unsigned int            sequenceAutoIndex=0;
    unsigned int            forkAutoIndex=0;

    static QList<TreeItem *>       sClipBoard; // shared btw tree models

    QList<QPersistentModelIndex> mSelectedIndices;
    bool                    isClipBoardCopy=false;
    UndoManager             mUndoManager;
    bool                    mModified=false;
    bool                    mLoaded=false;
    bool                    mHasPluginList=false;
    bool                    mHasError=false;
public slots:
    void undoChanged(bool canUndo) {Q_UNUSED(canUndo) emit canUndoChanged();}
    void redoChanged(bool canRedo) {Q_UNUSED(canRedo) emit canRedoChanged();}
signals :
    void modifiedChanged(bool aModified);
    void canCutOrCopyChanged();
    void canPasteChanged();
    void canUndoChanged();
    void canRedoChanged();
    void loadedChanged(bool aLoaded);
    void hasPluginListChanged(bool aStatus);
    void hasErrorChanged(bool aStatus);
    void configNameChanged();
    void configUrlChanged();
};

#endif // TREEMODEL_H
