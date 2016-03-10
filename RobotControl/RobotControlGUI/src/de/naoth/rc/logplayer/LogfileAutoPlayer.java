/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.logplayer;


/**
 *
 * @author helios
 */
public class LogfileAutoPlayer extends Thread {
        LogSimulator.LogSimulatorManager logSimulatorManager;
        CallbackClass callback;
        long stopTime;
        boolean loop;
        int currentFrame; 
        private boolean isRunning;
      
        public LogfileAutoPlayer () {
            this.logSimulatorManager = LogSimulator.LogSimulatorManager.getInstance();
            this.callback = null;
            this.stopTime = 33;
            this.currentFrame = this.logSimulatorManager.getCurrentFrame();          
            this.loop = false;
        }
        
        public LogfileAutoPlayer (boolean loop) {
            this.logSimulatorManager = LogSimulator.LogSimulatorManager.getInstance();
            this.currentFrame = this.logSimulatorManager.getCurrentFrame();
            this.callback = null;
            this.stopTime = 33;
            this.loop = loop;
        }
        
        public LogfileAutoPlayer (int stopTime) {
            this.logSimulatorManager = LogSimulator.LogSimulatorManager.getInstance();
            this.stopTime = stopTime;
            this.currentFrame = this.logSimulatorManager.getCurrentFrame();
            this.callback = null;
            this.loop = false;
        }
      
        public LogfileAutoPlayer (CallbackClass callback) {
            this.logSimulatorManager = LogSimulator.LogSimulatorManager.getInstance();
            this.callback = callback;
            this.stopTime = 0;
            this.currentFrame = this.logSimulatorManager.getCurrentFrame();          
            this.loop = false;

      }
      
      @Override
      public void run () {
            this.isRunning = true;
            this.currentFrame = this.logSimulatorManager.getCurrentFrame();
            while (isRunning) 
            {
                if (currentFrame < logSimulatorManager.getMaxFrame()) {
                    logSimulatorManager.stepForward();
                    currentFrame++;                                        
                 }
                 else {
                     if (loop) {
                         logSimulatorManager.jumpToBegin();
                         currentFrame = this.logSimulatorManager.getMinFrame();
                     } else {
                         if (this.callback!=null){
                             callback.playFinished();                             
                         }
                         break;                        
                     }
                 }
                 try {                    
                        LogfileAutoPlayer.sleep(stopTime);
                 } 
                 catch (InterruptedException ex ) {
                     break;
                 }
            }
      }      
      
      public void stopPlay() {
          this.isRunning = false;
          if (this.callback!=null) {
              callback.playStopped();
          }
      }
      
      public interface CallbackClass {
          public void playFinished();
          public void playStopped();
      }
  } // end class LogFileAutoPlay; 