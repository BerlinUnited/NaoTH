/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc;

import java.awt.Color;
import java.awt.DefaultKeyboardFocusManager;
import java.awt.KeyEventPostProcessor;
import java.awt.Window;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.util.HashMap;
import javax.swing.JButton;
import javax.swing.SwingUtilities;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.PlainDocument;

/**
 *
 * @author gxy
 */
public class DialogFastAccessPanel extends javax.swing.JPanel {

    private final HashMap<String, JButton> buttons = new HashMap<String, JButton>();
    
    /**
     * Creates new form DialogFastAccessPanel
     */
    public DialogFastAccessPanel() {
        initComponents();
        
        //for (KeyListener k: this.search.getKeyListeners())
        //    SwingUtilities.getWindowAncestor(this).addKeyListener(k);
        
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
    
    private void updateButtons(String text) {
        text = text.toLowerCase();
        
        this.dialogs.removeAll();
        
        JButton newSelectedButton = null;
        for(JButton b: buttons.values()) {
            if(b.getText().toLowerCase().startsWith(text)) {
               this.dialogs.add(b, 0); 
               newSelectedButton = b;
            }
            else if(b.getText().toLowerCase().contains(text)) {
               this.dialogs.add(b);
               if(newSelectedButton == null) {
                   newSelectedButton = b;
               }
            }
        }
        
        if(selectedButton != null) {
            //selectedButton.setBackground(oldBackgrund);
            selectedButton.setSelected(false);
        }
        
        if(newSelectedButton != null) {
            selectedButton = newSelectedButton;
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
            if(e.getID()        == KeyEvent.KEY_PRESSED && 
               Character.isAlphabetic(e.getKeyChar())) 
            {
                search.setText(search.getText() + e.getKeyChar());
                search.requestFocus();
                return true;
            }
            
            if(e.getID()        == KeyEvent.KEY_PRESSED && 
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
        
        this.buttons.put(name, b);
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
