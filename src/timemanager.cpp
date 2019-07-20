/*
 * CrazyAra, a deep learning chess variant engine
 * Copyright (C) 2018 Johannes Czech, Moritz Willig, Alena Beyer
 * Copyright (C) 2019 Johannes Czech
 *
 * CrazyAra is free software: You can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * @file: timemanager.cpp
 * Created on 16.07.2019
 * @author: queensgambit
 */

#include "timemanager.h"

using namespace std;


TimeManager::TimeManager(int expectedGameLength, int threshMove, float moveFactor, float incrementFactor, int timeBufferFactor):
    expectedGameLength(expectedGameLength),
    threshMove(threshMove),
    moveFactor(moveFactor),
    incrementFactor(incrementFactor),
    timeBufferFactor(timeBufferFactor)
{
    assert(threshMove < expectedGameLength);
}

int TimeManager::get_time_for_move(SearchLimits* searchLimits, Color me, int moveNumber)
{
    // leave an additional time buffer to avoid losing on time
    timeBuffer = searchLimits->moveOverhead * timeBufferFactor;

    if (searchLimits->movetime != 0) {
        // only return the plain move time substracted by the move overhead
        curMovetime = searchLimits->movetime - searchLimits->moveOverhead;
    }
    else if (searchLimits->movestogo != 0) {
        // calculate a constant move time based on increment and moves left
        curMovetime = int(((searchLimits->time[me] - timeBuffer) / float(searchLimits->movestogo) + 0.5f)
                + searchLimits->inc[me] - searchLimits->moveOverhead);
    }
    else if (searchLimits->time[me] != 0) {
        // calculate a movetime in sudden death mode
        if (moveNumber < threshMove) {
            curMovetime = int((searchLimits->time[me] - timeBuffer) / float(expectedGameLength-moveNumber) + 0.5f)
                    + int(searchLimits->inc[me] * incrementFactor) - searchLimits->moveOverhead;
        }
        else {
            curMovetime = int((searchLimits->time[me] - timeBuffer) * moveFactor + 0.5f)
                    + int(searchLimits->inc[me] * incrementFactor) - searchLimits->moveOverhead;
        }
    }
    else if (searchLimits->nodes) {
        // TODO
        assert(false);
    }
    else {
        curMovetime = 1000 - searchLimits->moveOverhead;
//        sync_cout << "string info No limit specification given, setting movetime to (1000 - moveOverhead)" << sync_endl;
    }

    if (curMovetime <= 0) {
        curMovetime = searchLimits->moveOverhead * 2;
    }
    return curMovetime;
}
