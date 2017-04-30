package de.naoth.rc.components.teamcommviewer;

import java.awt.Color;
import java.util.Arrays;
import java.util.List;
import javax.swing.JMenu;
import javax.swing.JMenuItem;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class TeamColorMenu extends JMenu
{
    public static List<Color> colors = Arrays.asList(
        Color.RED,
        Color.BLUE,
        Color.CYAN,
        Color.ORANGE,
        Color.GREEN,
        Color.YELLOW,
        Color.PINK,
        Color.MAGENTA,
        Color.LIGHT_GRAY,
        Color.DARK_GRAY,
        Color.GRAY,
        Color.BLACK,
        Color.WHITE,
        // some self defined colors
        new Color(180, 210, 255),
        new Color(210, 180, 200)
    );

    public int teamNumber;

    public TeamColorMenu(int team, Color color) {
        super();
        
        this.teamNumber = team;
        this.setText("Team " + team);
        this.setBackground(color);
        this.setOpaque(true);
        
        for(Color c : colors) {
            JMenuItem item = new JMenuItem(" ");
            item.setBackground(c);
            item.setOpaque(true);
            item.addActionListener((e) -> {
                this.setBackground(((JMenuItem)e.getSource()).getBackground());
                this.fireActionPerformed(e);
            });
            this.add(item);
        }
    }
    
    public static Color getNextColor(Color c) {
        int idx = colors.indexOf(c);
        if(idx == -1 || ++idx >= colors.size()) {
            idx = 0;
        }
        return colors.get(idx);
    }
}
