package de.naoth.rc.dialogsFx.multiagentconfiguration.components;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public class RequestTreeItem extends TreeNode<String>
{
    private String path;
    private String request;
    public boolean active;
    
    public RequestTreeItem(String path, String name) {
        this(path, name, false, name);
    }
    
    public RequestTreeItem(String path, String name, boolean selected) {
        this(path, name, selected, name);
    }
    
    public RequestTreeItem(String path, String name, boolean selected, String request) {
        super(name);
        this.path = path;
        this.request = request;
        active = selected;
    }
    
    public void setRequest(String request) {
        this.request = request;
    }
    
    public String getRequest() {
        return request;
    }
    
    public String getPath() {
        return path;
    }
}
