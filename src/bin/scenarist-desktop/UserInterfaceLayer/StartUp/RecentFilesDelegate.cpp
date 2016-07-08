#include "RecentFilesDelegate.h"

#include "RecentFileWidget.h"

#include <QPainter>

using UserInterface::RecentFilesDelegate;
using UserInterface::RecentFileWidget;


RecentFilesDelegate::RecentFilesDelegate(QObject *parent) :
	QStyledItemDelegate(parent),
	m_fileWidget(new RecentFileWidget)
{
}

RecentFilesDelegate::~RecentFilesDelegate()
{
	delete m_fileWidget;
	m_fileWidget = 0;
}

void RecentFilesDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	QVariant projectName = _index.data(Qt::DisplayRole);
	QVariant filePath = _index.data(Qt::WhatsThisRole);

	//
	// Установим в виджет данные
	//
	m_fileWidget->setProjectName(projectName.toString());
	m_fileWidget->setFilePath(filePath.toString());

	//
	// Получим настройки стиля
	//
    QStyleOptionViewItem opt = _option;
	initStyleOption(&opt, _index);

	//
	// Настраиваем размер области отрисовки
	//
	m_fileWidget->resize(opt.rect.size());
	m_fileWidget->setPalette(opt.palette);

	QPoint cursorPos = opt.widget->mapFromGlobal(QCursor::pos());
	QRect itemRect = opt.rect;
	itemRect.setHeight(itemRect.height()-2);

	bool mouseHover = itemRect.contains(cursorPos);
	m_fileWidget->setMouseHover(mouseHover);

	//
	// Отрисовываем виджет
	//
	_painter->save();
	_painter->setRenderHint(QPainter::Antialiasing, true);
	_painter->translate(opt.rect.topLeft());

	m_fileWidget->render(_painter);

	_painter->restore();
}

QSize RecentFilesDelegate::sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	Q_UNUSED(_option);
	Q_UNUSED(_index);

	return m_fileWidget->minimumSizeHint();
}
