#include "item.h"

using namespace std;
Item::Item(string name, int levelRequirement, ItemType type, int value,
           int durability, int price, int quantity)
    : name(name), levelRequirement(levelRequirement), type(type), value(value),
      durability(durability), price(price), quantity(quantity) {}
string Item::getName() const { return name; }
int Item::getLevelRequirement() const { return levelRequirement; }
ItemType Item::getType() const { return type; }
int Item::getValue() const { return value; }
int Item::getDurability() const { return durability; }
int Item::getPrice() const { return price; }
int Item::getQuantity() const { return quantity; }
void Item::setQuantity(int quantity) { this->quantity = quantity; }
void Item::setDurability(int durability) { this->durability = durability; }