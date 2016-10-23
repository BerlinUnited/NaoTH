/**
 * @author <a href="xu:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
 */
package de.naoth.rc.manager;

import de.naoth.rc.core.manager.AbstractManagerPlugin;
import de.naoth.rc.opengl.drawings.GLDrawable2;
import de.naoth.rc.server.Command;
import net.xeoh.plugins.base.annotations.PluginImplementation;

@PluginImplementation
public class GLViewerSceneManagerImpl extends AbstractManagerPlugin<String[][]>
    implements GLViewerSceneManager {

    private final String drawablePackageName = GLDrawable2.class.getPackage().getName();

    @Override
    public String[][] convertByteArrayToType(byte[] result) throws IllegalArgumentException {
        String[][] scene;

        String[] messages = (new String(result)).split("\n");

        scene = new String[messages.length][];

        for (int i = 0; i < messages.length; i++) {
            String str = messages[i];
            if (!str.isEmpty()) {
                String[] tokens = str.split(" ");
                
                tokens[0] = drawablePackageName + tokens[0];
                
                scene[i] = tokens;
            }
        }

        return scene;
    }

    @Override
    public Command getCurrentCommand() {
        return new Command("Cognition:representation:getbinary").addArg("DebugDrawings3D");
    }
}
