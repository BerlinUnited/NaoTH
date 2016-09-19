package de.naoth.rc.opengl.file;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.drawings.GLDrawable;
import de.naoth.rc.opengl.model.GLClone;
import de.naoth.rc.opengl.model.GLModel;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.model.GLTexturedModel;
import de.naoth.rc.opengl.representations.Point3f;

public class GLScene {

    private final GL3 gl;

    private static final char OBJ = 'O';
    private static final char SHADER = 'S';

    private final LinkedList<GLObject> runQueue;

    private final HashMap<GLDrawable, GLObject> objects;
    private final HashMap<GLDrawable, Boolean> glDrawableObjectAllreadyCreated;

    private final Shader standardTexturedModelShader;
    private final Shader standardModelShader;

    private final String pathToGLSL = System.getProperty("user.dir").replaceAll("\\\\", "/") + "/src/de/naoth/rc/opengl/glsl/";

    public GLScene(GL3 gl) {
        this.gl = gl;

        this.runQueue = new LinkedList();
        this.objects = new HashMap();
        this.glDrawableObjectAllreadyCreated = new HashMap();

        standardTexturedModelShader = new Shader(gl, pathToGLSL, "vertex_shader.glsl", "texture_FS.glsl");
        standardModelShader = new Shader(gl, pathToGLSL, "color_VS.glsl", "color_FS.glsl");

        standardTexturedModelShader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        standardTexturedModelShader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});

        standardModelShader.setGlobalUniform("light.position", new float[]{0f, 50f, 0f});
        standardModelShader.setGlobalUniform("light.intensities", new float[]{1f, 1f, 1f});
    }

    public void add(String sceneFileName) {
        parseSceneFile(sceneFileName);
    }

    public void add(final GLDrawable newModel) {
        final Shader newModelShader = newModel.getShader(gl);

        if (glDrawableObjectAllreadyCreated.containsKey(newModel)) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    GLObject newGLObject;
                    while (true){
                        if(objects.containsKey(newModel)) break;
                    }
                    newGLObject = objects.get(newModel);

                    if (newModelShader == null) {
                        if (newGLObject.getClass().isInstance(GLTexturedModel.class)) {
                            newGLObject = new GLClone(gl, newGLObject, standardModelShader);
                        } else {
                            newGLObject = new GLClone(gl, newGLObject, standardTexturedModelShader);
                        }
                    } else {
                        newGLObject = new GLClone(gl, newGLObject, newModelShader);
                    }
                    
                    while (true){
                        if(newGLObject.isReady()) break;
                    }
                    runQueue.add(newGLObject);
                }

            }).start();

        } else {
            glDrawableObjectAllreadyCreated.put(newModel, true);

            new Thread(new Runnable() {
                @Override
                public void run() {
                    GLObject newGLObject;
                    Texture texture = newModel.getTexture();

                    if (texture == null) {
                        if (newModelShader == null) {
                            newGLObject = new GLModel(gl, newModel.getGLData(), standardModelShader);
                        } else {
                            newGLObject = new GLModel(gl, newModel.getGLData(), newModelShader);
                        }

                    } else if (newModelShader == null) {
                        newGLObject = new GLTexturedModel(gl, texture, newModel.getGLData(), standardTexturedModelShader);
                    } else {
                        newGLObject = new GLTexturedModel(gl, texture, newModel.getGLData(), newModelShader);
                    }

                    objects.put(newModel, newGLObject);

                    Point3f scale = newModel.getScale();
                    newGLObject.getModelMatrix().scale(scale.x, scale.y, scale.z);

                    runQueue.add(newGLObject);

                }

            }).start();
        }

    }

    public void add(GLDrawable[] scene) {
        for (GLDrawable each : scene) {
            this.add(each);
        }
    }

    //TODO
    private void parseSceneFile(String sceneFileName) {
        System.err.println("ERROR while parsing " + sceneFileName + ", scene files are currently not supported!");
        /*        
        HashMap<String, Shader> shader;
	
        HashMap<String, GLModel> models;
	
        HashMap<String, GLObject> objects;
        
        LinkedList<String> pathList;
        
        pathList = new LinkedList();
		shader = new HashMap();
		models = new HashMap();
		objects = new HashMap();


        
    	BufferedReader bufferedReader = null;
    	
    	try {
    		bufferedReader = new BufferedReader(new FileReader(new File(sceneFileName)));
    	}
    	catch(IOException io){
    		System.out.println("Can't open: " + sceneFileName);
    	}
    	
        String line = null;
        String[] token;
        
        char state = 0;
        String lastElement = "";
        
        while (true) {        	
        	try {
        		 line = bufferedReader.readLine();
        	}
        	catch(IOException io){
        		System.out.println("Can't read: " + sceneFileName);
        	}
        	
            if (null == line) {
                break;
            }
            if (line.length() == 0 || line.startsWith("#")) {
                continue;
            }
            if (line.startsWith("OBJ")) {
            	state = OBJ;
            	token = line.split("\\s+");
            	
            	if(models.containsKey(token[2])){
            		GLClone newDouble = new GLClone(gl, models.get(token[2]), shader.get(token[3]));
            		
            		objects.put(token[1], newDouble);
            		runQueue.add(newDouble);
            	} else {
            		String path = "/";
                	
                	for(String pathFinder: pathList) {
                		if(new File(System.getProperty("user.dir").replaceAll("\\\\", "/") + pathFinder + token[2]).exists()){
                			path = pathFinder;
                			break;
                		}
                	}
                	
                	String fullPath = System.getProperty("user.dir").replaceAll("\\\\", "/") + path;
                	
                	GLModel newModel;
                	
                	if(token.length < 5) {
                    	newModel = new GLModel(gl, new ObjFile(fullPath, token[2]), shader.get(token[3]));
                	} else {
                    	newModel = new GLTexturedModel(gl, new Texture(fullPath, token[4]), new ObjFile(fullPath, token[2]), shader.get(token[3]));
                	}
                	
                	models.put(token[2], newModel);
                	objects.put(token[1], newModel);
                	runQueue.add(newModel);
            	}
            	
            	lastElement = token[1];
            	
            } else if (line.startsWith("SHADER")) {
            	state = SHADER;
            	token = line.split("\\s+");
            	String path = "/";
            	
            	for(String pathFinder: pathList) {
            		if(new File(System.getProperty("user.dir").replaceAll("\\\\", "/") + pathFinder + token[2]).exists()){
            			path = pathFinder;
            			break;
            		}
            	}
            	
            	shader.put(token[1], new Shader(gl, System.getProperty("user.dir").replaceAll("\\\\", "/") + path, token[2], token[3]));
            	lastElement = token[1];
            	
            } else if (line.startsWith("PATH")) {
            	state = 0;
            	token = line.split("\\s+");
            	for(int i=1; i<token.length; i++) {
            		pathList.add(token[i]);
            	}
            	
        	} else if (line.startsWith(":UNIFORM") && state == SHADER) {
        		token = line.split("\\s+");
        		shader.get(lastElement).setGlobalUniform(token[1], new float[]{Float.parseFloat(token[2]), Float.parseFloat(token[3]), Float.parseFloat(token[4])});        		
        	
        	} else if (line.startsWith(":UNIFORM") && state == OBJ) {
        		token = line.split("\\s+");
        		objects.get(lastElement).setShaderUniform(token[1], new float[]{Float.parseFloat(token[2]), Float.parseFloat(token[3]), Float.parseFloat(token[4])});        		
        	
        	} else if (line.startsWith(":OFFSET") && state == OBJ) {
        		token = line.split("\\s+");
        		objects.get(lastElement).getModelMatrix().translate(Float.parseFloat(token[1]), Float.parseFloat(token[2]), Float.parseFloat(token[3]));
        		
        	} else if (line.startsWith(":SCALE") && state == OBJ) {
        		token = line.split("\\s+");
        		objects.get(lastElement).getModelMatrix().scale(Float.parseFloat(token[1]), Float.parseFloat(token[2]), Float.parseFloat(token[3])); 
        		
    		} else if (line.startsWith(":ROTATION") && state == OBJ) {
    			token = line.split("\\s+");
    			objects.get(lastElement).getModelMatrix().rotate(Float.parseFloat(token[1]), Float.parseFloat(token[2]), Float.parseFloat(token[3]), Float.parseFloat(token[4]));        		
    		}    
        }
        
    	try {
    		bufferedReader.close();
    	}
    	catch(IOException io){
    		System.out.println("Can't close: " + sceneFileName);
   		}
         */
    }

    public LinkedList<GLObject> getRunQueue() {
        return runQueue;
    }
}
