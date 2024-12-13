#ifndef NOTE_INTERFACE_H
#define NOTE_INTERFACE_H

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
    std::array<unsigned char, 3> sendNoteInfo();

    //updating the note labe:
    void update_note_main_label(QString label_text);

private slots:
    void on_note_list_mode_combo_currentTextChanged(const QString &arg1);

private:
    Ui::Note_interface *note_ui;
    //Used for midiA
    //First letter to Midi Channel
    unsigned char letterToMidiChannel(QChar firstLetter);
    //index to pad note
    unsigned char indexToPadANote(int comboIndex);
    //Used for the notes
    //Returns the midi byte when provided with the midi channel
    unsigned char midichannel_to_hex(int midi_channel);
    //Returns the midi byte when provided with the midi channel
    unsigned char musical_note_to_hex(int musical_note_position);


};

#endif // NOTE_INTERFACE_H
