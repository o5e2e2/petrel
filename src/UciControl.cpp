#include "UciControl.hpp"
#include "UciOutput.hpp"

UciControl::UciControl (UciOutput& o) : SearchControl(o) {}

void UciControl::uciok() {
    static_cast<UciOutput&>(info).uciok( this->tt().getInfo() );
}

void UciControl::readyok() {
    static_cast<UciOutput&>(info).isready( this->isReady() );
}
