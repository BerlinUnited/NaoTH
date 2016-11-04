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
public class Head extends GLDrawable {

    public Head(String[] args) {
        super("head", 2);

        float[] primaryColor = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
        float[] secondaryColor = new float[]{0.0f, 0.0f, 1.0f, 1.0f};

        GLData mesh;
        Matrix4 offset;
        
        //Offset
        offset = new Matrix4();

        /**
         * load head
         */
        mesh = new ObjFile(super.pathToRES, "head.obj").buildGLData();

        super.primitives[0] = new Primitive("head", mesh, "ColorShader", offset);

        super.primitives[0].properties.put("color", primaryColor);

        /**
         * load colored head piece
         */
        mesh = new ObjFile(super.pathToRES, "head_top.obj").buildGLData();

        super.primitives[1] = new Primitive("TopHead", mesh, "ColorShader", offset);

        super.primitives[1].properties.put("color", secondaryColor);
    }

    @Override
    public void update(String[] args) {
        /*
        for (String arg : args) {
            System.out.print(arg + " ");
        }
        System.out.println();
        */

        Matrix4 offset = new Matrix4();//this.parseTransform3D(args, 2);
        offset.scale(0.2f, 0.2f, 0.2f);
        offset.translate(Float.parseFloat(args[2]), Float.parseFloat(args[3]), Float.parseFloat(args[4]));
       
        super.primitives[0].offset = offset;
        super.primitives[1].offset = offset;
    }

    protected Matrix4 parseTransform3D(String[] ds, int offset) {
        float[] m = new float[16];
        int k = offset;
        for (int j = 0; j < 12; j += 4) {
            m[j] = Float.parseFloat(ds[k++]);
            m[j + 1] = Float.parseFloat(ds[k++]);
            m[j + 3] = 0;
        }

        m[2] = m[4] * m[9] - m[5] * m[8];
        m[6] = m[1] * m[8] - m[0] * m[9];
        m[10] = m[0] * m[5] - m[1] * m[4];
        m[11] = 0;
        m[12] = Float.parseFloat(ds[k++]) * 0.001f;
        m[13] = Float.parseFloat(ds[k++]) * 0.001f;
        m[14] = Float.parseFloat(ds[k++]) * 0.001f;
        m[15] = 1;
        Matrix4 t3d = new Matrix4(m);
        
        return t3d;
    }

}
