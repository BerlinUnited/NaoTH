/*
 */
package naoscp.tools;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 *
 * @author Heinrch Mellmann
 */
public class Config {

    private String name;
    public final Map<String, String> values = new HashMap<String, String>();

    public Config(String name) {
        this.name = name;
    }

    public void writeToFile(File configFile) throws IOException {
        StringBuilder c = new StringBuilder();

        c.append('[').append(name).append(']').append('\n');

        for (Map.Entry<String, String> entry : values.entrySet()) {
            c.append(entry.getKey()).append('=').append(entry.getValue()).append('\n');
        }

        FileUtils.writeToFile(c.toString(), configFile);
    }

    public void readFromFile(File configFile) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(configFile));
        String line = reader.readLine();

        // parse the group
        Pattern groupPattern = Pattern.compile("\\[(.+)\\]");
        Matcher groupMatcher = groupPattern.matcher(line);
        // skip empty lines until the group starts
        while (line != null && !groupMatcher.matches()) {
            line = reader.readLine();
        }

        name = groupMatcher.group(1);
                //System.out.println(name);

        // parse the rest of the file
        Pattern keyValuePattern = Pattern.compile("\\s*(.+)\\s*=\\s*(.+)\\s*");
        line = reader.readLine();

        while (line != null) {
            Matcher keyValueMatcher = keyValuePattern.matcher(line);

            if (keyValueMatcher.matches()) {
                String key = keyValueMatcher.group(1);
                String value = keyValueMatcher.group(2);
                //System.out.println(key + " = " + value);
                values.put(key, value);
            }
            line = reader.readLine();
        }
    }
}//end class Config
