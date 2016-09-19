/**
 * @author <a href="xu:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import de.naoth.rc.opengl.drawings.ExampleGLDrawable;
import de.naoth.rc.opengl.drawings.GLDrawable;
import de.naoth.rc.opengl.file.GLScene;
import de.naoth.rc.server.Command;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.logging.Level;
import java.util.logging.Logger;
import net.xeoh.plugins.base.annotations.PluginImplementation;

@PluginImplementation
public class GLViewerSceneManagerImpl extends AbstractManagerPlugin<GLDrawable[]>
    implements GLViewerSceneManager {

    private final String drawablePackageName = GLDrawable.class.getPackage().getName();

    @Override
    public GLDrawable[] convertByteArrayToType(byte[] result) throws IllegalArgumentException {
        GLDrawable[] scene;

        String[] messages = (new String(result)).split("\n");

        scene = new GLDrawable[messages.length];

        for (int i = 0; i < messages.length; i++) {
            String str = messages[i];
            if (!str.isEmpty()) {
                try {
                    String[] tokens = str.split(" ");
                    // get the class of the drawing
                    Class drawingClass = Class.forName(drawablePackageName + "." + tokens[0]);
                    // get the constructor which takes String[] as parameter
                    Constructor constructor = drawingClass.getConstructor(tokens.getClass());
                    // create new Drawing
                    Object drawing = constructor.newInstance(new Object[]{
                        tokens
                    });
                    if (drawing instanceof GLDrawable) {
                        scene[i] = (GLDrawable) drawing;
                    }
                } catch (InstantiationException ex) {
                    Logger.getLogger(GLViewerSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                } catch (IllegalAccessException ex) {
                    Logger.getLogger(GLViewerSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                } catch (InvocationTargetException ex) {
                    Logger.getLogger(GLViewerSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                } catch (NoSuchMethodException ex) {
                    Logger.getLogger(GLViewerSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                } catch (SecurityException ex) {
                    Logger.getLogger(GLViewerSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                } catch (ClassNotFoundException ex) {
                    Logger.getLogger(GLViewerSceneManagerImpl.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            if (scene[i] == null) {
                scene[i] = new ExampleGLDrawable();
            }
        }
        
        return scene;
    }

    @Override
    public Command getCurrentCommand() {
        return new Command("Cognition:representation:getbinary").addArg("DebugDrawings3D");
    }
}
