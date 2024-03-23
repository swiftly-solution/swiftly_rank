#include <swiftly/swiftly.h>
#include <swiftly/server.h>
#include <swiftly/database.h>
#include <swiftly/commands.h>
#include <swiftly/configuration.h>
#include <swiftly/logger.h>
#include <swiftly/timers.h>
#include <swiftly/gameevents.h>

Server *server = nullptr;
PlayerManager *g_playerManager = nullptr;
Database *db = nullptr;
Commands *commands = nullptr;
Configuration *config = nullptr;
Logger *logger = nullptr;
Timers *timers = nullptr;

void OnProgramLoad(const char *pluginName, const char *mainFilePath)
{
    Swiftly_Setup(pluginName, mainFilePath);

    server = new Server();
    g_playerManager = new PlayerManager();
    commands = new Commands(pluginName);
    config = new Configuration();
    logger = new Logger(mainFilePath, pluginName);
    timers = new Timers();
}

void OnPlayerSpawn(Player *player)
{
    if (!db->IsConnected())
        return;

    if (player->IsFirstSpawn() && !player->IsFakeClient())
        db->Query("insert ignore into `ranks` (steamid, name) values ('%llu', '%s')", player->GetSteamID(), player->GetName());

    bool ScoreboardRanks = config->Fetch<bool>("swiftly_ranks.EnableScoreboardRanks");

    if (ScoreboardRanks == true) {
        DB_Result result = db->Query("SELECT points FROM %s WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
        int PointsCommand = 0;
        if(result.size() > 0) {
            PointsCommand = db->fetchValue<int>(result, 0, "points");
        }
        int Silver1 = config->Fetch<int>("swiftly_ranks.Ranks.Silver1");
        int Silver2 = config->Fetch<int>("swiftly_ranks.Ranks.Silver2");
        int Silver3 = config->Fetch<int>("swiftly_ranks.Ranks.Silver3");
        int Silver4 = config->Fetch<int>("swiftly_ranks.Ranks.Silver4");
        int Silver5 = config->Fetch<int>("swiftly_ranks.Ranks.Silver5");

        int Gold1 = config->Fetch<int>("swiftly_ranks.Ranks.Gold1");
        int Gold2 = config->Fetch<int>("swiftly_ranks.Ranks.Gold2");
        int Gold3 = config->Fetch<int>("swiftly_ranks.Ranks.Gold3");
        int Gold4 = config->Fetch<int>("swiftly_ranks.Ranks.Gold4");

        int MasterGuardian1 = config->Fetch<int>("swiftly_ranks.Ranks.MG1");
        int MasterGuardian2 = config->Fetch<int>("swiftly_ranks.Ranks.MG2");
        int MasterGuardianElite = config->Fetch<int>("swiftly_ranks.Ranks.MGE");

        int DMG = config->Fetch<int>("swiftly_ranks.Ranks.DMG");
        int LE = config->Fetch<int>("swiftly_ranks.Ranks.LE");
        int LEM = config->Fetch<int>("swiftly_ranks.Ranks.LEM");
        int SMFC = config->Fetch<int>("swiftly_ranks.Ranks.SMFC");

        int GE = config->Fetch<int>("swiftly_ranks.Ranks.GE");

        if (PointsCommand < Silver1) {
            player->clantag->Set("Unranked");
        } else if (PointsCommand >= Silver1 && PointsCommand < Silver2) {
            player->clantag->Set("[Silver 1]");
        } else if (PointsCommand >= Silver1 && PointsCommand < Silver3) {
            player->clantag->Set("[Silver 2]");
        } else if (PointsCommand >= Silver1 && PointsCommand < Silver4) {
            player->clantag->Set("[Silver 3]");
        } else if (PointsCommand >= Silver1 && PointsCommand < Silver5) {
            player->clantag->Set("[Silver 4]");
        } else if (PointsCommand >= Silver1 && PointsCommand < Gold1) {
            player->clantag->Set("[Silver Elite]");
        } else if (PointsCommand >= Silver1 && PointsCommand < Gold2) {
            player->clantag->Set("[Gold Nova 1]");
        } else if (PointsCommand >= Silver1 && PointsCommand < Gold3) {
            player->clantag->Set("[Gold Nova 2]");
        } else if (PointsCommand >= Silver1 && PointsCommand < Gold4) {
            player->clantag->Set("[Gold Nova 3]");
        } else if (PointsCommand >= Silver1 && PointsCommand < MasterGuardian1) {
            player->clantag->Set("[Gold Nova 4]");
        } else if (PointsCommand >= Silver1 && PointsCommand < MasterGuardian2) {
            player->clantag->Set("[MG1]");
        } else if (PointsCommand >= Silver1 && PointsCommand < MasterGuardianElite) {
            player->clantag->Set("[MG2]");
        } else if (PointsCommand >= MasterGuardianElite && PointsCommand < DMG) {
            player->clantag->Set("[MGE]");
        } else if (PointsCommand >= DMG && PointsCommand < LE) {
            player->clantag->Set("[DMG]");
        } else if (PointsCommand >= LE && PointsCommand < LEM) {
            player->clantag->Set("[LE]");
        } else if (PointsCommand >= LEM && PointsCommand < SMFC) {
            player->clantag->Set("[LEM]");
        } else if (PointsCommand >= SMFC && PointsCommand < GE) {
            player->clantag->Set("[SMFC]");
        } else if (PointsCommand >= GE) {
            player->clantag->Set("[GE]");
        }
    }

}



void Command_Ranks(int playerID, const char **args, uint32_t argsCount, bool silent)
{
     if (playerID == -1)
        return;
    if (!db->IsConnected())
        return;
        Player *player = g_playerManager->GetPlayer(playerID);
        if (player == nullptr)
        return;

    DB_Result result = db->Query("SELECT points, kills, deaths, assists FROM %s WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
    int PointsCommand = 0, KillsCommand = 0, DeathsCommand = 0, AssistsCommand = 0;
    if(result.size() > 0) {
        PointsCommand = db->fetchValue<int>(result, 0, "points");
        KillsCommand = db->fetchValue<int>(result, 0, "kills");
        DeathsCommand = db->fetchValue<int>(result, 0, "deaths");
        AssistsCommand = db->fetchValue<int>(result, 0, "assists");
    }
    float ratio = 0.0f;
    if (DeathsCommand != 0) {
        ratio = float(KillsCommand) / float(DeathsCommand);
    }
    int Silver1 = config->Fetch<int>("swiftly_ranks.Ranks.Silver1");
    int Silver2 = config->Fetch<int>("swiftly_ranks.Ranks.Silver2");
    int Silver3 = config->Fetch<int>("swiftly_ranks.Ranks.Silver3");
    int Silver4 = config->Fetch<int>("swiftly_ranks.Ranks.Silver4");
    int Silver5 = config->Fetch<int>("swiftly_ranks.Ranks.Silver5");

    int Gold1 = config->Fetch<int>("swiftly_ranks.Ranks.Gold1");
    int Gold2 = config->Fetch<int>("swiftly_ranks.Ranks.Gold2");
    int Gold3 = config->Fetch<int>("swiftly_ranks.Ranks.Gold3");
    int Gold4 = config->Fetch<int>("swiftly_ranks.Ranks.Gold4");

    int MasterGuardian1 = config->Fetch<int>("swiftly_ranks.Ranks.MG1");
    int MasterGuardian2 = config->Fetch<int>("swiftly_ranks.Ranks.MG2");
    int MasterGuardianElite = config->Fetch<int>("swiftly_ranks.Ranks.MGE");

    int DMG = config->Fetch<int>("swiftly_ranks.Ranks.DMG");
    int LE = config->Fetch<int>("swiftly_ranks.Ranks.LE");
    int LEM = config->Fetch<int>("swiftly_ranks.Ranks.LEM");
    int SMFC = config->Fetch<int>("swiftly_ranks.Ranks.SMFC");

    int GE = config->Fetch<int>("swiftly_ranks.Ranks.GE");


    if (PointsCommand < Silver1) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and unranked.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Silver2) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Silver 1.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Silver3) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Silver 2.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Silver4) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Silver 3.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Silver5) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s  {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Silver 4.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Gold1) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Silver Elite.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Gold2) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Gold Nova 1.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Gold3) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Gold Nova 2.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < Gold4) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Gold Nova 3.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < MasterGuardian1) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and Gold Nova 4.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < MasterGuardian2) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and MG1.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= Silver1 && PointsCommand < MasterGuardianElite) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and MG2.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= MasterGuardianElite && PointsCommand < DMG) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and MGE.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= DMG && PointsCommand < LE) {
    player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and DMG.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= LE && PointsCommand < LEM) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s  {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and LE.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= LEM && PointsCommand < SMFC) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and LEM.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= SMFC && PointsCommand < GE) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and SMFC.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    } else if (PointsCommand >= GE) {
        player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Player {RED}%s {default} has {red} %d {default} points, %d kills, %d deaths, and %d assists with a %.2f K/D Ratio and GE.", FetchTranslation("swiftly_ranks.prefix"), player->GetName(), PointsCommand, KillsCommand, DeathsCommand, AssistsCommand, ratio);
    }
}

void Command_Top(int playerID, const char **args, uint32_t argsCount, bool silent)
{
    if (!db->IsConnected())
        return;

    Player *player = g_playerManager->GetPlayer(playerID);
    if (player == nullptr)
        return;

    char query[256];
    sprintf(query, "SELECT name, points FROM %s ORDER BY points DESC LIMIT 10", "ranks");
    DB_Result result = db->Query(query);


    for (int i = 0; i < result.size(); i++) {
        const char* name = db->fetchValue<const char*>(result, i, "name");
        int points = db->fetchValue<int>(result, i, "points");

        player->SendMsg(HUD_PRINTTALK, "{DEFAULT}%d. {LIGHTBLUE}%s - {LIME}Experience: {LIGHTBLUE}%d.", i+1, name, points);
    }

}


void OnPluginStart()
{
        commands->Register("rank", reinterpret_cast<void *>(&Command_Ranks));
        commands->Register("top", reinterpret_cast<void *>(&Command_Top));

        db = new Database("swiftly_ranks");

        DB_Result result = db->Query("CREATE TABLE IF NOT EXISTS `ranks` (`steamid` varchar(128) NOT NULL, `points` int(11) NOT NULL DEFAULT 0, `kills` int(11) NOT NULL DEFAULT 0, `name` varchar(128) NOT NULL, `deaths` int(11) NOT NULL DEFAULT 0, `assists` int(11) NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;");
        
        if (result.size() > 0)
            db->Query("ALTER TABLE `ranks` ADD UNIQUE KEY `steamid` (`steamid`);");

}



void OnPlayerDeath(Player *player, Player *attacker, Player *assister, bool assistedflash, const char *weapon, bool headshot, short dominated, short revenge, short wipe, short penetrated, bool noreplay, bool noscope, bool thrusmoke, bool attackerblind, float distance, short dmg_health, short dmg_armor, short hitgroup)
{
    DB_Result resultPlayer = db->Query("SELECT points FROM %s WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
    DB_Result resultAttacker;
    int currentPointsPlayer = 0;
    int currentPointsAttacker = 0;
    if(resultPlayer.size() > 0) {
        currentPointsPlayer = db->fetchValue<int>(resultPlayer, 0, "points");
    }

    if(attacker) {
        resultAttacker = db->Query("SELECT points FROM %s WHERE steamid = '%llu' LIMIT 1", "ranks", attacker->GetSteamID());
        if(resultAttacker.size() > 0) {
            currentPointsAttacker = db->fetchValue<int>(resultAttacker, 0, "points");
        }
    }




    if(player == attacker) {
        if(currentPointsPlayer > 0) {
             attacker->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[- %d for suicide]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsPlayer - config->Fetch<int>("swiftly_ranks.Suicide"), config->Fetch<int>("swiftly_ranks.Suicide"));
            db->Query("UPDATE %s SET points = points - %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.Suicide"), player->GetSteamID());
            db->Query("UPDATE %s SET deaths = deaths + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
        }
    }
    else if(headshot && attacker) {
        attacker->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[+ %d for headshot]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsAttacker + config->Fetch<int>("swiftly_ranks.HeadShotKill"), config->Fetch<int>("swiftly_ranks.HeadShotKill"));
        db->Query("UPDATE %s SET points = points + %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.HeadShotKill"), attacker->GetSteamID());
        db->Query("UPDATE %s SET kills = kills + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", attacker->GetSteamID());
        if(currentPointsPlayer > 0) {
            player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[- %d for dying]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsPlayer - config->Fetch<int>("swiftly_ranks.Suicide"), config->Fetch<int>("swiftly_ranks.Death"));
            db->Query("UPDATE %s SET points = points - %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.Death"), player->GetSteamID());
            db->Query("UPDATE %s SET deaths = deaths + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
        }
    }
    else if(noscope && attacker) {
        attacker->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[+ %d for noscope]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsAttacker + config->Fetch<int>("swiftly_ranks.NoScopeKill"), config->Fetch<int>("swiftly_ranks.NoScopeKill"));
        db->Query("UPDATE %s SET points = points + %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.NoScopeKill"), attacker->GetSteamID());
        db->Query("UPDATE %s SET kills = kills + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", attacker->GetSteamID());
        if(currentPointsPlayer > 0) {
            player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[- %d for dying]\n", currentPointsPlayer - config->Fetch<int>("swiftly_ranks.Death"), config->Fetch<int>("swiftly_ranks.Death"));
            db->Query("UPDATE %s SET points = points - %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.Death"), player->GetSteamID());
            db->Query("UPDATE %s SET deaths = deaths + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
        }
    }
    else if(attacker) {
        attacker->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[+ %d for kill]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsAttacker + config->Fetch<int>("swiftly_ranks.NormalKill"), config->Fetch<int>("swiftly_ranks.NormalKill"));
        db->Query("UPDATE %s SET points = points + %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.NormalKill"), attacker->GetSteamID());
        db->Query("UPDATE %s SET kills = kills + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", attacker->GetSteamID());
        if(currentPointsPlayer > 0) {
            player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[- %d for dying]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsPlayer - config->Fetch<int>("swiftly_ranks.Death"), config->Fetch<int>("swiftly_ranks.Death"));
            db->Query("UPDATE %s SET points = points - %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.Death"), player->GetSteamID());
            db->Query("UPDATE %s SET deaths = deaths + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
        }
    }
    else if(assister) {
        assister->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[+ %d for assist]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsAttacker + config->Fetch<int>("swiftly_ranks.AssistKill"), config->Fetch<int>("swiftly_ranks.AssistKill"));
        db->Query("UPDATE %s SET points = points + %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.NormalKill"), assister->GetSteamID());
        db->Query("UPDATE %s SET assists = assists + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", assister->GetSteamID());
        if(currentPointsPlayer > 0) {
            player->SendMsg(HUD_PRINTTALK, "{RED} %s {DEFAULT}Your exp: %d {RED}[- %d for dying]\n", FetchTranslation("swiftly_ranks.prefix"), currentPointsPlayer - config->Fetch<int>("swiftly_ranks.Death"), config->Fetch<int>("swiftly_ranks.Death"));
            db->Query("UPDATE %s SET points = points - %d WHERE steamid = '%llu' LIMIT 1", "ranks", config->Fetch<int>("swiftly_ranks.Death"), player->GetSteamID());
            db->Query("UPDATE %s SET deaths = deaths + 1 WHERE steamid = '%llu' LIMIT 1", "ranks", player->GetSteamID());
        }
    }
}

void OnPluginStop()
{
}

const char *GetPluginAuthor()
{
    return "Swiftly Solutions";
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
    return "https://github.com/swiftly-solution/swiftly_skins";
}