#include "ExampleAIModule.h"
#include <iostream>
#include <vector> // use to build a building/training order
#include "BuildQueue.h"

using namespace BWAPI;
using namespace Filter;

static BuildQueue buildQueue;

void logMsg(const std::string& s, unsigned int i = 0)
{
    Broodwar->drawTextScreen(20, 50, "%s(%d)", s.c_str(), i);
}

void ExampleAIModule::onStart()
{
    Broodwar->sendText("ver. 180807");
    if (Broodwar->self()->getRace() != Races::Zerg)
    {
        Broodwar->sendText("I can only play Zerg.");
        Broodwar->restartGame(); // use leaveGame() is another option.
    }
    Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
    Broodwar->enableFlag(Flag::UserInput);
    // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
    //Broodwar->enableFlag(Flag::CompleteMapInformation);
    Broodwar->setCommandOptimizationLevel(2);
    if ( Broodwar->isReplay() )
    {
        Broodwar << "The following players are in this replay:" << std::endl;
        Playerset players = Broodwar->getPlayers();
        for(auto p : players)
        {
          if ( !p->isObserver() )
            Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
        }
    }
    else
    {
        Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
        buildQueue.push(UnitTypes::Zerg_Drone);
    }
}

void ExampleAIModule::onEnd(bool isWinner)
{
    if ( isWinner )
    {
        Broodwar->sendText("I win!");
    }
}

void ExampleAIModule::onFrame()
{
    Broodwar->drawTextScreen(20, 0,  "FPS: %d", Broodwar->getFPS() );
    Broodwar->drawTextScreen(120, 0, "Average FPS: %.2f", Broodwar->getAverageFPS());
    Broodwar->drawTextScreen(270, 0, "APM: %d", Broodwar->getAPM() );
    // andromeda: start locations: bottom right - (117, 119) top right - (117, 7) bottom -left (7, 118) top left (7,6)
    Broodwar->drawTextScreen(20, 10, "Start Loc: (%d,%d)",
        Broodwar->self()->getStartLocation() );
    // initiate all count variables
    static unsigned int totalUnitCount, droneCount, baseCount, overlordCount;
    static unsigned int overlordInterval;
    static Error lastErr;
    totalUnitCount = 0; droneCount = 0; baseCount = 0; overlordCount = 0;
    if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
        return;
    // Latency frames are the number of frames before commands are processed.
    if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
        return;
    // Iterate through all the units that we own
    for (auto &u : Broodwar->self()->getUnits())
    {
        // Ignore the unit if it no longer exists
        if ( !u->exists() ) // Make sure to include this block when handling any Unit pointer!
            continue;
        if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
            continue;
        if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
            continue;
        if ( !u->isCompleted() || u->isConstructing() ) 
            continue;
        if (!u->getType().isBuilding()) ++totalUnitCount;
        if (u->getType().isResourceDepot()) ++baseCount;
        if (u->getType() == UnitTypes::Zerg_Overlord) ++overlordCount;
        if ( u->getType().isWorker() )
        {
            ++droneCount;
            if ( u->isIdle() ) // if our worker is idle
            {
                // Order workers carrying a resource to return them to the center,
                // otherwise find a mineral patch to harvest.
                if ( u->isCarryingGas() || u->isCarryingMinerals() )
                    u->returnCargo();
                else if ( !u->getPowerUp() )  // The worker cannot harvest anything if it
                {                             // is carrying a powerup such as a flag
                    // Harvest from the nearest mineral patch or gas refinery
                    if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) )
                    {
                        // If the call fails, then print the last error message
                        Broodwar << Broodwar->getLastError() << std::endl;
                    }
                } // closure: has no powerup
            } // closure: if idle
        }
    }   // closure: unit iterator

  // start to deal with the queue
    if (buildQueue.getQueueSize() > 0)
    {
    }
} // end of onFrame

void ExampleAIModule::onSendText(std::string text)
{
    // Send the text to the game if it is not being processed.
    Broodwar->sendText("%s", text.c_str());
    // Make sure to use %s and pass the text as a parameter,
    // otherwise you may run into problems when you use the %(percent) character!
}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
    // Parse the received text
    Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
    // Interact verbally with the other players in the game by
    // announcing that the other player has left.
    Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{
    // Check if the target is a valid position
    if ( target )
    {
        // if so, print the location of the nuclear strike target
        Broodwar << "Nuclear Launch Detected at " << target << std::endl;
    }
    else 
    {
        // Otherwise, ask other players where the nuke is!
        Broodwar->sendText("Where's the nuke?");
    }
    // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit) // if is enemy, attack?
{
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
{
    if ( Broodwar->isReplay() )
    {
        // if we are in a replay, then we will print out the build order of the structures
        if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
        {
            int seconds = Broodwar->getFrameCount()/24;
            int minutes = seconds/60;
            seconds %= 60;
            Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
        }
    }
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
{
    if ( Broodwar->isReplay() )
    {
        // if we are in a replay, then we will print out the build order of the structures
        if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
        {
            int seconds = Broodwar->getFrameCount()/24;
            int minutes = seconds/60;
            seconds %= 60;
            Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
        }
    }
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
    Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
}
