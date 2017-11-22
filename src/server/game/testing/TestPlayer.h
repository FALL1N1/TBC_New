#ifndef TEST_PLAYER_H
#define TEST_PLAYER_H

//same as Player but without db saving
class TC_GAME_API TestPlayer : public Player
{
public:
    explicit TestPlayer(WorldSession *session) : Player(session) {}
    virtual ~TestPlayer() {}

    virtual void SaveToDB(bool create = false) override {}
    virtual void SaveInventoryAndGoldToDB(SQLTransaction trans) override {}
    virtual void SaveGoldToDB(SQLTransaction trans) override {}
    virtual void SaveDataFieldToDB() override {}
};

#endif TEST_PLAYER_H