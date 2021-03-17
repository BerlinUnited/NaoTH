/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.rc.logmanager;

/**
 *
 * @author thomas
 */
public class LogDataFrame {
    private final int number;
    private final String name;
    private final byte[] data;

    public LogDataFrame(int number, String name, byte[] data) {
        this.number = number;
        this.name = name;
        this.data = data;
    }

    public int getNumber() {
        return number;
    }

    public String getName() {
        return name;
    }

    public byte[] getData() {
        return data;
    }
    
} //end LFrame
