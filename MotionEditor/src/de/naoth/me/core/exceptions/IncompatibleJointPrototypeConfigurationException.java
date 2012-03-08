/*
 * This Exception is thrown everytime when ...
 */

package de.naoth.me.core.exceptions;

/**
 *
 * @author Heinrich Mellmann
 */
public class IncompatibleJointPrototypeConfigurationException extends Exception {

    /**
     * Creates a new instance of <code>IncompatibleJointPrototypeConfigurationException</code> 
     * without detail message.
     */
    public IncompatibleJointPrototypeConfigurationException() {
    }


    /**
     * Constructs an instance of <code>IncompatibleJointPrototypeConfigurationException</code> 
     * with the specified detail message.
     * @param msg the detail message.
     */
    public IncompatibleJointPrototypeConfigurationException(String msg) {
        super(msg);
    }
}//end IncompatibleJointPrototypeConfigurationException
