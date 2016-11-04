/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.ObjFile;
import de.naoth.rc.opengl.representations.Matrix4;
import de.naoth.rc.opengl.representations.Primitive;

/**
 *
 * @author robert
 */
public class LForeArm extends GLDrawable {

    public LForeArm(String[] args) {
        super("larm", 2);

        float[] primaryColor = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float[] secondaryColor = new float[]{0.0f, 0.0f, 1.0f, 1.0f};

        GLData mesh;
        Matrix4 offset;

        /**
         * load hand 
         */
        mesh = new ObjFile(super.pathToRES, "lhand.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[0] = new Primitive("lhand", mesh, "ColorShader", offset);

        super.primitives[0].properties.put("color", primaryColor);

        /**
         * load colored hand piece
         */
        mesh = new ObjFile(super.pathToRES, "lhand_color.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[1] = new Primitive("lhand_color", mesh, "ColorShader", offset);

        super.primitives[1].properties.put("color", secondaryColor);
    }
    
    public void update(String args[]) {
        Matrix4 offset = new Matrix4();//this.parseTransform3D(args, 2);
        offset.scale(0.2f, 0.2f, 0.2f);
        offset.translate(Float.parseFloat(args[2]), Float.parseFloat(args[3]), Float.parseFloat(args[4]));
        
        
        super.primitives[0].offset = offset;
        super.primitives[1].offset = offset;
    }

}
