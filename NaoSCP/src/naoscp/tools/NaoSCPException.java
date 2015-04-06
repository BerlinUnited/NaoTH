/*
 */

package naoscp.tools;

/**
 *
 * @author Heinrich Mellmann
 */
public class NaoSCPException extends Exception {
    public NaoSCPException() { super(); }
    public NaoSCPException(String message) { super(message); }
    public NaoSCPException(String message, Throwable cause) { super(message, cause); }
    public NaoSCPException(Throwable cause) { super(cause); }
}