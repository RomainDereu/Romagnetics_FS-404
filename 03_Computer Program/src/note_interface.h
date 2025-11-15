#ifndef NOTE_INTERFACE_H
#define NOTE_INTERFACE_H
#include "midi_note.h"
#include <QWidget>



namespace Ui {
class Note_interface;
}

class Note_interface : public QWidget
{
    Q_OBJECT

public:
    explicit Note_interface(QWidget *parent = nullptr);
    ~Note_interface();

    // Link between the UI and the other MIDI functions
    // Takes the UI values and transforms them.
    // The other functions are defined below.
    Midi_note sendNoteInfo();

    //updating the note label. Used during setup:
    void update_note_main_label(QString label_text);

    //updating the note combo. Used during setup:
    void update_note_value(int combo_index);

    void setIsBaseNote(bool isBase);

    enum NoteType : unsigned char {
        PLAY_NOTE = 0,
        NEXT_NOTE_A,
        NEXT_NOTE_LOOP_A,
        NEXT_NOTE_B,
        NEXT_NOTE_LOOP_B,
        PREVIOUS_NOTE_A,
        PREVIOUS_NOTE_LOOP_A,
        PREVIOUS_NOTE_B,
        PREVIOUS_NOTE_LOOP_B,
        REPEAT_NOTE,
        RESET_NOTE,
        KILL_ALL
    };

    unsigned char currentType() const;

private slots:
    void on_note_list_mode_combo_currentTextChanged(const QString &arg1);

private:
    //Setting up note interface
    Ui::Note_interface *note_ui;
    void setup_noteui();

};

#endif // NOTE_INTERFACE_H
