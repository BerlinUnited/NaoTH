/*
 */

package naoscp.components;

import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.InputVerifier;
import javax.swing.JComponent;
import javax.swing.JFormattedTextField;
import javax.swing.text.MaskFormatter;
import naoscp.tools.Config;

/**
 *
 * @author Heinrich Mellmann
 */
public class PlayerNumberPanel extends javax.swing.JPanel {

    private final Map<String,Integer> bodyIdToPlayerNumber = new HashMap<String, Integer>();
    
    public Map<String,Integer> getBodyIdToPlayerNumber() {
        return bodyIdToPlayerNumber;
    }
    
    /**
     * Creates new form PlayerNumberPanel
     */
    public PlayerNumberPanel() {
        initComponents();
    }

    public boolean setRobots(File configDir) {
        if (!configDir.isDirectory()) {
            return false;
        }

        File robotsDir = new File(configDir + "/robots");
        if (!robotsDir.isDirectory()) {
            return false;
        }
        
        // reamove the existing components
        this.removeAll();

        File files[] = robotsDir.listFiles();
        Arrays.sort(files);

        try {
            for (int i = 0, n = files.length; i < n; i++) {
                if (files[i].isDirectory()) {
                    final String name = files[i].getName();
                    int number = loadPlayerCfg(files[i]);
                    
                    bodyIdToPlayerNumber.put(name, number);

                    final MaskFormatter formatter = new MaskFormatter(name + ": *");
                    formatter.setValidCharacters("1234567890");
                    formatter.setPlaceholderCharacter(Character.forDigit(number, 10));

                    final JFormattedTextField input = new JFormattedTextField(formatter);
                    input.setInputVerifier(new InputVerifier() {
                        @Override
                        public boolean verify(JComponent input) {
                            JFormattedTextField ftf = (JFormattedTextField) input;
                            try {
                                String s = (String) formatter.stringToValue(ftf.getText());
                                // capture the player number from the input and parse it
                                final Matcher matcher = Pattern.compile(name + ":[ \\\\t]*(\\d)").matcher(ftf.getText());
                                if (matcher.find() && matcher.groupCount() == 1) {
                                    int number = Integer.parseInt(matcher.group(1));
                                    bodyIdToPlayerNumber.put(name, number);
                                }
                            } catch (Exception ex) {
                            }
                            return true;
                        }
                    });

                    this.add(input);
                }
            }

            return true;
        } catch (ParseException ex) {
            return false;
        }
    }
    
    private int loadPlayerCfg(File configFile)
    {
        Config playerCfg = new Config("player");
        try {
            playerCfg.readFromFile(new File(configFile, "player.cfg"));
        } catch (IOException ex) {
            Logger.getGlobal().log(Level.WARNING, "Could not load player.cfg\n" + ex.getMessage());
            ex.printStackTrace(System.err);
        }
        
        String playerNumber = playerCfg.values.get("PlayerNumber");
        if(playerNumber != null) {
            return Integer.parseInt(playerNumber);
        }
        
        return 3;
    }
    
    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setLayout(new java.awt.GridLayout(2, 0));
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
}
