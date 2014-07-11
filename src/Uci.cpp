#include "Uci.hpp"

#include <cstdlib>
#include <fstream>
#include <string>

#include "Clock.hpp"
#include "OutputBuffer.hpp"
#include "SearchControl.hpp"
#include "PositionFen.hpp"
#include "PositionMoves.hpp"

std::ostream& operator << (std::ostream& out, Bb bb) {
    FOR_INDEX(Rank, rank) {
        FOR_INDEX(File, file) {
            out << (bb[Square(file, rank)]? file:'.');
        }
        out << '\n';
    }
    return out;
}

std::ostream& operator << (std::ostream& out, VectorPiSquare squares) {
    auto flags = out.flags();

    out << std::hex;
    FOR_INDEX(Pi, pi) {
        out << ' ' << pi;
    }
    out << std::endl;

    FOR_INDEX(Pi, pi) {
        out << std::setw(2) << static_cast<unsigned>(small_cast<unsigned char>((squares._v[pi])));
    }
    out << std::endl;

    out.flags(flags);
    return out;
}

namespace {
    std::ostream& program_version(std::ostream& out) {
        char year[] {__DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10], '\0'};

        char month[] {
            (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')? '1' :
            (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')? '1' :
            (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')? '1' : '0',

            (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')? '1' :
            (__DATE__[0] == 'F' && __DATE__[1] == 'e' && __DATE__[2] == 'b')? '2' :
            (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')? '3' :
            (__DATE__[0] == 'A' && __DATE__[1] == 'p' && __DATE__[2] == 'r')? '4' :
            (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')? '5' :
            (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')? '6' :
            (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')? '7' :
            (__DATE__[0] == 'A' && __DATE__[1] == 'u' && __DATE__[2] == 'g')? '8' :
            (__DATE__[0] == 'S' && __DATE__[1] == 'e' && __DATE__[2] == 'p')? '9' :
            (__DATE__[0] == 'O' && __DATE__[1] == 'c' && __DATE__[2] == 't')? '0' :
            (__DATE__[0] == 'N' && __DATE__[1] == 'o' && __DATE__[2] == 'v')? '1' :
            (__DATE__[0] == 'D' && __DATE__[1] == 'e' && __DATE__[2] == 'c')? '2' : '0',

            '\0'
        };

        char day[] {(__DATE__[4] == ' ')? '0':__DATE__[4], __DATE__[5], '\0'};

        return out << "Petrel chess " << year << '-' << month << '-' << day
#ifndef NDEBUG
            << " DEBUG"
#endif
        ;
    }
}

Uci::Uci (SearchControl& s, std::ostream& out)
    : start_position{}, search(s), uci_out(out), isready_waiting{false}
{
    ucinewgame();
}

void Uci::ucinewgame() {
    search.clear();
    search.clear_count();
    set_startpos();
}

void Uci::set_startpos() {
    std::istringstream startpos{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
    colorToMove = PositionFen{start_position}.readFen(startpos);
}

bool Uci::operator() (int argc, const char* argv[]) {
    if (argc == 1) {
        //construct startup configuration filename from program's own name
        std::string filename{argv[0]};
        {
            auto pos = filename.find_last_of('/');
            if (pos != std::string::npos) { filename.erase(0, pos+1); }

            pos = filename.find_first_of('.', 1);
            if (pos != std::string::npos) { filename.erase(pos); }

            filename.append(".rc");
        }

        return operator()(filename);
    }
    else {
        std::string option{argv[1]};
        if (argc > 2 || option == "--version" || option == "--help" || !operator()(option)) {
            help_and_quit();
        }
    }

    return true;
}

bool Uci::operator() (std::istream& uci_in, std::ostream& uci_err) {
    for (std::string command_line; std::getline(uci_in, command_line); ) {
        command.clear(); //clear stream error state from the previous usage
        command.str(std::move(command_line));

        if (command == "position")  { position(); }
        else if (command == "go")   { go(); }
        else if (command == "stop") { search.stop(); }
        else if (command == "isready")    { isready(); }
        else if (command == "setoption")  { setoption(); }
        else if (command == "ucinewgame") { ucinewgame(); }
        else if (command == "uci")  { uci(); }
        else if (command == "quit") { quit(); break; /* actually noreturn */ }
        else if (command == "wait") { search.wait(); }
        else if (command == "echo") { echo(); }
        else if (command == "call") { call(uci_err); }
        else if (command == "exit") { break; }
        else {
            auto first = command.peek();
            if (first == '#' || first == ';') {
                continue; //ignore comment
            }
        }

        ::fail_if_any(command);

        if (!command && !command.eof()) {
            OutputBuffer{uci_err} << "Parsing error " << command.rdbuf() << '\n';
        }
    }

    return !uci_in.bad();
}

bool Uci::operator() (const std::string& filename, std::ostream& uci_err) {
    std::ifstream file{filename};
    return file && operator()(file, uci_err);
}

void Uci::go() {
    search.go(*this, uci_out, command, start_position, colorToMove);
}

void Uci::quit() {
    std::exit(EXIT_SUCCESS);
}

void Uci::uci() {
    OutputBuffer{uci_out} << "id name " << program_version << '\n'
        << "id author Aleks Peshkov\n"
        << "option name UCI_Chess960 type check default " << (chessVariant == Chess960? "true":"false") << '\n'
        << "option name Hash type spin min 0 max " << search.ttMaxSize() << " default " << search.ttSize() << '\n'
        << "uciok\n"
    ;
}

void Uci::help_and_quit() {
    OutputBuffer{uci_out} << program_version << '\n'
        << "UCI chess engine.\n"
        << "Copyright (C) 2014 Aleks Peshkov, aleks.peshkov@gmail.com\n"
        << "This software and its source code are NOT free.\n"
        << "For terms of use contact author.\n"
    ;
    quit();
}

void Uci::setoption() {
    if (command == "name") {
        if (command == "UCI_Chess960") {
            if (command == "value") {
                if (command == "true") {
                    chessVariant = Chess960;
                    return;
                }
                else if (command == "false") {
                    chessVariant = Orthodox;
                    return;
                }
            }
        }
        if (command == "Hash") {
            if (command == "value") {
                TranspositionTable::megabytes_t Hash;
                if (command >> Hash) {
                    search.ttResize(Hash);
                    return;
                }
            }
        }
    }

    //anything else is error
    ::fail_here(command);
}

void Uci::echo() {
    command >> std::ws;
    OutputBuffer{uci_out} << command.rdbuf() << '\n';
}

void Uci::call(std::ostream& uci_err) {
    std::string filename;
    filename.clear();
    command >> filename;
    ::fail_if_not(operator()(filename, uci_err), command);
}

void Uci::position() {
    if (command == "startpos") {
        set_startpos();
    }
    else if (command == "fen") {
        colorToMove = PositionFen{start_position}.readFen(command);
    }

    if (command == "moves") {
        colorToMove = PositionMoves{start_position}.makeMoves(command, colorToMove);
    }
    else {
        ::fail_if_any(command);
    }
}

void Uci::isready() {
    if (search.isReady()) {
        OutputBuffer{uci_out} << "readyok\n";
    }
    else {
        isready_waiting = true;
    }
}

void Uci::write_info_current() {
    if (isready_waiting) {
        isready_waiting = false;

        OutputBuffer out{uci_out};
        //out << "info fen ";
        //write(out, start_position, colorToMove, chessVariant);
        //out << '\n';
        search.uci_info_nps(out);
        out << "readyok\n";
    }
}

void Uci::write(std::ostream& out, Move move) const {
    ::write(out, move, colorToMove, chessVariant);
}
