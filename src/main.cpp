#include <swiftly/swiftly.h>
#include <swiftly/server.h>
#include <swiftly/database.h>
#include <swiftly/commands.h>
#include <swiftly/configuration.h>
#include <swiftly/logger.h>
#include <swiftly/timers.h>
#include <swiftly/gameevents.h>
#include <swiftly/exports.h>

Server *server = nullptr;
PlayerManager *g_playerManager = nullptr;
Database *db = nullptr;
Commands *commands = nullptr;
Configuration *config = nullptr;
Logger *logger = nullptr;
Timers *timers = nullptr;
Exports *exports = nullptr;

void OnProgramLoad(const char *pluginName, const char *mainFilePath)
{
    Swiftly_Setup(pluginName, mainFilePath);

    server = new Server();
    g_playerManager = new PlayerManager();
    commands = new Commands(pluginName);
    config = new Configuration();
    logger = new Logger(mainFilePath, pluginName);
    timers = new Timers();
    exports = new Exports(pluginName);
}

void UpdatePlayer(Player *player, int type, int value)
{

    // TYPE 1 = POINTS
    // TYPE 2 = KILLS
    // TYPE 3 = DEATHS
    // TYPE 4 = ASSISTS

    if (!db->IsConnected())
        return;

    if (type == 1)
    {
        db->Query("UPDATE %s SET points = %d WHERE steamid = '%llu'", "ranks", value, player->GetSteamID());
        player->vars->Set("points", value);
    }
    else if (type == 2)
    {
        db->Query("UPDATE %s SET kills = %d WHERE steamid = '%llu'", "ranks", value, player->GetSteamID());
        player->vars->Set("kills", value);
    }
    else if (type == 3)
    {
        db->Query("UPDATE %s SET deaths = %d WHERE steamid = '%llu'", "ranks", value, player->GetSteamID());
        player->vars->Set("deaths", value);
    }
    else if (type == 4)
    {
        db->Query("UPDATE %s SET assists = %d WHERE steamid = '%llu'", "ranks", value, player->GetSteamID());
        player->vars->Set("assists", value);
    }
}

int FetchPlayer(Player *player, const char *type)
{
    if (!db->IsConnected())
        return 0;

    return player->vars->Get<int>(type);
}

void OnPlayerSpawn(Player *player)
{
    if (!db->IsConnected())
        return;

    if (player->IsFirstSpawn() && !player->IsFakeClient())
    {
        db->Query("insert ignore into `ranks` (steamid, name) values ('%llu', '%s')", player->GetSteamID(), player->GetName());
    }
}

void OnClientFullConnected(Player *player)
{
    if (!db->IsConnected())
        return;

    if (player)
    {
        int points = 0;
        int kills = 0;
        int deaths = 0;
        int assists = 0;
        DB_Result result = db->Query("SELECT * FROM %s where steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
        if (result.size() > 0)
        {
            points = db->fetchValue<int>(result, 0, "points");
            kills = db->fetchValue<int>(result, 0, "kills");
            deaths = db->fetchValue<int>(result, 0, "deaths");
            assists = db->fetchValue<int>(result, 0, "assists");
        }
        player->vars->Set("points", points);
        player->vars->Set("kills", kills);
        player->vars->Set("deaths", deaths);
        player->vars->Set("assists", assists);
    }
}

void Command_Top(int playerID, const char **args, uint32_t argsCount, bool silent)
{
    if (playerID == -1)
        return;

    if (!db->IsConnected())
        return;

    Player *player = g_playerManager->GetPlayer(playerID);

    if (player == nullptr)
        return;

    DB_Result result = db->Query("SELECT name, points FROM %s ORDER BY points DESC LIMIT 10", "ranks");
    int size = result.size();
    if (size > 0)
    {
        for (int i = 0; i < size; i++)
        {
            const char *name = db->fetchValue<const char *>(result, i, "name");
            int points = db->fetchValue<int>(result, i, "points");

            player->SendMsg(HUD_PRINTTALK, "{DEFAULT}%d. {LIGHTBLUE}%s - {LIME}Experience: {LIGHTBLUE}%d.", i + 1, name, points);
        }
    }
}

void Command_GivePoints(int playerID, const char **args, uint32_t argsCount, bool silent)
{
    if (playerID == -1)
        return;

    if (!db->IsConnected())
        return;

    Player *player = g_playerManager->GetPlayer(playerID);

    if (player == nullptr)
        return;

    int access = exports->Call<bool>("swiftly_admins", "HasFlags", player->GetSlot(), config->Fetch<const char *>("swiftly_ranks.Admin.Flag"));
    if (access == 0)
    {
        char buffer[256];
        sprintf(buffer, "%s %s", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.NoAccess"));
        player->SendMsg(HUD_PRINTTALK, buffer);
        return;
    }

    if (argsCount < 2)
    {
        player->SendMsg(HUD_PRINTTALK, "%s Usage: !givepoints player points", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"));
        return;
    }

    auto target = GetPlayerId(args[0]);
    int points = atoi(args[1]);

    if (points <= 0)
    {
        player->SendMsg(HUD_PRINTTALK, "%s Points must be greater than zero.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"));
        return;
    }

    Player *targetplayer = g_playerManager->GetPlayer(target);
    int TargetPoints = FetchPlayer(targetplayer, "points");

    UpdatePlayer(targetplayer, 1, TargetPoints + points);
    targetplayer->SendMsg(HUD_PRINTTALK, "%s You have received %d credits from: %s", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), points, player->GetName());
    player->SendMsg(HUD_PRINTTALK, "%s You gave %d points to: %s", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), points, targetplayer->GetName());
    return;
}

void Command_GivePointsToAll(int playerID, const char **args, uint32_t argsCount, bool silent)
{
    if (playerID == -1)
        return;

    if (!db->IsConnected())
        return;

    Player *player = g_playerManager->GetPlayer(playerID);

    if (player == nullptr)
        return;

    int access = exports->Call<bool>("swiftly_admins", "HasFlags", player->GetSlot(), config->Fetch<const char *>("swiftly_ranks.Admin.Flag"));
    if (access == 0)
    {
        char buffer[256];
        sprintf(buffer, "%s %s", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.NoAccess"));
        player->SendMsg(HUD_PRINTTALK, buffer);
        return;
    }

    if (argsCount < 1)
    {
        player->SendMsg(HUD_PRINTTALK, "%s Usage: !givepointstoall points", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"));
        return;
    }

    int points = atoi(args[0]);

    if (points <= 0)
    {
        player->SendMsg(HUD_PRINTTALK, "%s Points must be greater than zero.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"));
        return;
    }

    for (int i = 0; i < g_playerManager->GetPlayerCap(); i++)
    {
        Player *AllPlayers = g_playerManager->GetPlayer(i);
        if (!AllPlayers)
            continue;

        int pPoints = FetchPlayer(AllPlayers, "points");
        UpdatePlayer(AllPlayers, 1, points + pPoints);
        g_playerManager->SendMsg(HUD_PRINTTALK, "%s You have received %d credits", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), points);
        return;
    }
}

void Command_Rank(int playerID, const char **args, uint32_t argsCount, bool silent)
{
    if (playerID == -1)
        return;

    if (!db->IsConnected())
        return;

    Player *player = g_playerManager->GetPlayer(playerID);

    if (player == nullptr)
        return;

    int points = FetchPlayer(player, "points");
    int kills = FetchPlayer(player, "kills");
    int deaths = FetchPlayer(player, "deaths");
    int assists = FetchPlayer(player, "assists");
    float kda = static_cast<float>(kills + assists) / static_cast<float>(deaths);

    std::string rank = "Unranked";

    const std::string rankNames[] = {
        "Silver 1", "Silver 2", "Silver 3", "Silver 4", "Silver 5",
        "GN1", "GN2", "GN3", "GN4",
        "MG1", "MG2", "MGE",
        "DMG", "LE", "LEM",
        "SMFC", "Global Elite"};

    for (const std::string &rankName : rankNames)
    {
        int threshold = config->Fetch<int>(("swiftly_ranks.Ranks." + rankName).c_str());
        if (points >= threshold)
        {
            rank = rankName;
        }
        else
        {
            break;
        }
    }

    const char *prefix = config->Fetch<const char *>("swiftly_ranks.Settings.Prefix");
    char buffer[512];

    print(rank.c_str());

    player->SendMsg(HUD_PRINTTALK, "%s -------------------------------------------------------------------", prefix);
    sprintf(buffer, "%s %s.", prefix, FetchTranslation("swiftly_ranks.DisplayEXP"));
    player->SendMsg(HUD_PRINTTALK, buffer, points, rank.c_str());
    sprintf(buffer, "%s %s.", prefix, FetchTranslation("swiftly_ranks.DisplayStats"));
    player->SendMsg(HUD_PRINTTALK, buffer, kills, deaths, assists);
    sprintf(buffer, "%s %s.", prefix, FetchTranslation("swiftly_ranks.DisplayKD"));
    player->SendMsg(HUD_PRINTTALK, buffer, kda);
    player->SendMsg(HUD_PRINTTALK, "%s -------------------------------------------------------------------", prefix);
}

void OnPlayerDeath(Player *player, Player *attacker, Player *assister, bool assistedflash, const char *weapon, bool headshot, short dominated, short revenge, short wipe, short penetrated, bool noreplay, bool noscope, bool thrusmoke, bool attackerblind, float distance, short dmg_health, short dmg_armor, short hitgroup)
{
    int normalkill = config->Fetch<int>("swiftly_ranks.Points.KillPoints");
    int headshotkill = config->Fetch<int>("swiftly_ranks.Points.HeadshotPoints");
    int noscopekill = config->Fetch<int>("swiftly_ranks.Points.NoscopeKill");
    int death = config->Fetch<int>("swiftly_ranks.Points.DeathPoints");
    int assist = config->Fetch<int>("swiftly_Ranks.Points.AssistPoints");

    int playerassists = FetchPlayer(player, "assists");

    int attackerpoints = 0;
    int attackerkills = 0;
    int playerpoints = 0;
    int playerdeaths = 0;
    int assisterpoints = 0;

    if (attacker == nullptr || player == nullptr)
        return;

    if (attacker)
    {
        attackerpoints = FetchPlayer(attacker, "points");
        attackerkills = FetchPlayer(attacker, "kills");
    }

    if (player)
    {
        playerpoints = FetchPlayer(player, "points");
        playerdeaths = FetchPlayer(player, "deaths");
    }

    if (assister)
    {
        assisterpoints = FetchPlayer(assister, "points");
    }

    if (attacker == player)
        return;

    if (headshot && attacker)
    {
        char buffer[256];
        sprintf(buffer, "%s %s.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.HeadshotKill"));
        attacker->SendMsg(HUD_PRINTTALK, buffer, attackerpoints + headshotkill, headshotkill);
        UpdatePlayer(attacker, 1, attackerpoints + headshotkill);
        UpdatePlayer(attacker, 2, attackerkills + 1);
        if (playerpoints > death)
        {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%s %s.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.Death"));
            player->SendMsg(HUD_PRINTTALK, buffer, playerpoints - death, death);
            UpdatePlayer(player, 1, playerpoints - death);
            UpdatePlayer(player, 3, playerdeaths + 1);
        }
    }
    else if (noscope && attacker)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s %s.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.NoscopeKill"));
        attacker->SendMsg(HUD_PRINTTALK, buffer, attackerpoints + noscopekill, noscopekill);
        UpdatePlayer(attacker, 1, attackerpoints + noscopekill);
        UpdatePlayer(attacker, 2, attackerkills + 1);
        if (playerpoints > death)
        {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%s %s.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.Death"));
            player->SendMsg(HUD_PRINTTALK, buffer, playerpoints - death, death);
            UpdatePlayer(player, 1, playerpoints - death);
            UpdatePlayer(player, 3, playerdeaths + 1);
        }
    }
    else if (attacker)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s %s.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.NormalKill"));
        attacker->SendMsg(HUD_PRINTTALK, buffer, attackerpoints + normalkill, normalkill);
        UpdatePlayer(attacker, 1, attackerpoints + normalkill);
        UpdatePlayer(attacker, 2, attackerkills + 1);
        if (playerpoints > death)
        {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%s %s.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.Death"));
            player->SendMsg(HUD_PRINTTALK, buffer, playerpoints - death, death);
            UpdatePlayer(player, 1, playerpoints - death);
            UpdatePlayer(player, 3, playerdeaths + 1);
        }
    }

    if (assister)
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s %s.", config->Fetch<const char *>("swiftly_ranks.Settings.Prefix"), FetchTranslation("swiftly_ranks.AssistKill"));
        assister->SendMsg(HUD_PRINTTALK, buffer, assist);
        UpdatePlayer(assister, 1, assisterpoints + assist);
        UpdatePlayer(assister, 4, playerassists + 1);
    }
}

void OnPluginStart()
{
    commands->Register("rank", reinterpret_cast<void *>(&Command_Rank));
    commands->Register("top", reinterpret_cast<void *>(&Command_Top));

    // ADMIN COMMANDS
    commands->Register("givepoints", reinterpret_cast<void *>(&Command_GivePoints));
    commands->Register("givepointstoall", reinterpret_cast<void *>(&Command_GivePointsToAll));

    db = new Database("swiftly_ranks");

    DB_Result result = db->Query("CREATE TABLE IF NOT EXISTS `ranks` (`steamid` varchar(1297) NOT NULL, `name` varchar(128) NOT NULL, `points` int(11) NOT NULL, `kills` int(11) NOT NULL, `deaths` int(11) NOT NULL, `assists` int(11) NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;");
    if (result.size() > 0)
    {
        db->Query("ALTER TABLE `ranks` ADD UNIQUE KEY `steamid` (`steamid`);");
    }
}

void OnPluginStop()
{
}

const char *GetPluginAuthor()
{
    return "Swiftly-Solutions";
}

const char *GetPluginVersion()
{
    return "1.0.0";
}

const char *GetPluginName()
{
    return "swiftly_rank";
}

const char *GetPluginWebsite()
{
    return "swiftlycs2.net";
}