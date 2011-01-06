#include <QtGui>
#include <iostream>
#include <cmath>
#include "notelabel.hh"
#include "notegraphwidget.hh"

NoteGraphWidget::NoteGraphWidget(QWidget *parent)
	: QWidget(parent)
{
	QFile dictionaryFile(":/dictionary/words.txt");
	dictionaryFile.open(QFile::ReadOnly);
	QTextStream inputStream(&dictionaryFile);

	int x = 5;
	int y = 5;

	while (!inputStream.atEnd()) {
		QString word;
		inputStream >> word;
		if (!word.isEmpty()) {
			NoteLabel *wordLabel = new NoteLabel(word, this);
			wordLabel->move(x, y);
			wordLabel->show();
			wordLabel->setAttribute(Qt::WA_DeleteOnClose);
			x += wordLabel->width() + 2;
		}
	}

	setAcceptDrops(true);
}

void NoteGraphWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-notelabel")) {
		if (children().contains(event->source())) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		} else {
			event->acceptProposedAction();
		}
	} else {
		event->ignore();
	}
}

void NoteGraphWidget::dragMoveEvent(QDragMoveEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-notelabel")) {
		if (children().contains(event->source())) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		} else {
			event->acceptProposedAction();
		}
	} else {
		event->ignore();
	}
}

void NoteGraphWidget::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-notelabel")) {
		const QMimeData *mime = event->mimeData();
		QByteArray itemData = mime->data("application/x-notelabel");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		QString text;
		QPoint offset;
		dataStream >> text >> offset;
		new NoteLabel(text, this, event->pos() - offset);

		if (event->source() == this) {
			event->setDropAction(Qt::MoveAction);
			event->accept();
		} else {
			event->acceptProposedAction();
		}
	} else {
		event->ignore();
	}
}

void NoteGraphWidget::mousePressEvent(QMouseEvent *event)
{
	NoteLabel *child = static_cast<NoteLabel*>(childAt(event->pos()));
	if (!child)
		return;

	if (event->button() == Qt::LeftButton) {
		QPoint hotSpot = event->pos() - child->pos();

		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << child->getText() << QPoint(hotSpot);

		QMimeData *mimeData = new QMimeData;
		mimeData->setData("application/x-notelabel", itemData);
		mimeData->setText(child->getText());

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setPixmap(*child->pixmap());
		drag->setHotSpot(hotSpot);

		child->hide();

		if (drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction) == Qt::MoveAction)
			child->close();
		else
			child->show();

	} else if (event->button() == Qt::RightButton) {
		// Cut the text in half
		int cutpos = int(std::ceil(child->getText().length() / 2.0));
		QString firstst = child->getText().left(cutpos);
		QString secondst = child->getText().right(child->getText().length() - cutpos);
		// Create new labels
		NoteLabel *newLabel1 = new NoteLabel(firstst, this,child->pos());
		new NoteLabel(secondst, this, newLabel1->pos() + QPoint(newLabel1->width(), 0));
		// Delete the old one
		child->close();
	}
}

void NoteGraphWidget::wheelEvent(QWheelEvent *event)
{
	NoteLabel *child = static_cast<NoteLabel*>(childAt(event->pos()));
	if (!child)
		return;

	// Figure out new size and apply it
	int neww = child->size().width() + event->delta() * 0.1;
	child->resize(neww, child->size().height());

	event->accept();
}

void NoteGraphWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	NoteLabel *child = static_cast<NoteLabel*>(childAt(event->pos()));
	if (!child)
		return;

	// Spawn an input dialog
	bool ok;
	QString text = QInputDialog::getText(this, tr("Edit lyric"),
										  tr("Lyric:"), QLineEdit::Normal,
										  child->getText(), &ok);
	if (ok && !text.isEmpty()) {
		child->setText(text);
		child->createPixmap();
	}

	event->accept();
}