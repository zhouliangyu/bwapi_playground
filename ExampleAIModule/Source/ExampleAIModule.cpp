#include "ExampleAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

void ExampleAIModule::onStart()
{
    Broodwar->sendText("Hello world!");
    Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
    Broodwar->enableFlag(Flag::UserInput);
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
    else // if this is not a replay
    {
        if ( Broodwar->enemy() ) // First make sure there is an enemy
            Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
    }

}

void ExampleAIModule::onEnd(bool isWinner)
{
    if ( isWinner )
    {
    }
}

void ExampleAIModule::onFrame()
{
    Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
    Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );
    if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
        return;
    if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
        return;

    for (auto &u : Broodwar->self()->getUnits())
    {
        if ( !u->exists() )
            continue;
        if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
            continue;
        if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
            continue;
        if ( !u->isCompleted() || u->isConstructing() )
            continue;

        if ( u->getType().isWorker() )
        {
            if ( u->isIdle() )
            {
                if ( u->isCarryingGas() || u->isCarryingMinerals() )
                {
                    u->returnCargo();
                }
                else if ( !u->getPowerUp() )  // The worker cannot harvest anything if it
                {                             // is carrying a powerup such as a flag
                    if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) )
                    {
                        Broodwar << Broodwar->getLastError() << std::endl;
                    }
                } // closure: has no powerup
            } // closure: if idle

        }
        else if ( u->getType().isResourceDepot() ) // A resource depot is a Command Center, Nexus, or Hatchery
        {
            if ( u->isIdle() && !u->train(u->getType().getRace().getWorker()) )
            {
                Position pos = u->getPosition();
                Error lastErr = Broodwar->getLastError();
                Broodwar->registerEvent([pos,lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
                        nullptr,    // condition
                        Broodwar->getLatencyFrames());  // frames to run
                UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
                static int lastChecked = 0;
                if (  lastErr == Errors::Insufficient_Supply &&
                        lastChecked + 400 < Broodwar->getFrameCount() &&
                        Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0 )
                {
                    lastChecked = Broodwar->getFrameCount();
                    Unit supplyBuilder = u->getClosestUnit(  GetType == supplyProviderType.whatBuilds().first &&
                            (IsIdle || IsGatheringMinerals) &&
                            IsOwned);
                    if ( supplyBuilder )
                    {
                        if ( supplyProviderType.isBuilding() )
                        {
                            TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
                            if ( targetBuildLocation )
                            {
                                Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*)
                                        {
                                        Broodwar->drawBoxMap( Position(targetBuildLocation),
                                                Position(targetBuildLocation + supplyProviderType.tileSize()),
                                                Colors::Blue);
                                        },
                                        nullptr,  // condition
                                        supplyProviderType.buildTime() + 100 );  // frames to run
                                supplyBuilder->build( supplyProviderType, targetBuildLocation );
                            }
                        }
                        else
                        {
                            supplyBuilder->train( supplyProviderType );
                        }
                    } // closure: supplyBuilder is valid
                } // closure: insufficient supply
            } // closure: failed to train idle unit

        }

    } // closure: unit iterator
}

void ExampleAIModule::onSendText(std::string text)
{
    Broodwar->sendText("%s", text.c_str());
}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
    Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
    Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{
    if ( target )
    {
        Broodwar << "Nuclear Launch Detected at " << target << std::endl;
    }
    else 
    {
        Broodwar->sendText("Where's the nuke?");
    }
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
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
