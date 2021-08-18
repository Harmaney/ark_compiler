#include "ccvm.h"

int TagTable::next_slot;

void TagTable::init() { TagTable::next_slot = 0; }

std::string* TagTable::create_tag_G() {
    return new std::string("L" + std::to_string(TagTable::next_slot++));
}