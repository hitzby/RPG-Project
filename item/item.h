#pragma once
#include <string>
enum ItemType { Weapon = 1, Armor = 2, Potion = 3 };
class Item {
private:
  std::string name;
  int levelRequirement;
  ItemType type;
  int value;
  int durability;
  int price;
  int quantity;

public:
  Item(std::string name, int levelRequirement, ItemType type, int value,
       int durability, int price, int quantity);
  std::string getName() const;
  int getLevelRequirement() const;
  ItemType getType() const;
  int getValue() const;
  int getDurability() const;
  int getPrice() const;
  int getQuantity() const;
  void setQuantity(int quantity);
  void setDurability(int durability);
};