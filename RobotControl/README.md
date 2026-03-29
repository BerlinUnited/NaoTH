
## Setup

1. Install Java 21 LTS SDK (see versions)
    - set active java version:
      ```
      JAVA_HOME=<path to java install directory>
      ```
      add to path
      ```
      %JAVA_HOME%/bin
      ```

2. Compile and package (jar): In `naoth-2020/RobotControl` run
    ```sh
    ./gradlew clean build
    ```
    or
    ```sh
    ./gradlew.bat clean build
    ```
3. Compile and run: In `naoth-2020/RobotControl` runexecute
    ```sh
    ./gradlew
    ```
    or
    ```sh
    ./gradlew.bat
    ```

4. For development of RobotControl: install Netbeans (see versions)


## Versions

- Java 21 LTS
  - this is a reference version, other versions might work as well (see Limitations)
  - https://www.oracle.com/de/java/technologies/downloads/#jdk21-windows
  - https://www.oracle.com/de/java/technologies/downloads/#jdk21-linux
  - https://www.oracle.com/de/java/technologies/downloads/#jdk21-mac
  
- Gradle 8.14.4
    - Gradle version is fixed and is installed throgh a wrapper
    - https://docs.gradle.org/current/userguide/gradle_wrapper.html

- Netbeans 29
    - other version might work as well
    - https://netbeans.apache.org/front/main/download/


## Limitations

- Gradle 8.14.4 does not work with Java 25 and later
- Gradle 9.x does not work with Java earlier than 17


## Updating to a higher version

1. check gradle compatibility and choose a version:
    https://docs.gradle.org/current/userguide/compatibility.html


2. update the gradle wrapper
    https://docs.gradle.org/current/userguide/gradle_wrapper.html

    ```sh
    gradle wrapper --gradle-version 8.14.4
    ```


