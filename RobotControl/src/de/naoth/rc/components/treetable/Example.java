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
 
        ParameterDataModel treeTableModel = new ParameterDataModel();
 
        treeTableModel.insertPath("A:b:n",":").setValue(new Boolean(true));
        treeTableModel.insertPath("A:c:m",":").setValue(new Double(1.0));
        treeTableModel.insertPath("A:b:k",":").setValue(new Double(2.0));
        treeTableModel.insertPath("B:b:n",":").setValue(new Double(1.0));
        treeTableModel.insertPath("B:c:m",":").setValue(new Double(1.0));
        treeTableModel.insertPath("B:a:k",":").setValue(new Double(1.0));
        
        
        
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
