#include "ExampleAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

void ExampleAIModule::onStart()
{
    Broodwar->sendText("ver. 180807");
    if (Broodwar->self()->getRace() != Races::Zerg)
    {
        Broodwar->sendText("I can only play Zerg.");
        Broodwar->restartGame(); // use leaveGame() is another option.
    }
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
        Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
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
    if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
        return;
    if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
        return;
    for (auto &u : Broodwar->self()->getUnits())
    {   // Make sure to include this block when handling any Unit pointer!
        if ( !u->exists() ) continue;
        if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() ) continue;
        if ( u->isLoaded() || !u->isPowered() || u->isStuck() ) continue;
        if ( !u->isCompleted() || u->isConstructing() ) continue;
        if ( u->getType().isWorker() )
        {
            if ( u->isIdle() )
            {
                if ( u->isCarryingGas() || u->isCarryingMinerals() )
                    u->returnCargo();
                else if ( !u->getPowerUp() )
                {
                    if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) )
                    {
                        Broodwar << Broodwar->getLastError() << std::endl;
                    }
                } 
            } 
        }
    } 

} // end of onFrame

