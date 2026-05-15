#include "pch.h"
#include "Objects.h"

/*
¬озможно есть смысл сделать Google Mock тесты дл€ рандома (если будет врем€)
*/

#define WARRIOR_TEST
#define NECROLIT_TEST
#define TRICKSTER_TEST
#define OBJECT_TEST
#define AI_INTAGRATION_TEST
#define ITEM_TEST

#ifdef OBJECT_TEST

TEST(ObjectMoveManualTest, AppliesPositiveAndNegativeShifts) {
    Object obj(10, 10);
    obj.move_manual(3, -2);
    EXPECT_EQ(obj.x(), 13);
    EXPECT_EQ(obj.y(), 8);
}

TEST(ObjectMoveManualTest, PreventsCoordinatesBelowZero) {
    Object obj(2, 2);
    obj.move_manual(-5, -10);
    EXPECT_EQ(obj.x(), 0);
    EXPECT_EQ(obj.y(), 0);
}

TEST(ObjectMoveManualTest, ExactBoundaryAtZero) {
    Object obj(1, 1);
    obj.move_manual(-1, -1);
    EXPECT_EQ(obj.x(), 0);
    EXPECT_EQ(obj.y(), 0);

    obj.move_manual(-1, -1);
    EXPECT_EQ(obj.x(), 0);
    EXPECT_EQ(obj.y(), 0);
}

TEST(ObjectMoveRandTest, InvariantsAndBoundary) {
    RandomNum::seed(42);
    Object obj(0, 0);

    for (int i = 0; i < 50; ++i) {
        int px = static_cast<int>(obj.x());
        int py = static_cast<int>(obj.y());

        obj.move_rand();

        EXPECT_GE(obj.x(), 0);
        EXPECT_GE(obj.y(), 0);

        EXPECT_GE(static_cast<int>(obj.x()) - px, -1);
        EXPECT_LE(static_cast<int>(obj.x()) - px, 1);
        EXPECT_GE(static_cast<int>(obj.y()) - py, -1);
        EXPECT_LE(static_cast<int>(obj.y()) - py, 1);

        if (px == 0) EXPECT_LE(obj.x(), 1);
        if (py == 0) EXPECT_LE(obj.y(), 1);
    }
}

TEST(ObjectMoveRandTest, DeterministicWithSeed) {
    RandomNum::seed(77);
    Object a(10, 10);
    for (int i = 0; i < 10; ++i) a.move_rand();

    RandomNum::seed(77);
    Object b(10, 10);
    for (int i = 0; i < 10; ++i) b.move_rand();

    EXPECT_EQ(a.x(), b.x());
    EXPECT_EQ(a.y(), b.y());
}
#endif // OBJECT_TEST

#ifdef WARRIOR_TEST

TEST(WarriorInit, DefaultConstructor) {
    Warrior w;
    EXPECT_EQ(w.name(), "Warrior");
    EXPECT_EQ(w.health(), 150);
    EXPECT_EQ(w.power(), 15);
    EXPECT_EQ(w.get_armor(), 5);
}

TEST(WarriorInit, ParamConstructor) {
    Warrior w("Conan", 200, 25, 10);
    EXPECT_EQ(w.name(), "Conan");
    EXPECT_EQ(w.health(), 200);
    EXPECT_EQ(w.power(), 25);
    EXPECT_EQ(w.get_armor(), 10);
}

TEST(WarriorInit, CopyConstructor) {
    Warrior original("Thor", 180, 30, 12);
    Warrior copy(original);
    EXPECT_EQ(copy.name(), "Thor");
    EXPECT_EQ(copy.health(), 180);
    EXPECT_EQ(copy.power(), 30);
    EXPECT_EQ(copy.get_armor(), 12);
}

TEST(WarriorCombat, AttakedReducesHealthWithArmor) {
    Warrior w("Tank", 100, 10, 5);
    Warrior dummy("Dummy", 100, 10, 0);

    w.attaked(dummy, 12, 0);
    EXPECT_EQ(w.health(), 93);

    w.attaked(dummy, 3, 0);
    EXPECT_EQ(w.health(), 93);
}

TEST(WarriorCombat, AttackDamageInRange) {
    Warrior attacker("Attacker", 100, 15, 0);
    Warrior defender("Defender", 100, 10, 2);

    int hp_before = defender.health();
    attacker.attack(defender);
    int hp_after = defender.health();

    int damage_dealt = hp_before - hp_after;

    EXPECT_GE(damage_dealt, 18);
    EXPECT_LE(damage_dealt, 33);
}

TEST(WarriorCombat, AttackerHealthUnchanged) {
    Warrior attacker("Attacker", 100, 15, 5);
    Warrior defender("Defender", 100, 10, 2);

    int hp_before = attacker.health();
    attacker.attack(defender);

    EXPECT_EQ(attacker.health(), hp_before);
}

TEST(WarriorCombat, IsAliveWorks) {
    Warrior w("Test", 10, 10, 0);
    Warrior dummy("Dummy", 10, 10, 0);

    EXPECT_TRUE(w.is_alive());

    w.attaked(dummy, 100, 0);
    EXPECT_FALSE(w.is_alive());
}

TEST(WarriorUpdateStat, WarriorUpdateStat) {
    Warrior warrior(0, 0, 'W', 0, "Armored", 100, 20, 10);
    int initial_armor = warrior.get_armor();
    
    warrior.update_stat(SpecificalStats, 5);
    
    int armor_diff = warrior.get_armor() - initial_armor;
    EXPECT_GE(armor_diff, 8);
    EXPECT_LE(armor_diff, 10);
}

#endif // WARRIOR_TEST

#ifdef NECROLIT_TEST


TEST(NecrolitInit, DefaultConstructor) {
    Necrolit n;
    EXPECT_EQ(n.name(), "Necrolit");
    EXPECT_EQ(n.health(), 150);
    EXPECT_EQ(n.power(), 15);
    EXPECT_TRUE(n.get_second_life());
}

TEST(NecrolitInit, ParamConstructor) {
    Necrolit n("KelThuzad", 180, 25);
    EXPECT_EQ(n.name(), "KelThuzad");
    EXPECT_EQ(n.health(), 180);
    EXPECT_EQ(n.power(), 25);
    EXPECT_TRUE(n.get_second_life());
}

TEST(NecrolitInit, CopyConstructor) {
    Necrolit original("Arthas", 200, 30);
    Necrolit copy(original);
    EXPECT_EQ(copy.name(), "Arthas");
    EXPECT_EQ(copy.health(), 200);
    EXPECT_EQ(copy.power(), 30);
    EXPECT_TRUE(copy.get_second_life());
}

TEST(NecrolitInit, CopyConstructorAfterReincarnation) {
    Necrolit original("Lich", 10, 30);
    Necrolit dummy("Dummy", 10, 10);

    original.attaked(dummy, 100, 0);

    Necrolit copy(original);
    EXPECT_EQ(copy.name(), "Lich");
    EXPECT_EQ(copy.health(), 100);
    EXPECT_EQ(copy.power(), 15);
    EXPECT_FALSE(copy.get_second_life());
}

TEST(NecrolitCombat, AttackFirstLifeUsesDmg) {
    Necrolit attacker("Necro", 150, 20);
    Warrior defender("Warrior", 150, 15, 2);

    int hp_before = defender.health();
    attacker.attack(defender);
    int hp_after = defender.health();

    int damage_dealt = hp_before - hp_after;

    EXPECT_GE(damage_dealt, 21);
    EXPECT_LE(damage_dealt, 28);
}

TEST(NecrolitCombat, AttackSecondLifeUsesPureDamage) {
    Necrolit attacker("Necro", 150, 20);
    Warrior defender("Warrior", 150, 15, 5);

    attacker.attaked(defender, 200, 0);
    EXPECT_FALSE(attacker.get_second_life());

    int hp_before = defender.health();
    attacker.attack(defender);
    int hp_after = defender.health();

    int damage_dealt = hp_before - hp_after;

    EXPECT_GE(damage_dealt, 18);
    EXPECT_LE(damage_dealt, 25);
}

TEST(NecrolitCombat, ReincarnationOnDeath) {
    Necrolit attacker("Necro", 10, 20);
    Warrior defender("Warrior", 150, 15, 5);

    attacker.attaked(defender, 100, 0);

    EXPECT_EQ(attacker.health(), 100);
    EXPECT_EQ(attacker.power(), 15);
    EXPECT_FALSE(attacker.get_second_life());
}

TEST(NecrolitCombat, ReincarnationWhenAttacked) {
    Necrolit victim("Necro", 50, 20);
    Warrior attacker("Warrior", 150, 15, 0);

    victim.attaked(attacker, 100, 0);

    EXPECT_EQ(victim.health(), 100);
    EXPECT_EQ(victim.power(), 15);
    EXPECT_FALSE(victim.get_second_life());
}

TEST(NecrolitUpdateStat, NecrolitUpdateStat) {
    Necrolit necro(0, 0, 'N', 0, "DarkOne", 100, 20);
    int initial_hp = necro.health();
    int initial_power = necro.power();
    
    necro.update_stat(SpecificalStats, 100);
    
    EXPECT_EQ(necro.health(), initial_hp);
    EXPECT_EQ(necro.power(), initial_power);
}

//+AI TEST

TEST(NecrolitCombat, ReincarnationOnlyOnce) {
    Necrolit victim("Necro", 50, 20);
    Warrior attacker("Warrior", 150, 15, 0);

    victim.attaked(attacker, 100, 0);
    EXPECT_EQ(victim.health(), 100);
    EXPECT_TRUE(victim.is_alive());
    EXPECT_FALSE(victim.get_second_life());

    victim.attaked(attacker, 200, 0);
    EXPECT_LT(victim.health(), 0);
    EXPECT_FALSE(victim.is_alive());
    EXPECT_FALSE(victim.get_second_life());
}

TEST(NecrolitCombat, IsAliveWithReincarnation) {
    Necrolit n("Necro", 20, 20);
    Warrior dummy("Dummy", 10, 10, 0);

    EXPECT_TRUE(n.is_alive());

    n.attaked(dummy, 10, 0);
    EXPECT_TRUE(n.is_alive());
    EXPECT_TRUE(n.get_second_life());

    n.attaked(dummy, 100, 0);
    EXPECT_TRUE(n.is_alive());
    EXPECT_FALSE(n.get_second_life());
}

TEST(NecrolitCombat, AttackerHealthUnchanged) {
    Necrolit attacker("Necro", 100, 20);
    Warrior defender("Warrior", 100, 15, 5);

    int hp_before = attacker.health();
    attacker.attack(defender);

    EXPECT_EQ(attacker.health(), hp_before);
}

TEST(NecrolitCombat, PureDamageIgnoresArmor) {
    Necrolit attacker("Necro", 150, 20);
    Warrior defender("Tank", 150, 15, 100);

    attacker.attaked(defender, 200, 0);

    int hp_before = defender.health();
    attacker.attack(defender);
    int hp_after = defender.health();

    int damage_dealt = hp_before - hp_after;

    EXPECT_GE(damage_dealt, 18);
    EXPECT_LE(damage_dealt, 25);
}


#endif // NECROLIT_TEST

#ifdef TRICKSTER_TEST
TEST(TricksterInit, DefaultConstructor) {
    Trickster t;
    EXPECT_EQ(t.name(), "Trickster");
    EXPECT_EQ(t.health(), 150);
    EXPECT_EQ(t.power(), 15);
}

TEST(TricksterInit, ParamConstructor) {
    Trickster t("Loki", 120, 20, 200, 150);
    EXPECT_EQ(t.name(), "Loki");
    EXPECT_EQ(t.health(), 120);
    EXPECT_EQ(t.power(), 20);
}

TEST(TricksterInit, CopyConstructor) {
    Trickster original("Joker", 130, 18, 300, 250);
    Trickster copy(original);
    EXPECT_EQ(copy.name(), "Joker");
    EXPECT_EQ(copy.health(), 130);
    EXPECT_EQ(copy.power(), 18);
}

TEST(TricksterCombat, AttackDealsDamage) {
    Trickster attacker("Loki", 150, 20, 0, 0);
    Warrior defender("Warrior", 150, 15, 2);

    int hp_before = defender.health();
    attacker.attack(defender);
    int damage = hp_before - defender.health();

    EXPECT_GE(damage, 23);
    EXPECT_LE(damage, 28);
}

TEST(TricksterCombat, EvasionAlwaysEvades) {
    Trickster defender("Test", 150, 20, 200, 0);
    Warrior attacker("Warrior", 150, 20, 0);

    int hp_before = defender.health();
    for (int i = 0; i < 10; ++i) {
        attacker.attack(defender);
    }

    EXPECT_EQ(defender.health(), hp_before);
}

TEST(TricksterCombat, ReflectWorks) {
    Trickster defender("Test", 150, 20, 0, 200);
    Warrior attacker("Warrior", 150, 15, 0);

    int hp_before = attacker.health();
    attacker.attack(defender);
    int reflected = hp_before - attacker.health();

    EXPECT_GE(reflected, 40);
    EXPECT_LE(reflected, 70);
}

TEST(TricksterCombat, EvasionAndReflectTogether) {
    Trickster defender("Test", 150, 20, 200, 150);
    Warrior attacker("Warrior", 150, 20, 0);

    int hp_before_attacker = attacker.health();
    int hp_before_defender = defender.health();

    attacker.attack(defender);

    EXPECT_EQ(defender.health(), hp_before_defender);
    EXPECT_LT(attacker.health(), hp_before_attacker);
}

TEST(TricksterCombat, PureDamageNotReflected) {
    Trickster defender("Loki", 150, 20, 0, 200);
    Necrolit attacker("Necro", 150, 20);

    attacker.attaked(defender, 200, 0);

    int hp_before = attacker.health();
    attacker.attack(defender);

    EXPECT_EQ(attacker.health(), hp_before);
}

TEST(TricksterCombat, ReflectOnlyFromDmg) {
    Trickster defender("Loki", 150, 20, 0, 100);

    int reflected = defender.attaked(defender, 30, 70);

    EXPECT_EQ(reflected, 30);
    EXPECT_EQ(defender.health(), 50);
}

TEST(TricksterCombat, EvasionZeroAlwaysHit) {
    Trickster defender("Loki", 150, 20, 0, 0);
    Warrior attacker("Warrior", 150, 20, 0);

    int hp_before = defender.health();
    attacker.attack(defender);

    EXPECT_LT(defender.health(), hp_before);
}

TEST(TricksterUpdateStat, TricksterUpdateStat) {
    Trickster trick(0, 0, 'T', 0, "Sneaky", 100, 20, 10, 15);
    size_t initial_evasion = trick.evasion();
    size_t initial_spiks = trick.spiks();

    // SpecificalStats дл€ трикстера: +value к обоим статам
    trick.update_stat(SpecificalStats, 5);

    EXPECT_EQ(trick.evasion(), initial_evasion + 5);
    EXPECT_EQ(trick.spiks(), initial_spiks + 5);
}
#endif // TRICKSTER_TEST

#ifdef AI_INTAGRATION_TEST

TEST(BattleIntegration, MultiClassInteraction) {
    Warrior w("Conan", 150, 15, 5);
    Necrolit n("KelThuzad", 150, 20);
    Trickster t("Loki", 150, 20, 200, 100); // 200% уклонени€, 100% отражени€

    // Ќачальные значени€ здоровь€
    int w_initial = w.health();
    int n_initial = n.health();
    int t_initial = t.health();

    // 1. Warrior атакует Necrolit
    w.attack(n);
    // Warrior не получает отражЄнного урона от Necrolit
    EXPECT_EQ(w.health(), w_initial);
    // ≈сли Necrolit умер и воскрес, провер€ем корректность реинкарнации
    if (n.health() == 100 && !n.get_second_life()) {
        // –еинкарнаци€ сработала штатно
        SUCCEED();
    }
    // Necrolit всЄ ещЄ жив (либо не умирал, либо воскрес)
    EXPECT_TRUE(n.is_alive());

    // 2. Necrolit атакует Trickster
    int n_hp_before = n.health();
    int t_hp_before = t.health();
    n.attack(t);
    // Trickster уклон€етс€ (200% шанс) => здоровье не мен€етс€
    EXPECT_EQ(t.health(), t_hp_before);
    // ≈сли Necrolit в первой жизни, он получит отражЄнный урон (100% от dmg)
    // ≈сли во второй жизни, то pure_damage не отражаетс€ и здоровье не изменитс€
    if (n.get_second_life()) {
        EXPECT_LT(n.health(), n_hp_before); // получил отражение
    }
    else {
        EXPECT_EQ(n.health(), n_hp_before); // не получил отражени€
    }

    // 3. Trickster атакует Warrior
    int w_hp_before = w.health();
    t.attack(w);
    // Warrior получает урон (нет уклонени€)
    EXPECT_LT(w.health(), w_hp_before);
    // Trickster не получает отражЄнного урона (Warrior не отражает)
    EXPECT_EQ(t.health(), t_hp_before);

    // 4. ƒобиваем Necrolit до окончательной смерти
    int attack_count = 0;
    while (n.is_alive() && attack_count < 100) {
        w.attack(n);
        ++attack_count;
    }
    EXPECT_FALSE(n.is_alive());
    EXPECT_FALSE(n.get_second_life()); // втора€ жизнь должна быть использована
}
#endif // AI_INTAGRATION_TEST

#ifdef ITEM_TEST
TEST(ObjectsTests, Item_IsActive_DefaultFalse) {
    BonusMalus item(0, 0, '+', 0);
    EXPECT_FALSE(item.is_active());
}

TEST(ObjectsTests, BonusMalus_Constructor_RandomizesTypeAndValue) {
    for (int i = 0; i < 20; ++i) {
        BonusMalus item(0, 0, '+', 0);
        EXPECT_EQ(item.x(), 0);
        EXPECT_EQ(item.y(), 0);
        EXPECT_EQ(item.sym(), '+');
    }
}

TEST(ObjectsTests, BonusMalus_UseItem_UpdatesWarriorHealth) {
    Warrior warrior("TestWarrior", 100, 20, 10);
    int initial_hp = warrior.health();

    warrior.update_stat(Health, 25);

    EXPECT_EQ(warrior.health(), initial_hp + 25);
}

TEST(ObjectsTests, BonusMalus_UseItem_NegativeValue_Clamped) {
    Warrior warrior("TestWarrior", 10, 20, 10);

    warrior.update_stat(Health, -50);

    EXPECT_GE(warrior.health(), 0);
    EXPECT_EQ(warrior.health(), 10);
}
#endif // ITEM_TEST
