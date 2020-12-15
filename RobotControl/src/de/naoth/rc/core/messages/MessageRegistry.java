package de.naoth.rc.core.messages;

import com.google.protobuf.Descriptors;
import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Message;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * @author Philipp Strobel <philippstrobel@posteo.de>
 */
public final class MessageRegistry
{
    /** The singleton instance of this registry */
    private static MessageRegistry instance;
    /** All found protobuf messages */
    private final Map<String, Descriptors.Descriptor> messages = new HashMap<>();
    
    /**
     * The singleton constructor.
     * Retrieves all available protobuf message descriptors and stores them to the map.
     */
    private MessageRegistry() {
        // list all proto files
        Object[] l = new Object[] {
            FrameworkRepresentations.getDescriptor().getMessageTypes(),
            CommonTypes.getDescriptor().getMessageTypes(),
            Messages.getDescriptor().getMessageTypes(),
            Representations.getDescriptor().getMessageTypes(),
            AudioDataOuterClass.getDescriptor().getMessageTypes(),
            RobotPoseOuterClass.getDescriptor().getMessageTypes(),
            TeamMessageOuterClass.getDescriptor().getMessageTypes()
        };
        
        // add all proto messages to the map
        for (Object o : l) {
            ((List<Descriptors.Descriptor>)o).forEach(d -> {
                messages.put(d.getName(), d);
            });
        }
    }
    
    /**
     * Returns the singleton instance.
     * 
     * @return the singleton instance of this registry
     */
    private static MessageRegistry getInstance() {
        if (instance == null) {
            instance = new MessageRegistry();
        }
        return instance;
    }
    
    /**
     * Returns a list of all available protobuf messages (names).
     * 
     * @return name of available messages
     */
    public static Set<String> list()
    {
        return getInstance().messages.keySet();
    }
    
    /**
     * Returns true, if the protobuf message with the name exists, false otherwise.
     * 
     * @param name  name of the protobuf message to check
     * @return      true, if message is available, false otherwise
     */
    public static boolean has(String name) {
        return getInstance().messages.containsKey(name);
    }

    /**
     * Returns the protobuf message descriptor with the name or null if the messsage
     * name could not be found.
     * 
     * @param name  name of the message
     * @return      protobuf message descriptor or null
     */
    public static Descriptors.Descriptor get(String name) {
        return getInstance().messages.getOrDefault(name, null);
    }
    
    /**
     * Parses a protobuf message of the given name and returns the message object.
     * 
     * @param name  name of the protobuf message
     * @param data  the data to parse
     * @return      the message object with the parsed data
     * @throws InvalidProtocolBufferException is thrown, if the message name doesn't exists or if parsing fails
     */
    public static Message parse(String name, byte[] data) throws InvalidProtocolBufferException {
        // check if the message is known
        if (has(name)) {
            // ... and return the parsed message
            return DynamicMessage.parseFrom(get(name), data);
        }
        // ... otherwise throw an descriptive error
        throw new InvalidProtocolBufferException("Unknown message type!");
    }
}
