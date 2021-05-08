
package de.naoth.rc;

import java.awt.Color;
import java.awt.DefaultKeyboardFocusManager;
import java.awt.KeyboardFocusManager;
import javax.swing.AbstractAction;
import java.awt.event.ActionEvent;
import java.awt.event.InputEvent;
import java.awt.KeyEventPostProcessor;
import javax.swing.KeyStroke;
import java.awt.Window;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.SwingUtilities;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.PlainDocument;

/**
 *
 * @author Heinrich Mellmann
 */
public class DialogFastAccessPanel extends javax.swing.JPanel {

    private final ArrayList<DialogEntry> dialogEntries = new ArrayList<>();
    
    class DialogEntry {
        final JButton button;
        private final String name;
        private final String category;
        private final String className;
        
        DialogEntry(String name, String category, String className, JButton button) {
            this.name = name.toLowerCase();
            this.category = category.toLowerCase();
            this.className = className.toLowerCase();
            this.button = button;
        }
        
        public double calculateScore(String str) {
            str = str.toLowerCase();
            
            if(name.startsWith(str)) {
                return 1e+10;
            } 
            
            if(category.startsWith(str)) {
                return 1e+9;
            }
            
            if(className.startsWith(str)) {
                return 1e+5;
            } 
            
            if(name.contains(str)) {
                return 1e+4;
            }
            
            if(category.contains(str)) {
                return 1e+2;
            }
            
            if(className.contains(str)) {
                return 1e+1;
            }
            
            return 0.0;
        }
    }
    
    class Pair implements Comparable<Pair> {
        double v;
        JButton b;
        Pair(double v, JButton b) {
            this.v = v;
            this.b = b;
        }

        @Override
        public int compareTo(Pair o) {
            double diff = o.v - this.v;
            if(diff == 0) {
                return 0;
            } else if(diff < 0) {
                return -1;
            } else {
                return 1;
            }
        }
    }
    
    /**
     * Creates new form DialogFastAccessPanel
     */
    public DialogFastAccessPanel() {
        initComponents();
        
        //for (KeyListener k: this.search.getKeyListeners())
        //    SwingUtilities.getWindowAncestor(this).addKeyListener(k);
        this.add_tab_actions();

        this.search.setDocument(new PlainDocument(){
            @Override
            public void remove(int offs, int len) throws BadLocationException {
                super.remove(offs, len);
                
                String text = getText(0, getLength());
                updateButtons(text);
            }
    
            @Override
            public void insertString(int offs, String str, AttributeSet a) throws BadLocationException {
                super.insertString(offs, str, a);
                
                String text = getText(0, getLength());
                updateButtons(text);
                
            }
        });
        
        this.search.addFocusListener(new FocusListener() {
            @Override
            public void focusGained(FocusEvent e) {
                DefaultKeyboardFocusManager.getCurrentKeyboardFocusManager().removeKeyEventPostProcessor(charCapture);
            }

            @Override
            public void focusLost(FocusEvent e) {
                DefaultKeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventPostProcessor(charCapture);
            }
        });
        
        /*
        this.search.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                
                if(e.getKeyCode() == KeyEvent.VK_ENTER)
                {
                    //System.out.println("press");
                    if(selectedButton != null) {
                        selectedButton.doClick();
                    }
                }
            }
        });
        */
    }

    JButton selectedButton = null;
    Color oldBackgrund = null;

    private void add_tab_actions()
    {
        Set<KeyStroke> forwardKeys = new HashSet<KeyStroke>(1);
        forwardKeys.add(KeyStroke.getKeyStroke(KeyEvent.VK_TAB, InputEvent.CTRL_MASK));
        setFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS, forwardKeys);

        Set<KeyStroke> backwardKeys = new HashSet<KeyStroke>(1);
        backwardKeys.add(KeyStroke.getKeyStroke(KeyEvent.VK_TAB, InputEvent.SHIFT_MASK | InputEvent.CTRL_MASK));
        setFocusTraversalKeys(KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS, backwardKeys);

        final KeyboardFocusManager manager = KeyboardFocusManager.getCurrentKeyboardFocusManager();
        
        AbstractAction stepForwardAction = new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                manager.focusNextComponent();

                // treat special cases
                SwingUtilities.invokeLater(()-> {
                    JButton tmp = selectedButton; // remember selected button
                    if (manager.getFocusOwner() instanceof javax.swing.JTextField) {
                        manager.focusNextComponent();
                    }
                    // handle case of first tab press after some text was typed
                    // the first button was the default button so jump to the next one
                    if (manager.getFocusOwner() == tmp) {
                        manager.focusNextComponent();
                    }
                });
            }
        };

        AbstractAction stepBackwardAction = new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent e) {
                manager.focusPreviousComponent();
                
                // treat special cases
                SwingUtilities.invokeLater(()-> {
                    if (manager.getFocusOwner() instanceof javax.swing.JTextField) {
                        manager.focusPreviousComponent();
                    }
                });
            }
        };

        getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke(KeyEvent.VK_TAB, 0), "TAB");
        getActionMap().put("TAB", stepForwardAction);

        getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, 0), "DOWN");
        getActionMap().put("DOWN", stepForwardAction);
        
        getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke(KeyEvent.VK_TAB, InputEvent.SHIFT_MASK), "shift TAB");
        getActionMap().put("shift TAB", stepBackwardAction);

        getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, 0), "UP");
        getActionMap().put("UP", stepBackwardAction);
    }

    
    private void updateButtons(String text) {
        text = text.toLowerCase();
    
        ArrayList<Pair> showList = new ArrayList<>();
        for(DialogEntry e: dialogEntries) {
            double s = e.calculateScore(text);
            if(s > 0) {
                showList.add( new Pair(s, e.button) );
            }
        }
        Collections.sort(showList);
        
        this.dialogs.removeAll();
        showList.forEach(pair -> this.dialogs.add(pair.b));
        
        
        if(selectedButton != null) {
            //selectedButton.setBackground(oldBackgrund);
            selectedButton.setSelected(false);
        }
        
        if(this.dialogs.getComponentCount() > 0) {
            selectedButton = (JButton)this.dialogs.getComponent(0);
            //oldBackgrund = selectedButton.getBackground();
            //selectedButton.setBackground(Color.cyan);
            selectedButton.setSelected(true);
            getRootPane().setDefaultButton(selectedButton);
        }

        this.revalidate();
        this.repaint();
    }
    
    public void close() {
        final Window w = SwingUtilities.getWindowAncestor(this);
        w.setVisible(false);
        search.setText("");
        updateButtons("");
        selectedButton = null;
    }
    
    
    KeyEventPostProcessor charCapture = new KeyEventPostProcessor() {
        @Override
        public boolean postProcessKeyEvent(KeyEvent e) {
            if(!DialogFastAccessPanel.this.isShowing()) {
                return false;
            }
            
            if(e.getID() == KeyEvent.KEY_PRESSED && 
               Character.isAlphabetic(e.getKeyChar())) 
            {
                search.setText(search.getText() + e.getKeyChar());
                search.requestFocus();
                return true;
            }
            
            if(e.getID() == KeyEvent.KEY_PRESSED && 
               e.getKeyCode() == KeyEvent.VK_BACK_SPACE) 
            {
                if(!search.getText().isEmpty()) {
                    search.setText(search.getText().substring(0, search.getText().length()-1));
                    search.requestFocus();
                    return true;
                }
            }

            return false;
        }
        
    };
    

    public void registerDialog(String name, String category, ActionListener action)
    {
        JButton b = new JButton(name);
        b.addActionListener((e) -> close());
        b.addActionListener(action);
        b.addFocusListener(new FocusListener() {
            @Override
            public void focusGained(FocusEvent e) {
                getRootPane().setDefaultButton(b);
            }
            
            @Override
            public void focusLost(FocusEvent e) {
            }
        });
        
        this.dialogEntries.add(new DialogEntry(name, category, "", b));
        this.dialogs.add(b);
        
        this.revalidate();
        this.repaint();
        
        //System.out.println(name);
    }

    /**
     * This method is called from within the constructor to initialize the form. WARNING: Do NOT
     * modify this code. The content of this method is always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        search = new javax.swing.JTextField();
        dialogs = new javax.swing.JPanel();

        setBackground(new java.awt.Color(153, 153, 153));

        dialogs.setBackground(javax.swing.UIManager.getDefaults().getColor("scrollpane_border"));
        dialogs.setOpaque(false);
        dialogs.setLayout(new java.awt.GridLayout(10, 5));

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(dialogs, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(search, javax.swing.GroupLayout.DEFAULT_SIZE, 380, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(search, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(dialogs, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel dialogs;
    private javax.swing.JTextField search;
    // End of variables declaration//GEN-END:variables
}
