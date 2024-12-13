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
    std::vector<int> sendNoteInfo();

    //updating the note labe:
    void update_note_main_label(QString label_text);

private slots:
    void on_note_list_mode_combo_currentTextChanged(const QString &arg1);

private:
    Ui::Note_interface *note_ui;


};

#endif // NOTE_INTERFACE_H
