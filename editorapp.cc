#include <QtGui>
#include <cstdlib>
#include "editorapp.hh"
#include "notelabel.hh"
#include "notegraphwidget.hh"


EditorApp::EditorApp(QWidget *parent): QMainWindow(parent)
{
	ui.setupUi(this);

	noteGraph = new NoteGraphWidget(NULL);
	ui.noteGraphScroller->setWidget(noteGraph);

	// Splitter sizes cannot be set through designer :(
	QList<int> ss; ss.push_back(700); ss.push_back(300); // Proportions, not pixels
	ui.splitter->setSizes(ss);

	// Custom signals/slots
	connect(noteGraph, SIGNAL(updateNoteInfo(NoteLabel*)), this, SLOT(updateNoteInfo(NoteLabel*)));
	updateNoteInfo(NULL);

	// Some icons to menus to make them prettier
	ui.actionNew->setIcon(QIcon::fromTheme("document-new"));
	ui.actionOpen->setIcon(QIcon::fromTheme("document-open"));
	ui.actionSave->setIcon(QIcon::fromTheme("document-save"));
	ui.actionSaveAs->setIcon(QIcon::fromTheme("document-save-as"));
	ui.actionExit->setIcon(QIcon::fromTheme("application-exit"));
	ui.actionUndo->setIcon(QIcon::fromTheme("edit-undo"));
	ui.actionRedo->setIcon(QIcon::fromTheme("edit-redo"));
	ui.actionPreferences->setIcon(QIcon::fromTheme("preferences-other"));
	ui.actionMusicFile->setIcon(QIcon::fromTheme("insert-object"));
	ui.actionLyricsFromFile->setIcon(QIcon::fromTheme("insert-text"));
	ui.actionLyricsFromClipboard->setIcon(QIcon::fromTheme("insert-text"));
	ui.actionAbout->setIcon(QIcon::fromTheme("help-about"));
}


void EditorApp::updateNoteInfo(NoteLabel *note)
{
	if (note) {
		ui.valNoteBegin->setText(QString::number(note->x()));
		ui.valNoteEnd->setText(QString::number(note->x() + note->width()));
		ui.valNoteDuration->setText(QString::number(note->width()));
		ui.valNote->setText(QString::number(note->y() / NoteGraphWidget::noteYStep));
		ui.cmbNoteType->setEnabled(true);
		ui.cmbNoteType->setCurrentIndex(note->note().type);
		ui.chkFloating->setEnabled(true);
		ui.chkFloating->setChecked(note->isFloating());
	} else {
		ui.valNoteBegin->setText("-");
		ui.valNoteEnd->setText("-");
		ui.valNoteDuration->setText("-");
		ui.valNote->setText("-");
		ui.cmbNoteType->setEnabled(false);
		ui.chkFloating->setEnabled(false);
	}
}

void EditorApp::on_actionNew_triggered()
{
	// TODO: Check if a save prompt is in order
	if (true) {
		QMessageBox::StandardButton b = QMessageBox::question(this, tr("Unsaved changes"),
			tr("There are unsaved changes. Do you wish to save before creating a new project?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		switch(b) {
		case QMessageBox::Yes:
			// TODO: Save
		case QMessageBox::No:
			noteGraph->clear(); break;
		default: break;
		}
	} else {
		noteGraph->clear();
	}
}

void EditorApp::on_actionOpen_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
			"",
			tr("All supported formats (*.TBD!!! *.xml *.mid *.ini *.txt)") + ";;" +
			tr("Project files (*.TBD!!!)") + ";;" + // FIXME: Project file extension
			tr("SingStar XML (*.xml)") + ";;" +
			tr("Frets on Fire MIDI (*.mid *.ini)") + ";;" +
			tr("UltraStar TXT (*.txt)") + ";;" +
			tr("All files (*)")
			);

	if (!fileName.isNull()) {
		// TODO: Spawn a parser
	}
}

void EditorApp::on_actionExit_triggered()
{
	// TODO: Check if a save prompt is in order
	if (false) {
		QMessageBox::StandardButton b = QMessageBox::question(this, tr("Unsaved changes"),
			tr("There are unsaved changes. Do you wish to save before quitting?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		switch(b) {
		case QMessageBox::Yes:
			// TODO: Save
		case QMessageBox::No:
			close(); break;
		default: break;
		}
	} else {
		close();
	}
}

void EditorApp::on_actionMusicFile_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
			"",
			tr("Music files (*.mp3 *.ogg)"));

	if (!fileName.isNull()) {
		ui.valMusicFile->setText(fileName);
		ui.tabWidget->setCurrentIndex(1);
		// TODO: Do something the file
	}
}

void EditorApp::on_actionLyricsFromFile_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
			"",
			tr("Text files (*.txt)"));

	if (!fileName.isNull()) {
		QFile file(fileName);
		 if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			 return;

		 QTextStream in(&file);
		 QString text = "";
		 while (!in.atEnd()) {
			 text += in.readLine();
			 if (!in.atEnd()) text += " "; // TODO: Some kind of line separator here
		 }

		 if (text != "")
			 noteGraph->setLyrics(text);
	}
}

void EditorApp::on_actionLyricsFromClipboard_triggered()
{
	const QClipboard *clipboard = QApplication::clipboard();
	const QMimeData *mimeData = clipboard->mimeData();

	if (mimeData->hasText() && !mimeData->text().isEmpty()) {
		QString text = mimeData->text();
		QMessageBox::StandardButton b = QMessageBox::question(this, tr("Replace lyrics"),
			tr("Pasting lyrics from clipboard will replace the existing ones. Continue?"),
			QMessageBox::Ok | QMessageBox::Cancel);
		if (b == QMessageBox::Ok) {
			noteGraph->setLyrics(text);
		}
	} else {
		QMessageBox::warning(this, tr("No text to paste"), tr("No suitable data on the clipboard."));
	}
}

void EditorApp::on_actionWhatsThis_triggered()
{
	QWhatsThis::enterWhatsThisMode ();
}

void EditorApp::on_actionAbout_triggered()
{
	QMessageBox::about(this, tr("About"),
		tr("Semi-automatic karaoke song editor.\n")
		);
}


void EditorApp::on_cmbNoteType_currentIndexChanged(int index)
{
	if (noteGraph->selectedNote())
		noteGraph->selectedNote()->setType(index);
}

void EditorApp::on_chkFloating_stateChanged(int state)
{
	if (noteGraph->selectedNote())
		noteGraph->selectedNote()->setFloating(state != 0);
}
