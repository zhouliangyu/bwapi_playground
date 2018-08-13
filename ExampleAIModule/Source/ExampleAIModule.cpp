#include "ExampleAIModule.h"
#include <iostream>
#include "TaskQueue.h"
#include "PositionQueue.h"
#include "DevelopTools.h"

using namespace BWAPI;
using namespace Filter;

// initiation of system wide variables
TaskQueue taskQueue;
PositionQueue positionQueue;
int overlordLastChecked = 0; const int OVERLORD_CHECK_INTERVAL = 650;
int zerglingLastChecked = 0; const int ZERGLING_CHECK_INTERVAL = 470;
const int DRONE_BOUNDARY_FACTOR = 3; const int DRONE_EVERY_BASE = 20;
const int FIRST_SPAWNING = 6;
int suspensionLastCheck = 0; const int SUSPENSION_INTERVAL = 200;
bool isBuildingDeployed = true;
bool isScoutSent = false; int lastSendScout = 0; const int SCOUT_INTERVAL = 600;
bool lackingLarva = false; int hatcheryLastPushed = 0; const int HATCHERY_CHECK_INTERVAL = 600;

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
            Broodwar << "The matchup is " << Broodwar->self()->getRace() <<
                " vs " << Broodwar->enemy()->getRace() << std::endl;
        // when game starts, push a drone
        taskQueue.push(TaskItem(TaskCategories::TRAIN_UNIT, UnitTypes::Zerg_Drone));
        DevelopTools::logMessegeOnScreen("On start of game pushed a drone");
        positionQueue.addStartingLocations();
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
    Broodwar->drawTextScreen(250, 0,  "APM: %d", Broodwar->getAPM() );
    Broodwar->drawTextScreen(200, 10, "Average FPS: %f", Broodwar->getAverageFPS() );

    if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
        return;
    if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
        return;
    taskQueue.updateOnScreen();
    // variable initiation for each frame
    TaskItem currTask;
    Error lastErr = Errors::None;
    lackingLarva = true;
    // on every frame, pop out a task
    if (taskQueue.getQueueSize() > 0) currTask = taskQueue.pop();
    if (currTask.getTaskCategory() == TaskCategories::BUILD_UNIT)
        isBuildingDeployed = false;
    // assign scouter type
    if (currTask.getTaskCategory() == TaskCategories::SCOUT_MAP)
    {
        isScoutSent = false;
        if (Broodwar->self()->allUnitCount(UnitTypes::Zerg_Spawning_Pool) == 0)
        {
            currTask.setRelatedUnit(UnitTypes::Zerg_Overlord);
        }
        else if (Broodwar->self()->allUnitCount(UnitTypes::Zerg_Zergling) == 0)
        {
            currTask.setRelatedUnit(UnitTypes::Zerg_Drone);
        }
        else
        {
            currTask.setRelatedUnit(UnitTypes::Zerg_Zergling);
        }
    }
    // loop through all units
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
        // assign scouter
        if (currTask.getTaskCategory() == TaskCategories::SCOUT_MAP)
        {
            if (u->getType() == currTask.getRelatedUnit())
            {
                if (!isScoutSent && (u->isIdle() || u->getType() == UnitTypes::Zerg_Drone))
                {
                    for (int i=positionQueue.getPositionQueueSize()-1; i>=0; --i)
                    {
                        if (!u->move(positionQueue.getPosition(i),
                            (i!=positionQueue.getPositionQueueSize()-1)))
                        {
                            lastErr = Broodwar->getLastError();
                            break;
                        }
                        isScoutSent = true;
                        lastSendScout = Broodwar->getFrameCount();
                    }
                }
            }
        }
        // If the unit is a worker unit
        if ( u->getType().isWorker() )
        {
            if (currTask.getTaskCategory() == TaskCategories::BUILD_UNIT)
            {
                if (currTask.getMineralRequired() >= Broodwar->self()->minerals())
                {
                    lastErr = Errors::Insufficient_Minerals;
                }
                else
                {
                    if (!isBuildingDeployed)
                    {
                        if (!u->build(currTask.getRelatedUnit(),
                            Broodwar->getBuildLocation(currTask.getRelatedUnit(),
                                u->getTilePosition())))
                        {
                            lastErr = Broodwar->getLastError();
                        }
                        isBuildingDeployed = true;
                        suspensionLastCheck = Broodwar->getFrameCount();
                    }
                }
            }
            if ( u->isIdle() )
            {
                if ( u->isCarryingGas() || u->isCarryingMinerals() )
                {
                    u->returnCargo();
                }
                if ( !u->getPowerUp() )
                {
                    if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) )
                    {
                        Broodwar << Broodwar->getLastError() << std::endl;
                    }
                }
            }
        }

        // if the unit is a Hatchery, Lair or Hive
        if ( u->getType().producesLarva() )
        {
            if ( currTask.getTaskCategory() == TaskCategories::TRAIN_UNIT &&
                Broodwar->getFrameCount() - suspensionLastCheck > SUSPENSION_INTERVAL)
            {
                for (const auto& l : u->getLarva())
                {
                    if ( !l->morph(currTask.getRelatedUnit()) )
                    {
                        lastErr = Broodwar->getLastError();
                    }
                    lackingLarva = false;
                    break; // break the foor loop of larva
                }
            }
        }

    } // end of unit loop
    
    // Error handling
    if ( lastErr == Errors::Insufficient_Minerals )
    {
        if ( currTask.getTaskCategory() == TaskCategories::TRAIN_UNIT ||
            currTask.getTaskCategory() == TaskCategories::BUILD_UNIT)
            taskQueue.push(currTask);
        lastErr = Errors::None;
        return;
    }
    if ( lastErr == Errors::Insufficient_Supply )
    {
        if ( currTask.getTaskCategory() == TaskCategories::TRAIN_UNIT)
            taskQueue.push(currTask);
        if ( Broodwar->getFrameCount() - overlordLastChecked > OVERLORD_CHECK_INTERVAL &&
            Broodwar->self()->incompleteUnitCount(UnitTypes::Zerg_Overlord) == 0)
        {
            taskQueue.push(TaskItem(TaskCategories::TRAIN_UNIT, UnitTypes::Zerg_Overlord));
            DevelopTools::logMessegeOnScreen("Pushed overlord because of the Insufficient_Supply error");
            overlordLastChecked = Broodwar->getFrameCount();
        }
        lastErr = Errors::None;
        return;
    }
    if (lastErr == Errors::Insufficient_Gas)
    {
    }

    // add new tasks 
    if (Broodwar->getFrameCount() - lastSendScout > SCOUT_INTERVAL)
    {
        taskQueue.push(TaskItem(TaskCategories::SCOUT_MAP));
        lastSendScout = Broodwar->getFrameCount();
        DevelopTools::logMessegeOnScreen("Add a scouting task");
    }

    // add new build tasks
    if (!isBuildingDeployed || Broodwar->getFrameCount() - suspensionLastCheck < SUSPENSION_INTERVAL) return;
    if ((Broodwar->self()->allUnitCount() > Broodwar->self()->allUnitCount(UnitTypes::Zerg_Drone) *
        DRONE_BOUNDARY_FACTOR || (Broodwar->self()->allUnitCount(UnitTypes::Zerg_Hatchery) +
        Broodwar->self()->allUnitCount(UnitTypes::Zerg_Lair) +
        Broodwar->self()->allUnitCount(UnitTypes::Zerg_Hive)) * DRONE_EVERY_BASE >
        Broodwar->self()->allUnitCount(UnitTypes::Zerg_Drone)) &&
        Broodwar->self()->incompleteUnitCount(UnitTypes::Zerg_Drone) == 0)
    {
        taskQueue.push(TaskItem(TaskCategories::TRAIN_UNIT, UnitTypes::Zerg_Drone));
        DevelopTools::logMessegeOnScreen("Pushed drone because drones are few");
    }
    if (lackingLarva && currTask.getTaskCategory() == TaskCategories::TRAIN_UNIT &&
        Broodwar->self()->incompleteUnitCount(UnitTypes::Zerg_Hatchery) == 0 &&
        Broodwar->getFrameCount() - hatcheryLastPushed > HATCHERY_CHECK_INTERVAL)
    {
        taskQueue.push(TaskItem(TaskCategories::BUILD_UNIT, UnitTypes::Zerg_Hatchery));
        hatcheryLastPushed = Broodwar->getFrameCount();
        DevelopTools::logMessegeOnScreen("Pushed a Hatchery since lacking larva.");
    }
    if (Broodwar->self()->allUnitCount(UnitTypes::Zerg_Drone) >= FIRST_SPAWNING &&
        Broodwar->self()->allUnitCount(UnitTypes::Zerg_Spawning_Pool) == 0 &&
        taskQueue.searchTaskRelatedUnit(UnitTypes::Zerg_Spawning_Pool) == -1 &&
        Broodwar->self()->incompleteUnitCount(UnitTypes::Zerg_Spawning_Pool) == 0)
    {
        taskQueue.push(TaskItem(TaskCategories::BUILD_UNIT, UnitTypes::Zerg_Spawning_Pool));
        DevelopTools::logMessegeOnScreen("Pushed spawning because it reaches the drone number ", FIRST_SPAWNING);
    }
    if (Broodwar->self()->allUnitCount(UnitTypes::Zerg_Spawning_Pool) > 0 &&
        Broodwar->getFrameCount() - zerglingLastChecked > ZERGLING_CHECK_INTERVAL &&
        taskQueue.searchTaskRelatedUnit(UnitTypes::Zerg_Zergling) == -1)
    {
        taskQueue.push(TaskItem(TaskCategories::TRAIN_UNIT, UnitTypes::Zerg_Zergling));
        DevelopTools::logMessegeOnScreen("push a pair of zergling");
    }



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
