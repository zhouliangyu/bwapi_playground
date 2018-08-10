#include "ExampleAIModule.h"
#include <iostream>
#include "TaskQueue.h"

using namespace BWAPI;
using namespace Filter;

TaskQueue taskQueue;

void ExampleAIModule::onStart()
{
    if (Broodwar->self()->getRace() != Races::Zerg) Broodwar->restartGame();
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
    else
    {
        if ( Broodwar->enemy() )
            Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
        taskQueue.push(TaskCategories::TRAIN_UNIT, UnitTypes::Zerg_Drone); // onStart, push a worker
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
    Broodwar->drawTextScreen(200, 10, "Average FPS: %f", Broodwar->getAverageFPS() );
    if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
        return;
    if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
        return;
    
    // on every frame, pop out a task
    if (taskQueue.pop() > 0)
    {
        TaskItem currTask = taskQueue.pop();
    }

    for (auto &u : Broodwar->self()->getUnits())
    {
        // Make sure to include this block when handling any Unit pointer!
        if ( !u->exists() )
            continue;
        if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
            continue;
        if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
            continue;
        if ( !u->isCompleted() || u->isConstructing() )
            continue;

        // If the unit is a worker unit
        if ( u->getType().isWorker() )
        {
            if ( u->isIdle() )
            {
                if ( u->isCarryingGas() || u->isCarryingMinerals() )
                {
                    u->returnCargo();
                }
                else if ( !u->getPowerUp() )
                {
                    if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) )
                    {
                        Broodwar << Broodwar->getLastError() << std::endl;
                    }
                }
            }
        }

    /* else if ( u->getType().isResourceDepot() ) */
    /* { */
    /*   if ( u->isIdle() && !u->train(u->getType().getRace().getWorker()) ) */
    /*   { */
    /*     Position pos = u->getPosition(); */
    /*     Error lastErr = Broodwar->getLastError(); */
    /*     Broodwar->registerEvent([pos,lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action */
    /*                             nullptr,    // condition */
    /*                             Broodwar->getLatencyFrames());  // frames to run */
    /*     UnitType supplyProviderType = u->getType().getRace().getSupplyProvider(); */
    /*     static int lastChecked = 0; */
    /*     if (  lastErr == Errors::Insufficient_Supply && */
    /*           lastChecked + 400 < Broodwar->getFrameCount() && */
    /*           Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0 ) */
    /*     { */
    /*       lastChecked = Broodwar->getFrameCount(); */
    /*       Unit supplyBuilder = u->getClosestUnit(  GetType == supplyProviderType.whatBuilds().first && */
    /*                                                 (IsIdle || IsGatheringMinerals) && */
    /*                                                 IsOwned); */
    /*       if ( supplyBuilder ) */
    /*       { */
    /*         if ( supplyProviderType.isBuilding() ) */
    /*         { */
    /*           TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition()); */
    /*           if ( targetBuildLocation ) */
    /*           { */
    /*             Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*) */
    /*                                     { */
    /*                                       Broodwar->drawBoxMap( Position(targetBuildLocation), */
    /*                                                             Position(targetBuildLocation + supplyProviderType.tileSize()), */
    /*                                                             Colors::Blue); */
    /*                                     }, */
    /*                                     nullptr,  // condition */
    /*                                     supplyProviderType.buildTime() + 100 );  // frames to run */
    /*             supplyBuilder->build( supplyProviderType, targetBuildLocation ); */
    /*           } */
    /*         } */
    /*         else */
    /*         { */
    /*           // Train the supply provider (Overlord) if the provider is not a structure */
    /*           supplyBuilder->train( supplyProviderType ); */
    /*         } */
    /*       } */
    /*     } */
    /*   } */
    /* } */
  } // end of unit loop



} // end of "onFrame"

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
