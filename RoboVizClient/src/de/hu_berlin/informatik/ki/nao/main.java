/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.hu_berlin.informatik.ki.nao;

import java.util.Timer;

public class main {

  public static void main(String[] args) throws Exception
  {
    RoboVizClient rvc = new RoboVizClient();
    TeamCommUpdater teamComm = new TeamCommUpdater(rvc);
    Timer timer = new Timer();
    timer.scheduleAtFixedRate(teamComm, 0, 1);
  }
}
