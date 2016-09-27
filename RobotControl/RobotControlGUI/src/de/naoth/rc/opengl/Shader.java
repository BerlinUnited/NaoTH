package de.naoth.rc.opengl;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.representations.Matrix4;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * Based on GLSLProgramObject.h - Wrapper for GLSL program objects
 *
 * OriginalConcept: Louis Bavoil
 *
 * Edited by Robert Martin
 *           Peter Woltersdorf
 */

public class Shader{
    
    public final char VERTEX_SHADER = 'V';
    public final char FRAGMENT_SHADER = 'F';

    protected ArrayList<Integer> vertexShader;
    protected ArrayList<Integer> fragmentShader;
    private Map<String, Object> globalUniforms;
    
    private int progId;

    private Shader(GL3 gl) {
        vertexShader = new ArrayList<>();
        fragmentShader = new ArrayList<>();
        progId = 0;
        globalUniforms = new HashMap<>();
    }

    public Shader(GL3 gl, String shaderFilepath, String vertexShader, String fragmentShader) {

        this(gl);

        attachShader(gl, shaderFilepath + vertexShader, VERTEX_SHADER);
        attachShader(gl, shaderFilepath + fragmentShader, FRAGMENT_SHADER);

        initializeProgram(gl, true);
    }

    public Shader(GL3 gl, String shaderFilepath, String[] vertexShader, String[] fragmentShader) {

        this(gl);

        for (String vShader : vertexShader) {
            attachShader(gl, shaderFilepath + vShader, VERTEX_SHADER);
        }
        for (String fShader : fragmentShader) {
            attachShader(gl, shaderFilepath + fShader, FRAGMENT_SHADER);
        }

        initializeProgram(gl, true);
    }
    
    
    public final void initializeProgram(GL3 gl, boolean cleanUp) {
        progId = gl.glCreateProgram();

        for (Integer vShader : vertexShader) {
            gl.glAttachShader(progId, vShader);
        }

        for (Integer fShader : fragmentShader) {
            gl.glAttachShader(progId, fShader);
        }

        gl.glLinkProgram(progId);

        int[] params = new int[]{0};
        gl.glGetProgramiv(progId, GL3.GL_LINK_STATUS, params, 0);

        if (params[0] != 1) {

            System.err.println("link status: " + params[0]);
            gl.glGetProgramiv(progId, GL3.GL_INFO_LOG_LENGTH, params, 0);
            System.err.println("log length: " + params[0]);

            byte[] abInfoLog = new byte[params[0]];
            gl.glGetProgramInfoLog(progId, params[0], params, 0, abInfoLog, 0);
            System.err.println(new String(abInfoLog));
        }

        gl.glValidateProgram(progId);

        if (cleanUp) {
            for (Integer vShader : vertexShader) {
                gl.glDetachShader(progId, vShader);
                gl.glDeleteShader(vShader);
            }

            for (Integer fShader : fragmentShader) {
                gl.glDetachShader(progId, fShader);
                gl.glDeleteShader(fShader);
            }
        }
    }
    

    public void destroy(GL3 gl) {
        for (Integer vShader : vertexShader) {
            gl.glDeleteShader(vShader);
        }
        for (Integer fShader : fragmentShader) {
            gl.glDeleteShader(fShader);
        }
        if (progId != 0) {
            gl.glDeleteProgram(progId);
        }
    }

    public void bind(GL3 gl) {
        if(progId != -1) {
            gl.glUseProgram(progId);
            bindGlobalUniforms(gl);
        }
    }

    public void unbind(GL3 gl) {
        gl.glUseProgram(0);
    }

    public final void attachShader(GL3 gl, String filename, char shader) {
        InputStream inputStream = null;

        try {
            inputStream = new FileInputStream(filename);
        } catch (FileNotFoundException e) {
            System.err.println("Unable to find the shader file " + filename);
        }
        if (inputStream == null) {
            System.err.println("Problem with InputStream");
            return;
        }

        String content = "";
        BufferedReader input = null;

        try {
            input = new BufferedReader(new InputStreamReader(inputStream));
            String line;
            while ((line = input.readLine()) != null) {
                content += line + "\n";
            }
        } catch (FileNotFoundException fileNotFoundException) {
            System.err.println("Unable to find the shader file " + filename);
        } catch (IOException iOException) {
            System.err.println("Problem reading the shader file " + filename);
        } finally {
            try {
                if (input != null) {
                    input.close();
                }
            } catch (IOException iOException) {
                System.out.println("Problem closing the BufferedReader, " + filename);
            }
        }

        int iID;
        switch (shader){
            case FRAGMENT_SHADER: iID = gl.glCreateShader(GL3.GL_FRAGMENT_SHADER);
                break;
            case VERTEX_SHADER: iID = gl.glCreateShader(GL3.GL_VERTEX_SHADER);
                break;
            default: System.err.println("Shader not supported"); iID = 0;
                break;
        }

        String[] akProgramText = new String[1];
        // find and replace program name with "main"
        akProgramText[0] = content;

        int[] params = new int[]{0};

        int[] aiLength = new int[1];
        aiLength[0] = akProgramText[0].length();
        int iCount = 1;

        gl.glShaderSource(iID, iCount, akProgramText, aiLength, 0);

        gl.glCompileShader(iID);

        gl.glGetShaderiv(iID, GL3.GL_COMPILE_STATUS, params, 0);

        if (params[0] != 1) {
            System.err.println(filename);
            System.err.println("compile status: " + params[0]);
            gl.glGetShaderiv(iID, GL3.GL_INFO_LOG_LENGTH, params, 0);
            System.err.println("log length: " + params[0]);
            byte[] abInfoLog = new byte[params[0]];
            gl.glGetShaderInfoLog(iID, params[0], params, 0, abInfoLog, 0);
            System.err.println(new String(abInfoLog));
            System.exit(-1);
        }

        switch (shader){
            case FRAGMENT_SHADER: fragmentShader.add(iID);
                break;
            case VERTEX_SHADER: vertexShader.add(iID);
                break;
        }
    }
    
    
    

    public void setGlobalUniform(String name, Object object)  {
        if(progId != -1) globalUniforms.put(name, object);
    }

    public Map<String, Object> getGlobalUniforms() {
        return globalUniforms;
    }

    public void bindGlobalUniforms(GL3 gl){
        for (Map.Entry<String, Object> entry : getGlobalUniforms().entrySet()) {
            setUniform(gl, entry.getKey(), entry.getValue());
        }
    }

    public void setUniform(GL3 gl, String name, Object object) {
        if (object instanceof float[]) {
            setUniform(gl, name,(float[]) object );
        } else if (object instanceof Integer) {
            setUniform(gl, name, (int) object);
        } else if (object instanceof Matrix4) {
            setUniform(gl, name, (Matrix4) object);
        } else {
            System.err.println("Warning: Invalid uniform parameter \"" + name +"\"");
        }

    }

    private void setUniform(GL3 gl3, String name, float[] val) {
        int id = gl3.glGetUniformLocation(progId, name);
        if (id == -1) {
            System.err.println("Warning: Invalid uniform parameter \"" + name +"\"");
            return;
        }
        switch (val.length) {
            case 1:
                gl3.glUniform1fv(id, 1, val, 0);
                break;
            case 2:
                gl3.glUniform2fv(id, 1, val, 0);
                break;
            case 3:
                gl3.glUniform3fv(id, 1, val, 0);
                break;
            case 4:
                gl3.glUniform4fv(id, 1, val, 0);
                break;
        }
    }

    private void setUniform(GL3 gl, String name, int val) {
        int id = gl.glGetUniformLocation(progId, name);

        if (id == -1) {
            System.err.println("Warning: Invalid uniform parameter " + name);
            return;
        }
        gl.glUniform1i(id, val);
    }

    private void setUniform(GL3 gl, String name, Matrix4 matrix) {
        int id = gl.glGetUniformLocation(progId, name);

        if (id == -1) {
            System.err.println("Warning: Invalid uniform parameter " + name);
            return;
        }
        gl.glUniformMatrix4fv(id, 1, false, matrix.get(), 0);
    }

    public Integer getProgramId() {
        return progId;
    }
}
