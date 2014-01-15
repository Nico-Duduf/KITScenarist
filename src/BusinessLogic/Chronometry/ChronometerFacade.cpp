#include "ChronometerFacade.h"

#include "AbstractChronometer.h"
#include "PagesChronometer.h"
#include "CharactersChronometer.h"
#include "ConfigurableChronometer.h"

#include <Storage/StorageFacade.h>
#include <Storage/SettingsStorage.h>

#include <QTextCursor>
#include <QTextBlock>
#include <QString>

using namespace StorageLayer;


int ChronometerFacade::calculate(const QTextBlock& _fromBlock, const QTextBlock& _toBlock)
{
	return calculate(
				const_cast<QTextDocument*>(_fromBlock.document()),
				_fromBlock.position(),
				_toBlock.position() + _toBlock.length() - 1);
}

int ChronometerFacade::calculate(QTextDocument* _document, int _fromCursorPosition, int _toCursorPosition)
{
	float chronometry = 0;

	QTextCursor cursor(_document);
	cursor.setPosition(_fromCursorPosition);
	bool isFirstStep = true;
	do {
		//
		// Перейти к следующему, если это не первый шаг цикла
		//
		if (!isFirstStep) {
			cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
			if (cursor.position() > _toCursorPosition) {
				cursor.setPosition(_toCursorPosition+1, QTextCursor::KeepAnchor);
			}
		} else {
			isFirstStep = false;
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		}

		//
		// Получить текст одного блока
		//
		if (cursor.atBlockEnd()
			|| cursor.position() == _toCursorPosition) {
			//
			// Посчитать его хронометраж, добавив к результату
			//
			chronometry +=
					chronometer()->calculateFrom(
						ScenarioTextBlockStyle::forBlock(cursor.block()),
						cursor.selectedText()
						);
			cursor.clearSelection();
		}
	} while (!cursor.atEnd()
			 && cursor.position() <= _toCursorPosition);

	return qRound(chronometry);
}

AbstractChronometer* ChronometerFacade::chronometer()
{
	static QString CHRONOMETRY_TYPE_KEY = "chronometry/current-chronometer-type";
	static QString CHRONOMETRY_PAGES = PagesChronometer().name();
	static QString CHRONOMETRY_CHARACTERS = CharactersChronometer().name();
	static QString CHRONOMETRY_CONFIGURABLE = ConfigurableChronometer().name();

	//
	// Определить какой хронометр нужно использовать
	// Если не задан, настроить на хронометр для страниц
	//
	QString chronometryType = StorageFacade::settingsStorage()->value(CHRONOMETRY_TYPE_KEY);
	if (chronometryType.isEmpty()) {
		chronometryType = CHRONOMETRY_PAGES;
		StorageFacade::settingsStorage()->setValue(CHRONOMETRY_TYPE_KEY, chronometryType);
	}

	//
	// Проверить какой используется
	// Если нужно создать необходимый
	//
	if (chronometryType == CHRONOMETRY_PAGES) {
		if (s_chronometer == 0
			|| s_chronometer->name() != CHRONOMETRY_PAGES) {
			delete s_chronometer;
			s_chronometer = new PagesChronometer;
		}
	} else if (chronometryType == CHRONOMETRY_CHARACTERS) {
		if (s_chronometer == 0
			|| s_chronometer->name() != CHRONOMETRY_CHARACTERS) {
			delete s_chronometer;
			s_chronometer = new CharactersChronometer;
		}
	} else if (chronometryType == CHRONOMETRY_CONFIGURABLE) {
		if (s_chronometer == 0
			|| s_chronometer->name() != CHRONOMETRY_CONFIGURABLE) {
			delete s_chronometer;
			s_chronometer = new ConfigurableChronometer;
		}
	} else {
		Q_ASSERT(0);
	}

	return s_chronometer;
}

AbstractChronometer* ChronometerFacade::s_chronometer = 0;