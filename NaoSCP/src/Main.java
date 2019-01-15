
import java.text.ParseException;
import java.util.StringTokenizer;

import javax.swing.JFrame;
import javax.swing.text.DefaultFormatter;
import naoscp.NaoSCP;

public class Main {
  public static void main(String args[]) throws ParseException {
    /* Set the Nimbus look and feel */
    //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
    /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
     * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
     */
    try {
        for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
            if ("Nimbus".equals(info.getName())) {
                javax.swing.UIManager.setLookAndFeel(info.getClassName());
                break;
            }
        }
    } catch (ClassNotFoundException ex) {
        java.util.logging.Logger.getLogger(NaoSCP.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    } catch (InstantiationException ex) {
        java.util.logging.Logger.getLogger(NaoSCP.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    } catch (IllegalAccessException ex) {
        java.util.logging.Logger.getLogger(NaoSCP.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    } catch (javax.swing.UnsupportedLookAndFeelException ex) {
        java.util.logging.Logger.getLogger(NaoSCP.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
    }
    //</editor-fold>
    
    final JFrame frame = new JFrame();
    final NaoSCP panel = new NaoSCP();
    
    // set frame attributes
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    frame.setTitle("NaoSCP " + NaoSCP.VERSION);
    frame.setLocationByPlatform(true);
    
    //frame.setMinimumSize(new java.awt.Dimension(525, 564));
    //frame.setPreferredSize(new java.awt.Dimension(1028, 564));
    //frame.setSize(1028, 564);
    // set content (panel)
    frame.setContentPane(panel);
    //content.setLayout(new BoxLayout(content, BoxLayout.PAGE_AXIS));
    //content.add(panel);
    
    frame.addWindowListener(new java.awt.event.WindowAdapter() {
        @Override
        public void windowClosing(java.awt.event.WindowEvent evt) {
            panel.formWindowClosing();
        }
    });
    
    /*
    JFormattedTextField ftf1 = new JFormattedTextField(new IPAddressFormatter());
    content.add(ftf1);

    content.add(new JTextField());
    */
    frame.pack();
    frame.setVisible(true);
  }
}
/**
 * A formatter for 4-byte IP addresses of the form a.b.c.d
 */
class IPAddressFormatter extends DefaultFormatter
{
   public String valueToString(Object value) throws ParseException
   {
      if (!(value instanceof byte[])) throw new ParseException("Not a byte[]", 0);
      byte[] a = (byte[]) value;
      if (a.length != 4) throw new ParseException("Length != 4", 0);
      StringBuilder builder = new StringBuilder();
      for (int i = 0; i < 4; i++)
      {
         int b = a[i];
         if (b < 0) b += 256;
         builder.append(String.valueOf(b));
         if (i < 3) builder.append('.');
      }
      return builder.toString();
   }

   public Object stringToValue(String text) throws ParseException
   {
      StringTokenizer tokenizer = new StringTokenizer(text, ".");
      byte[] a = new byte[4];
      for (int i = 0; i < 4; i++)
      {
         int b = 0;
         if (!tokenizer.hasMoreTokens()) throw new ParseException("Too few bytes", 0);
         try
         {
            b = Integer.parseInt(tokenizer.nextToken());
         }
         catch (NumberFormatException e)
         {
            throw new ParseException("Not an integer", 0);
         }
         if (b < 0 || b >= 256) throw new ParseException("Byte out of range", 0);
         a[i] = (byte) b;
      }
      if (tokenizer.hasMoreTokens()) throw new ParseException("Too many bytes", 0);
      return a;
   }
}