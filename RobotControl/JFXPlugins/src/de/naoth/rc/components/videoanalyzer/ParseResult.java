/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.videoanalyzer;

import de.naoth.rc.dataformats.LogFile;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.TreeMap;

/**
 *
 * @author thomas
 */
public class ParseResult implements Serializable
{
  LogFile logfile;
  /**
   * Maps a second (fractioned) to a log frame number
   */
  final TreeMap<Double, Integer> time2LogFrame = new TreeMap<>();
  /**
   * Maps a log frame number to a second (fractioned)
   */
  final TreeMap<Integer, Double> logFrame2Time = new TreeMap<>();
  final ArrayList<VideoAnalyzer.GameStateChange> changes = new ArrayList<>();
  double firstTime;

  public TreeMap<Double, Integer> getTime2LogFrame()
  {
    return time2LogFrame;
  }

  public ArrayList<VideoAnalyzer.GameStateChange> getChanges()
  {
    return changes;
  }
  
}
