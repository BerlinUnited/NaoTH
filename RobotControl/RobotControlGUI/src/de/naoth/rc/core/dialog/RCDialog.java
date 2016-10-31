package de.naoth.rc.core.dialog;


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;


@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface RCDialog {
    // ACHTUNG: default has to be empty string
    String name() default "";
    // ACHTUNG: default has to be empty string
    String category() default "";
}
