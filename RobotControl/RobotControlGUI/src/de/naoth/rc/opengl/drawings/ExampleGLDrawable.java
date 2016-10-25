/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package de.naoth.rc.opengl.drawings;

import de.naoth.rc.opengl.file.GLData;
import de.naoth.rc.opengl.file.ObjFile;
import de.naoth.rc.opengl.file.Texture;
import de.naoth.rc.opengl.representations.Matrix4;
import de.naoth.rc.opengl.representations.Primitive;

/**
 *
 * @author Robert
 */
public class ExampleGLDrawable extends GLDrawable {

    public ExampleGLDrawable(String args[]) {
        super("example", 2);

        GLData mesh;
        Matrix4 offset;

        mesh = new ObjFile(super.pathToRES, "ball.obj").buildGLData();

        //offset
        offset = new Matrix4();
        offset.translate(((float) Math.random() * 9) - 4.5f, 0.18f, ((float) Math.random() * 6) - 3f);

        super.primitives[0] = new Primitive("example1", mesh, "TexturedModelShader", offset, new Texture(super.pathToRES, "replacement.png"));

        
        
        
        mesh = new ObjFile(super.pathToRES, "palm01.obj").buildGLData();

        //offset
        offset = new Matrix4();
        offset.translate(((float) Math.random() * 9) - 4.5f, 0f, ((float) Math.random() * 6) - 3f);

        super.primitives[1] = new Primitive("example2", mesh, "ColorShader", offset, new Texture(super.pathToRES, "replacement.png"));

        super.primitives[1].properties.put("color", new float[]{(float) Math.random(), (float) Math.random(), (float) Math.random(), 1.0f});
    }

    public void update(String args[]) {
        Matrix4 offset;

        //update offset
        offset = new Matrix4();
        offset.translate(((float) Math.random() * 9) - 4.5f, 0.18f, ((float) Math.random() * 6) - 3f);

        super.primitives[0].offset = offset;

        
        
        
        //update offset
        offset = new Matrix4();
        offset.translate(((float) Math.random() * 9) - 4.5f, 0f, ((float) Math.random() * 6) - 3f);

        super.primitives[1].offset = offset;

        super.primitives[1].properties.put("color", new float[]{(float) Math.random(), (float) Math.random(), (float) Math.random(), 1.0f});
    }

}
