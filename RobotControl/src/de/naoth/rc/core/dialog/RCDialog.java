package de.naoth.rc.core.dialog;


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;



@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface RCDialog {
    
    public interface CategoryElement {
        public char mnemonic();
    }
    
    public enum Category implements CategoryElement {
        Tools {
            @Override
            public char mnemonic() { return 'T'; }
        },
        View {
            @Override
            public char mnemonic() { return 'V'; }
        },
        Log {
            @Override
            public char mnemonic() { return 'L'; }
        },
        Status {
            @Override
            public char mnemonic() { return 'S'; }
        },
        Debug {
            @Override
            public char mnemonic() { return 'D'; }
        },
        Configure {
            @Override
            public char mnemonic() { return 'C'; }
        },
        Team {
            @Override
            public char mnemonic() { return 'M'; }
        }
    }
    
    // ACHTUNG: default has to be empty string
    String name() default "";
    // ACHTUNG: default has to be empty string
    Category category();
}
