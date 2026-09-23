#ifndef LINE_EDITOR_H
#define LINE_EDITOR_H

#include <string>
#include "terminal.h"
#include "autocomplete.h"

class LineEditor {
public:
    LineEditor(Terminal& term, Autocompleter& completer);

    bool read_line(std::string& line);
    void redraw(const std::string& line);

private:
    Terminal& term;
    Autocompleter& completer;
    int tab_counter = 0;
};

#endif // LINE_EDITOR_H
