package de.naoth.rc.opengl.model;

import de.naoth.rc.opengl.representations.Matrix4;


public interface GLDrawable {
	
    void display(Matrix4 viewPoint);
    
    void reshape();
    
    void dispose();

}