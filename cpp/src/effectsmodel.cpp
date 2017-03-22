#include "effectsmodel.h"
#include "parametermodel.h"

#include <qqml.h>

#include <QDebug>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>

#include <QQmlEngine>

EffectsModel::EffectsModel( QObject *parent ) : QAbstractTableModel( parent ),
    m_roleNames {
        { Roles::EffectName, "effectName" },
        { Roles::EffectType, "effectType" },
        { Roles::EffectModel, "parameterModel" },
}
{

}

EffectsModel::~EffectsModel() {
    for ( int i=0; i < m_model.size(); ++i ) {
        if ( QQmlEngine::objectOwnership( m_model[ i ] ) == QQmlEngine::CppOwnership ) {
            delete m_model[ i ];
        }
    }

    m_model.clear();
}

QHash<int, QByteArray> EffectsModel::roleNames() const {
    return m_roleNames;
}

int EffectsModel::rowCount(const QModelIndex &) const {
    return m_model.size();
}

int EffectsModel::columnCount(const QModelIndex &) const {
    return m_roleNames.size();
}

QVariant EffectsModel::data(const QModelIndex &index, int role) const {

    if ( index.isValid() ) {
        switch( role ) {
        case Roles::EffectName:
            return m_model[ index.row() ]->name();
        case Roles::EffectType:
            return QVariant::fromValue( m_model[ index.row() ]->effectType() );
        case Roles::EffectModel:
            return QVariant::fromValue( m_model[ index.row() ]->model() );
        default:
            Q_UNREACHABLE();
        }
    }

    return QVariant();
}

Qt::ItemFlags EffectsModel::flags(const QModelIndex &index) const {
    return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void EffectsModel::registerTypes() {

    qRegisterMetaType<Effect *>();
    qRegisterMetaType<ParameterModel *>();
    qRegisterMetaType<Parameter>( "Parameter" );


    qmlRegisterType<Effect>( "Models", 1, 0, "Effect" );
    qmlRegisterType<EffectsModel>( "Models", 1, 0, "EffectsModel" );


}

QQmlListProperty<Effect> EffectsModel::effectList() {
    return QQmlListProperty<Effect>( this
                                     , nullptr
                                     , &appendEffect
                                     , nullptr
                                     , nullptr
                                     , nullptr );
}

void EffectsModel::appendEffect(QQmlListProperty<Effect> *list, Effect *effect) {
    EffectsModel *parentModel = qobject_cast<EffectsModel *>( list->object );
    effect->setModel( effect->effectType() );
    parentModel->append( effect );
}

void EffectsModel::append( Effect::Type t_type ) {

    beginInsertRows( QModelIndex(), m_model.size(), m_model.size() );

    Effect *effect = new Effect( this );
    effect->setEffectType( t_type );
    effect->setModel( effect->effectType() );

    m_model.append( effect );

    endInsertRows();
}

void EffectsModel::remove(int t_index) {
    beginRemoveRows( QModelIndex(), t_index, t_index );

    delete m_model[ t_index ];

    m_model.removeAt( t_index );

    endRemoveRows();
}

void EffectsModel::insert(int t_index, Effect::Type t_type ) {

    beginInsertRows( QModelIndex(), t_index, t_index );

    Effect *effect = new Effect( this );
    effect->setEffectType( t_type );
    effect->setModel( t_type );

    m_model.insert( t_index, effect );

    endInsertRows();
}

void EffectsModel::move(int t_srcIndex, int t_destIndex) {

    beginMoveRows( QModelIndex(), t_srcIndex, t_srcIndex, QModelIndex(), t_destIndex );
    m_model.move( t_srcIndex, t_destIndex );
    endMoveRows();

}

void EffectsModel::clear() {
    beginRemoveRows( QModelIndex(), 0, m_model.size() );

    qDeleteAll( m_model );
    m_model.clear();

    endRemoveRows();
}

bool EffectsModel::loadSetlist( QString filePath ) {

    QFile file( filePath );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        qDebug( "Could not open %s", qPrintable( file.fileName() ) );
        return false;
    }

    QJsonParseError parserError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson( file.readAll(), &parserError );

    if ( parserError.error != QJsonParseError::NoError ) {
        qDebug( "%s cannot be parsed, error %s", qPrintable( file.fileName() ), qPrintable( parserError.errorString() ) );
        return false;
    }

    clear();

    QJsonArray rootArray = jsonDoc.array();

    beginInsertRows( QModelIndex(), m_model.size(), rootArray.size() - 1 );

    for ( const QJsonValue &jsonVal : rootArray ) {
        QJsonObject effectMap = jsonVal.toObject();

        Effect *effect = new Effect( this );
        effect->setEffectType( static_cast<Effect::Type>( effectMap[ "effectType" ].toInt() ) );
        effect->setModel( effectMap[ "parameters" ].toArray() );

        m_model.append( effect );
    }

    endInsertRows();

    return true;
}

bool EffectsModel::saveSetlist( QString filePath ) {

    if ( !m_model.isEmpty() ) {

        QFile jsonFile( filePath );
        if ( jsonFile.open( QIODevice::WriteOnly ) ) {


            QJsonArray rootArray;
            \
            for ( Effect *effect : m_model ) {

                QJsonObject effectObject;
                QJsonArray effectParameters;

                for ( int i=0; i < effect->model()->size(); ++i ) {

                    const Parameter &parameter = effect->model()->at( i );

                    QJsonObject parameterMap {
                        { "name", parameter.name },
                        { "min", parameter.min },
                        { "max", parameter.max },
                        { "value", parameter.value },
                    };
                    effectParameters.append( parameterMap );
                }

                effectObject[ "effectType" ] = static_cast<int>( effect->effectType() );
                effectObject[ "parameters" ] = effectParameters;
                rootArray.append( effectObject );
            }

            QJsonDocument jsonDoc( rootArray );

            jsonFile.write( jsonDoc.toJson( QJsonDocument::Indented ) );

            return true;
        }

    }

    return false;
}

void EffectsModel::append(Effect *t_effect) {

    beginInsertRows( QModelIndex(), m_model.size(), m_model.size() );

    m_model.append( t_effect );

    endInsertRows();
}
