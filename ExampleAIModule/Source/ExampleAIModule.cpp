#include "ExampleAIModule.h"
#include <iostream>
#include <vector> // use to build a building/training order

using namespace BWAPI;
using namespace Filter;
static std::vector<UnitType> buildQueue; // define buildQueue

void ExampleAIModule::onStart()
{
  Broodwar->sendText("The first attempt to modified the code 180805");     // type text
  if (Broodwar->self()->getRace() != Races::Zerg)
  {
    Broodwar->sendText("I can only play Zerg.");
    Broodwar->restartGame(); // use leaveGame() is another option.
  }
  Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;   // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
  Broodwar->enableFlag(Flag::UserInput);                                  // Enable user control
  // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
  //Broodwar->enableFlag(Flag::CompleteMapInformation);
  // Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
  Broodwar->setCommandOptimizationLevel(2);
  if ( Broodwar->isReplay() ) // Check if this is a replay
  {
    Broodwar << "The following players are in this replay:" << std::endl; // Announce the players in the replay
    Playerset players = Broodwar->getPlayers();  // Iterate all the players in the game using a std:: iterator
    for(auto p : players)
    {
      // Only print the player if they are not an observer
      if ( !p->isObserver() )
        Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
    }
  }
  else // if this is not a replay
  {
    // Retrieve you and your enemy's races. enemy() will just return the first enemy.
      Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
      buildQueue.push_back(UnitTypes::Zerg_Drone);
      /* buildQueue.push_back(UnitTypes::Zerg_Overlord); */
  }
}

void ExampleAIModule::onEnd(bool isWinner) // Called when the game ends
{
  if ( isWinner )
  {
    // Log your win here!
    Broodwar->sendText("I win!");
  }
}

void ExampleAIModule::onFrame() // Called once every game frame
{
  Broodwar->drawTextScreen(20, 0,  "FPS: %d", Broodwar->getFPS() ); // Display the game frame rate as text in the upper left area of the screen
  Broodwar->drawTextScreen(120, 0, "Average FPS: %.2f", Broodwar->getAverageFPS());
  Broodwar->drawTextScreen(270, 0, "APM: %d", Broodwar->getAPM() );
  // andromeda: start locations: bottom right - (117, 119) top right - (117, 7) bottom -left (7, 118) top left (7,6)
  Broodwar->drawTextScreen(20, 10, "Start Loc: (%d,%d)",
    Broodwar->self()->getStartLocation() );
  static unsigned int queueSize = buildQueue.size();
  static unsigned int totalUnitCount, droneCount, baseCount, overlordCount;
  Broodwar->drawTextScreen(120, 10, "Next item (queue size): %s(%d)",
    buildQueue[queueSize-1].c_str(), queueSize);
  // Return if the game is a replay or is paused
  if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
    return;
  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
    return;
  
  // Iterate through all the units that we own
  // initiate all count variables
  totalUnitCount = 0;droneCount = 0; baseCount = 0; overlordCount = 0;

  for (auto &u : Broodwar->self()->getUnits())
  {
    // Ignore the unit if it no longer exists
    if ( !u->exists() ) // Make sure to include this block when handling any Unit pointer!
      continue;
    if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() ) // Ignore the unit if it has one of the following status ailments
      continue;
    if ( u->isLoaded() || !u->isPowered() || u->isStuck() ) // Ignore the unit if it is in one of the following states
      continue;
    if ( !u->isCompleted() || u->isConstructing() ) // Ignore the unit if it is incomplete or busy constructing
      continue;
    // Finally make the unit do some stuff!
    // If the unit is a worker unit
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

    /*
    else if ( u->getType().isResourceDepot() ) // A resource depot is a Command Center, Nexus, or Hatchery
    {
      // Order the depot to construct more workers! But only when it is idle.
      if ( u->isIdle() && !u->train(u->getType().getRace().getWorker()) )
      {
        // If that fails, draw the error at the location so that you can visibly see what went wrong!
        // However, drawing the error once will only appear for a single frame
        // so create an event that keeps it on the screen for some frames
        Position pos = u->getPosition();
        Error lastErr = Broodwar->getLastError();
        Broodwar->registerEvent([pos,lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
                                nullptr,    // condition
                                Broodwar->getLatencyFrames());  // frames to run
        // Retrieve the supply provider type in the case that we have run out of supplies
        UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
        static int lastChecked = 0;
        // If we are supply blocked and haven't tried constructing more recently
        if (  lastErr == Errors::Insufficient_Supply &&
              lastChecked + 400 < Broodwar->getFrameCount() &&
              Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0 )
        {
          lastChecked = Broodwar->getFrameCount();
          // Retrieve a unit that is capable of constructing the supply needed
          Unit supplyBuilder = u->getClosestUnit(  GetType == supplyProviderType.whatBuilds().first &&
                                                    (IsIdle || IsGatheringMinerals) &&
                                                    IsOwned);
          // If a unit was found
          if ( supplyBuilder )
          {
            if ( supplyProviderType.isBuilding() )
            {
              TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
              if ( targetBuildLocation )
              {
                // Register an event that draws the target build location
                Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*)
                                        {
                                          Broodwar->drawBoxMap( Position(targetBuildLocation),
                                                                Position(targetBuildLocation + supplyProviderType.tileSize()),
                                                                Colors::Blue);
                                        },
                                        nullptr,  // condition
                                        supplyProviderType.buildTime() + 100 );  // frames to run
                // Order the builder to construct the supply structure
                supplyBuilder->build( supplyProviderType, targetBuildLocation );
              }
            }
            else
            {
              // Train the supply provider (Overlord) if the provider is not a structure
              supplyBuilder->train( supplyProviderType );
            }
          } // closure: supplyBuilder is valid
        } // closure: insufficient supply
      } // closure: failed to train idle unit
    }
    */
  } // closure: unit iterator

  // start to deal with the queue
  static auto currBuildItem = buildQueue.back();
  buildQueue.pop_back();
  static Error lastErr;
  static bool popedOutHandled = false;
  switch (currBuildItem)
  {
    // TODO why never hit the switch inside? TODO
    Broodwar->drawTextScreen(30, 30, "Inside switch!" );
    case UnitTypes::Zerg_Drone:
      for (const auto &u : Broodwar->self()->getUnits().getLarva())
      {
          u->morph(currBuildItem);
          Error lastErr = Broodwar->getLastError();
          break;
      }
      if (lastErr == Errors::Insufficient_Minerals)
      {
          buildQueue.push_back(UnitTypes::Zerg_Drone);
          break;
      }
      if (lastErr == Errors::Insufficient_Supply)
      {
          buildQueue.push_back(UnitTypes::Zerg_Drone);
          buildQueue.push_back(UnitTypes::Zerg_Overlord);
          break;
      }
      if ((totalUnitCount > 4*droneCount) || (12*baseCount > droneCount))
        buildQueue.push_back(UnitTypes::Zerg_Drone);
      if (totalUnitCount+4 >= 8*overlordCount)
        buildQueue.push_back(UnitTypes::Zerg_Overlord);
      popedOutHandled = true;
      break;
    case UnitTypes::Zerg_Overlord:
      for (const auto &u : Broodwar->self()->getUnits().getLarva())
      {
          u->morph(currBuildItem);
          Error lastErr = Broodwar->getLastError();
          break;
      }
      if (lastErr == Errors::Insufficient_Minerals)
      {
          buildQueue.push_back(UnitTypes::Zerg_Overlord);
          break;
      }
      popedOutHandled = true;
      break;
    default:
      popedOutHandled = false;
      break;
  }
  if (!popedOutHandled) buildQueue.push_back(currBuildItem);
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
