#include "line_editor.h"
#include <iostream>
#include <sstream>
#include <vector>

LineEditor::LineEditor(Terminal& term, Autocompleter& completer)
    : term(term), completer(completer), tab_counter(0) {}

void LineEditor::redraw(const std::string& line) {
    std::cout << "\r\033[K$ " << line;
    std::cout.flush();
}

bool LineEditor::read_line(std::string& line) {
    std::cout << "$ ";
    std::cout.flush();
    line.clear();
    tab_counter = 0;

    while (true) {
        char ch = term.getchar();

        if (static_cast<int>(ch) == 9) { // Tab key
            if (line.find(' ') != std::string::npos) {
                std::string temp = line;
                line = completer.trie_autocomplete(line);
                redraw(line);

                if (temp != line) {
                    std::cout << "\033[1C";
                    line += " ";
                } else {
                    std::vector<std::string> matches = completer.match(line);
                    tab_counter++;

                    if (tab_counter == 1) {
                        if (matches.empty()) {
                            std::cout << "\x07";
                        } else {
                            std::string lcp = completer.longest_common_prefix(matches);
                            if (lcp.size() > line.size()) {
                                line = lcp;
                                redraw(line);

                                if (matches.size() == 1) {
                                    std::cout << "\033[1C";
                                    line += " ";
                                }
                                tab_counter = 0;
                            } else {
                                std::cout << "\x07";
                            }
                        }
                    } else if (tab_counter == 2) {
                        if (matches.empty()) {
                            std::cout << '\a' << std::flush;
                        } else {
                            std::cout << '\n';
                            for (const auto& it : matches) {
                                std::cout << it << " ";
                            }
                            std::cout << '\n';
                            redraw(line);
                        }
                        tab_counter = 0;
                    } else {
                        std::cout << "\x07";
                        tab_counter = 0;
                    }
                }
                std::cout << std::flush;
            } else {
                completer.load_files();
                if (completer.has_files()) {
                    std::stringstream ww(line);
                    std::vector<std::string> rec;
                    std::string word;

                    while (ww >> word) {
                        if (ww.peek() == EOF || ww.eof()) {
                            break;
                        }
                        rec.push_back(word);
                    }

                    std::string cmpt = completer.file_autocomplete(word);
                    if (cmpt.empty()) {
                        std::cout << "\x07" << std::flush;
                    } else {
                        std::string ne = "";
                        for (const auto& it : rec) {
                            ne += it;
                            ne += " ";
                        }
                        ne += cmpt;
                        line = ne;
                        redraw(line);
                        std::cout << "\033[1C";
                        line += " ";
                    }
                }
            }
        } else if (static_cast<int>(ch) == 127) { // Backspace
            if (!line.empty()) {
                line.pop_back();
            }
            redraw(line);
        } else if (static_cast<int>(ch) == 10) { // Enter
            std::cout << '\n' << std::flush;
            break;
        } else {
            line += ch;
            redraw(line);
        }
    }

    return true;
}
