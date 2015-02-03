/*
 */

package naoscp.tools;

import com.jcraft.jsch.SftpProgressMonitor;
import javax.swing.JProgressBar;

/**
 *
 * @author claas
 * @author Heinrich Mellmann
 */
public class BarProgressMonitor implements SftpProgressMonitor {
    private final JProgressBar progressBar;
    private long max = 0;
    
    public BarProgressMonitor(JProgressBar progressBar) {
        this.progressBar = progressBar;
    }

    @Override
    public void init(int op, String src, String dest, long max) {
        this.max = max;
        progressBar.setIndeterminate(false);
        progressBar.setValue(0);
        progressBar.setMinimum(0);
        progressBar.setMaximum(100);
        progressBar.setEnabled(true);
    }

    @Override
    public boolean count(long count) {
        long percent = (count * 100) / max;
        progressBar.setValue((int) percent);
        return true;
    }

    @Override
    public void end() {
        progressBar.setEnabled(false);
    }
}
