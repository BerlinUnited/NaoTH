/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.dialogs;

import java.awt.Color;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author helios
 */
class ResultSaver {
    int     topMissedBalls,topFalseBalls, topMissedGP, topFalseGP, 
                bottomMissedBalls,bottomFalseBalls, bottomMissedGP,bottomFalseGP;
    enum ResultType {
        TOP_MISSED_BALLS, TOP_FALSE_BALLS, TOP_MISSED_GP, TOP_FALSE_GP,
        BOTTOM_MISSED_BALLS, BOTTOM_FALSE_BALLS, BOTTOM_MISSED_GP, BOTTOM_FALSE_GP; 
    }
    
    List<ResultType> printResults;
    double         parameterValue;

    ResultSaver (double parameterValue) {        
        clearValues(parameterValue);
        this.printResults = new LinkedList<>();
        this.printResults.addAll(Arrays.asList(ResultType.values()));
    }

    ResultSaver (ResultSaver resultSaver) {
       this.topMissedBalls = resultSaver.topMissedBalls;
       this.topFalseBalls = resultSaver.topFalseBalls;
       this.topMissedGP = resultSaver.topMissedGP;
       this.topFalseGP = resultSaver.topFalseGP;
       this.bottomMissedBalls = resultSaver.bottomMissedBalls;
       this.bottomFalseBalls = resultSaver.bottomFalseBalls;
       this.bottomMissedGP = resultSaver.bottomMissedGP;
       this.bottomFalseGP = resultSaver.bottomFalseGP; 
       this.parameterValue = resultSaver.parameterValue;
       this.printResults = new LinkedList<>(resultSaver.printResults);
    }

    final void clearValues(double parameterValue) {
      this.topMissedBalls = 0;
      this.topFalseBalls = 0;
      this.topMissedGP = 0;
      this.topFalseGP = 0;
      this.bottomMissedBalls = 0;
      this.bottomFalseBalls = 0;
      this.bottomMissedGP = 0;
      this.bottomFalseGP = 0;
      this.parameterValue = parameterValue;
    }

    public int getValue(ResultType resultType) {
        switch (resultType) {
            case TOP_MISSED_BALLS: 
                return this.topMissedBalls;
            case TOP_FALSE_BALLS:
                return this.topFalseBalls;
            case TOP_MISSED_GP:
                return this.topMissedGP;
            case TOP_FALSE_GP:
                return this.topFalseGP;
            case BOTTOM_MISSED_BALLS:
                return this.bottomMissedBalls;
            case BOTTOM_FALSE_BALLS:
                return this.bottomFalseBalls;
            case BOTTOM_MISSED_GP:
                return this.bottomMissedGP;
            case BOTTOM_FALSE_GP:
                return this.bottomFalseGP;
            default:
                return 0;
        }
    }
    
    static public Color getColor (ResultType resultType) {
        switch (resultType) {
            case TOP_MISSED_BALLS: 
                return Color.red;
            case TOP_FALSE_BALLS:
                return Color.blue;
            case TOP_MISSED_GP:
                return Color.yellow;
            case TOP_FALSE_GP:
                return Color.gray;
            case BOTTOM_MISSED_BALLS:
                return Color.green;
            case BOTTOM_FALSE_BALLS:
                return Color.cyan;
            case BOTTOM_MISSED_GP:
                return Color.magenta;
            case BOTTOM_FALSE_GP:
                return Color.orange;
            default:
                return Color.black;
        }
    }
}
