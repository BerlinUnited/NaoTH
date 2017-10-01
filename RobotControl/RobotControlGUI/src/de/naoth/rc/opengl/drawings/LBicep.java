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
@Deprecated
public class LBicep extends GLDrawable {

    public LBicep(String[] args) {
        super("lbicep", 3);

        float[] primaryColor = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float[] secondaryColor = new float[]{0.0f, 0.0f, 1.0f, 1.0f};
        float[] jointColor = new float[]{0.5f, 0.5f, 0.5f, 1.0f};

        GLData mesh;
        Matrix4 offset;

        /**
         * load torso 
         */
        mesh = new ObjFile(super.pathToRES, "lbiceps.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[0] = new Primitive("lbicep", mesh, "ColorShader", offset);

        super.primitives[0].properties.put("color", primaryColor);

        /**
         * load colored torso piece
         */
        mesh = new ObjFile(super.pathToRES, "lbiceps_color.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[1] = new Primitive("lbicep_color", mesh, "ColorShader", offset);

        super.primitives[1].properties.put("color", secondaryColor);

        /**
         * load button
         */
        mesh = new ObjFile(super.pathToRES, "lbiceps_joint.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[2] = new Primitive("lbicep_joint", mesh, "ColorShader", offset);

        super.primitives[2].properties.put("color", jointColor);
    }
    
        public void update(String args[]) {
        Matrix4 offset = new Matrix4();//this.parseTransform3D(args, 2);
        offset.scale(0.2f, 0.2f, 0.2f);
        offset.translate(Float.parseFloat(args[2]), Float.parseFloat(args[3]), Float.parseFloat(args[4]));
        
        
        super.primitives[0].offset = offset;
        super.primitives[1].offset = offset;
        super.primitives[2].offset = offset;
    }

}
