/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.components.treetable;

import java.awt.Container;
import java.awt.GridLayout;
 
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
 
public class Example extends JFrame {
 
     
    public Example() {
        super("Tree Table Demo");
             
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
 
        setLayout(new GridLayout(0, 1));
 
        ModifyDataModel treeTableModel = new ModifyDataModel();
 
        treeTableModel.insertPath("A:b:n",':').enabled = true;
        treeTableModel.insertPath("A:c:m",':').enabled = true;
        treeTableModel.insertPath("A:b:k",':').enabled = true;
        treeTableModel.insertPath("B:b:n",':').enabled = false;
        treeTableModel.insertPath("B:c:m",':').enabled = true;
        treeTableModel.insertPath("B:a:k",':').enabled = true;
        
        
        
        TreeTable myTreeTable = new TreeTable(treeTableModel);
 
        Container cPane = getContentPane();
         
        cPane.add(new JScrollPane(myTreeTable));
     
        //setSize(1000, 800);
        setLocationRelativeTo(null);
         
 
    }
   
 
    public static void main(final String[] args) {
        Runnable gui = new Runnable() {
 
            public void run() {
                try {
                    UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
                } catch (Exception e) {
                    e.printStackTrace();
                }
                new Example().setVisible(true);
            }
        };
        SwingUtilities.invokeLater(gui);
    }
}
