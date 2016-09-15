package de.naoth.rc.opengl.file;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;

import com.jogamp.opengl.GL3;
import de.naoth.rc.opengl.Shader;
import de.naoth.rc.opengl.model.GLClone;
import de.naoth.rc.opengl.model.GLModel;
import de.naoth.rc.opengl.model.GLObject;
import de.naoth.rc.opengl.model.GLTexturedModel;


public class GLScene {
	private GL3 gl;
	
	private static final char OBJ = 'O';
	private static final char SHADER = 'S';
	
	private LinkedList<String> pathList;
	
	private HashMap<String, Shader> shader;
	
	private HashMap<String, GLModel> models;
	
	private HashMap<String, GLObject> objects;
	
	private LinkedList<GLObject> runQueue;
	
	
	public GLScene(GL3 gl, String sceneFileName) {
		this.gl = gl;
		pathList = new LinkedList<String>();
		shader = new HashMap<String, Shader>();
		models = new HashMap<String, GLModel>();
		objects = new HashMap<String, GLObject>();
		runQueue = new LinkedList<GLObject>();
		
		parseSceneFile(sceneFileName);
	}
	
	
    private void parseSceneFile(String sceneFileName) {    	
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
            	
            	shader.put(token[1], new Shader(gl, path, token[2], token[3]));
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
    }
    
    public LinkedList<GLObject> getModelList() {
    	return runQueue;
    }
}
