package de.naoth.rc.components.simspark;

import de.naoth.rc.components.simspark.commands.AgentCommand;
import de.naoth.rc.components.simspark.commands.BallCommand;
import de.naoth.rc.components.simspark.commands.DropBallCommand;
import de.naoth.rc.components.simspark.commands.SimsparkCommand;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class SimsparkCommandParser
{
    /*
       Possible commands:
        "(ball (pos 0 0 0))",
        "(ball (pos 0 0 0)(vel 0 0 0))",
        "(dropBall)",
        "(playMode )",
        "(agent (unum <num>) (team <team>) (pos <x> <y> <z>))",
        "(agent (unum <num>) (team <team>) (move <x> <y> <z> <rot>))",
        "(agent (unum <num>) (team <team>) (battery <batterylevel>))",
        "(agent (unum <num>) (team <team>) (temperature <temperature>))",
        "(agent (unum <num>) (team <team>) (pos <x> <y> <z>)(move <x> <y> <z> <rot>)(battery <batterylevel>)(temperature <temperature>))",
        "(kickOff <team>)",
        "(select (unum <num>) (team <team>))",
        "(kill)",
        "(kill (unum <num>) (team <team>))",
        "(repos)",
        "(repos (unum <num>) (team <team>))",
        "(time <time>)",
        "(score (left <score>) (right <score>))",
        "(reqfullstate)"
     */

    public static SimsparkCommand parseCommand(String command)
    {
        String[] commandParts = command.split("\\s+");

        System.out.println("Command (" + commandParts.length + "): " + command);

        if (commandParts.length == 0)
        {
            Logger.getLogger(SimsparkCommandParser.class.getName()).log(Level.INFO, "Empty command");
            return null;
        }

        SimsparkCommand cmd = null;
        switch (commandParts[0])
        {
            case "b":
            case "ball":
                cmd = handleCommandBall(command);
                break;
            case "p":
            case "pos":
                cmd = handleCommandAgent(command, null);
                break;
            case "l":
            case "left":
                cmd = handleCommandAgent(command, "Left");
                break;
            case "r":
            case "right":
                cmd = handleCommandAgent(command, "Right");
                break;
        }

        if (cmd == null)
        {
            Logger.getLogger(SimsparkCommandParser.class.getName()).log(Level.INFO, "Invalid or unknown command: {0}", command);
        }

        return cmd;
    }

    private static SimsparkCommand handleCommandBall(String command)
    {
        String[] commandParts = command.split("\\s+");
        Pattern ballPattern = Pattern.compile("(b|ball)(\\s+drop|(\\s+[+-]?(\\d*\\.\\d+|\\d+(\\.\\d+)?)){3,6})");
        Matcher ballMatcher = ballPattern.matcher(command);
        if (ballMatcher.matches())
        {
            switch (commandParts.length)
            {
                case 2:
                    return new DropBallCommand();
                case 4:
                    return new BallCommand(
                            Float.parseFloat(commandParts[1]),
                            Float.parseFloat(commandParts[2]),
                            Float.parseFloat(commandParts[3]));
                case 7:
                    return new BallCommand(
                            Float.parseFloat(commandParts[1]),
                            Float.parseFloat(commandParts[2]),
                            Float.parseFloat(commandParts[3]),
                            Float.parseFloat(commandParts[4]),
                            Float.parseFloat(commandParts[5]),
                            Float.parseFloat(commandParts[6]));
            }
        }

        return null;
    }

    private static SimsparkCommand handleCommandAgent(String command, String team)
    {
        String[] commandParts = command.split("\\s+");
        String pattern = (team == null ? "(p|pos)\\s+(Left|Right)" : "(l|left|r|right)") + "\\s+(\\d+)(\\s+[+-]?(\\d*\\.\\d+|\\d+(\\.\\d+)?)){3}";
        System.out.println(pattern);
        Pattern posPattern = Pattern.compile(pattern);
        Matcher posMatcher = posPattern.matcher(command);
        if (posMatcher.matches())
        {
            int i = 0;
            return new AgentCommand(
                    team != null ? team : commandParts[++i],
                    Integer.parseInt(commandParts[++i]),
                    Float.parseFloat(commandParts[++i]),
                    Float.parseFloat(commandParts[++i]),
                    Float.parseFloat(commandParts[++i]));
        }

        return null;
    }
}
