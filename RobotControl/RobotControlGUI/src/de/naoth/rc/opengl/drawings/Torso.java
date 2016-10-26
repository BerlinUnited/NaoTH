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
public class Torso extends GLDrawable {

    public Torso(String[] args) {
        super("torso", 3);

        float[] primaryColor = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float[] secondaryColor = new float[]{0.0f, 0.0f, 1.0f, 1.0f};

        GLData mesh;
        Matrix4 offset;

        /**
         * load torso 
         */
        mesh = new ObjFile(super.pathToRES, "torso.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[0] = new Primitive("torso", mesh, "ColorShader", offset);

        super.primitives[0].properties.put("color", primaryColor);

        /**
         * load colored torso piece
         */
        mesh = new ObjFile(super.pathToRES, "torso_colored_plate.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[1] = new Primitive("torso_plate", mesh, "ColorShader", offset);

        super.primitives[1].properties.put("color", secondaryColor);

        /**
         * load button
         */
        mesh = new ObjFile(super.pathToRES, "torso_button.obj").buildGLData();

        //Offset
        offset = new Matrix4();
        offset.scale(0.2f, 0.2f, 0.2f);

        super.primitives[2] = new Primitive("button", mesh, "ColorShader", offset);

        super.primitives[2].properties.put("color", secondaryColor);
    }

}
