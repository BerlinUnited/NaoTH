/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;

import de.naoth.me.core.Scanner;

import java.io.*;
import javax.swing.JOptionPane;
import js.math.vector.*;
import java.util.ArrayList;
import java.util.Hashtable;

//import de.hu_berlin.informatik.ki.motioneditor.core.Scanner;

/**
 *
 * @author claas
 */

public class ParserVRML
{

  private Scanner scanner;
  private boolean occuredError = false;

  public ArrayList<SpotlightD> spotLights;
  public ArrayList<ObjectVRML> ObjectsVRML;

//  Hashtable<String, MeshD> namedMeshes;
//  Hashtable<String, SphereD> namedSpheres;
//  Hashtable<String, BoxD> namedBoxes;
//  Hashtable<String, CylinderD> namedCylinders;
//  Hashtable<String, IndexedFaceSetD> namedIndexedFaceSets;
//  Hashtable<String, IndexedLineSetD> namedIndexedLineSets;
  Hashtable<String, SpotlightD> namedSpotlights;
  Hashtable<String, TransformD> namedTransforms;
  Hashtable<String, ShapeD> namedShapes;
  Hashtable<String, AppearanceD> namedAppearances;
  Hashtable<String, MaterialD> namedMaterials;
  Hashtable<String, GeometryD> namedGeometries;
  Hashtable<String, ArrayList<Vec3d>> namedCoordinates;
  Hashtable<String, ArrayList<Vec3d>> namedTexCoordinates;



  private enum KeyWordID
  {
    DEF,
    USE,
    FALSE,
    TRUE,
    Transform,
    Coordinate,
    IndexedLineSet,
    IndexedFaceSet,
    Shape,
    Box,
    Sphere,
    Cylinder,
    Appearance,
    Material,
    ImageTexture,
    TextureCoordinate,
    SpotLight,
    children,
    translation,
    rotation,
    appearance,
    geometry,
    size,
    top,
    bottom,
    height,
    radius,
    coord,
    coordIndex,
    point,
    creaseAngle,
    material,
    texture,
    texCoord,
    texCoordIndex,
    url,
    emissiveColor,
    specularColor,
    diffuseColor,
    shininess,
    on,
    color,
    location,
    direction,
    attenuation,
    beamWidth,
    cutOffAngle,
    intensity,
    ambientIntensity,
    blockObjectOpen,
    blockObjectClose,
    blockArrayOpen,
    blockArrayClose,
    doubleQuote,
    separator
  };

             
  ParserVRML()
  {
    spotLights = new ArrayList<SpotlightD>();
    ObjectsVRML = new ArrayList<ObjectVRML>();

    namedSpotlights = new Hashtable<String, SpotlightD>();
    namedTransforms = new Hashtable<String, TransformD>();
    namedShapes = new Hashtable<String, ShapeD>();
    namedAppearances = new Hashtable<String, AppearanceD>();
    namedMaterials = new Hashtable<String, MaterialD>();
    namedGeometries = new Hashtable<String, GeometryD>();
    namedCoordinates = new Hashtable<String, ArrayList<Vec3d>>();
    namedTexCoordinates = new Hashtable<String, ArrayList<Vec3d>>();


  }


  private void eat() throws IOException
  {
    scanner.getNextToken();
  }
  //end eat

  private boolean isToken(KeyWordID tokenId)
  {
    return scanner.isToken(getKeyWord(tokenId));
  }//end isToken

  private void isTokenAndEat(KeyWordID tokenId) throws IOException
  {
    if(isToken(tokenId))
      eat();
    else
      ;//throw "ERROR: Syntaxfehler in Zeile %d.\n";
  }
  //end isTokenAndEat


  private String getKeyWord(KeyWordID keyWordID)
  {
    switch(keyWordID)
    {
      case DEF: return "DEF";
      case USE: return "USE";
      case FALSE: return "FALSE";
      case TRUE: return "TRUE";
      case Transform: return "Transform";
      case Coordinate: return "Coordinate";
      case IndexedLineSet: return "IndexedLineSet";
      case IndexedFaceSet: return "IndexedFaceSet";
      case Shape: return "Shape";
      case Box: return "Box";
      case Sphere: return "Sphere";
      case Cylinder: return "Cylinder";
      case Appearance: return "Appearance";
      case Material: return "Material";
      case ImageTexture: return "ImageTexture";
      case TextureCoordinate: return "TextureCoordinate";
      case SpotLight: return "SpotLight";
      case children: return "children";
      case translation: return "translation";
      case rotation: return "rotation";
      case appearance: return "appearance";
      case geometry: return "geometry";
      case size: return "size";
      case top: return "top";
      case bottom: return "bottom";
      case height: return "height";
      case radius: return "radius";
      case coord: return "coord";
      case coordIndex: return "coordIndex";
      case point: return "point";
      case creaseAngle: return "creaseAngle";
      case material: return "material";
      case texture: return "texture";
      case texCoord: return "texCoord";
      case texCoordIndex: return "texCoordIndex";
      case url: return "url";
      case emissiveColor: return "emissiveColor";
      case specularColor: return "specularColor";
      case diffuseColor: return "diffuseColor";
      case shininess: return "shininess";
      case on: return "on";
      case color: return "color";
      case location: return "location";
      case direction: return "direction";
      case attenuation: return "attenuation";
      case beamWidth: return "beamWidth";
      case cutOffAngle: return "cutOffAngle";
      case intensity: return "intensity";
      case ambientIntensity: return "ambientIntensity";
      case blockObjectOpen: return "{";
      case blockObjectClose: return "}";
      case blockArrayOpen: return "[";
      case blockArrayClose: return "]";
      case doubleQuote: return "\"";
      case separator: return ",";
    }//end switch
    return "unknown";
  }
  //end getJointName


  public void parse(String filePath) throws IOException
  {
    try
    {
      File file = new File(filePath);
      scanner = new Scanner(new FileReader(file));
      scanner.getNextChar();
      eat();
      parse();
    }
    catch (IOException e)
    {
      JOptionPane.showMessageDialog(null,
        "Error : Could not open " + filePath,
        "Error", JOptionPane.ERROR_MESSAGE);
      this.occuredError = true;
      return;
    }
  }


  public void parse() throws IOException
  {
    while
    (
      !this.occuredError &&
      (
        isToken(KeyWordID.DEF) ||
        isToken(KeyWordID.SpotLight) ||
        isToken(KeyWordID.Transform) ||
        isToken(KeyWordID.Shape)
      )
    )
    {
      if(isToken(KeyWordID.DEF))
      {
         ObjectsVRML.add(parseNamedObject());
      }

      if(isToken(KeyWordID.SpotLight))
      {
        SpotlightD spot = parseSpotlight();
        ObjectsVRML.add(spot);
        System.out.println("Spotlight found");
      }
      else if(isToken(KeyWordID.Transform))
      {
        TransformD transform = parseTransformObject();
        ObjectsVRML.add(transform);
        System.out.println("Transform found");
      }
      else if(isToken(KeyWordID.Shape))
      {
        ShapeD shape = parseShape();
        ObjectsVRML.add(shape);
        System.out.println("Shape found");
      }
      else
      {
        System.out.println("unknown token: " + scanner.buffer);
        eat();
      }
    }//end while
    System.out.println("parsed all elements");

  }//end parse

  private ObjectVRML parseNamedObject() throws IOException
  {
    eat();
    String name = parseString();
    ObjectVRML object = new ObjectVRML();
    if(isToken(KeyWordID.SpotLight))
    {
      System.out.println("Spotlight found");
      object = parseSpotlight(name);
      namedSpotlights.put(name, (SpotlightD) object);
    }
    else if(isToken(KeyWordID.Transform))
    {
      System.out.println("Transform '" + name + "' found");
      object = parseTransformObject(name);
      namedTransforms.put(name, (TransformD) object);
    }
    else if(isToken(KeyWordID.Shape))
    {
      object =  parseShape(name);
      namedShapes.put(name, (ShapeD) object);
      System.out.println("Shape '" + name + "' found");
    }
    else
    {
      System.out.println("unknown token: " + scanner.buffer);
      eat();
    }
    return object;
  }

  private SpotlightD parseSpotlight(String n) throws IOException
  {
    SpotlightD spot = parseSpotlight();
    spot.name = n;
    return spot;
  }

  private SpotlightD parseSpotlight() throws IOException
  {
    eat();
    SpotlightD spot = new SpotlightD();
    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.on))
        {
          eat();
          if(isToken(KeyWordID.TRUE))
          {
            spot.on = true;
          }
          eat();
        }
        else if(isToken(KeyWordID.intensity))
        {
          eat();
          spot.intensity = parseFloat();
        }
        else if(isToken(KeyWordID.ambientIntensity))
        {
          eat();
          spot.ambientIntensity = parseFloat();
        }
        else if(isToken(KeyWordID.color))
        {
          eat();
          spot.color = parseVec3f();
        }
        else if(isToken(KeyWordID.location))
        {
          eat();
          spot.location = parseVec3f();
        }
        else if(isToken(KeyWordID.direction))
        {
          eat();
          spot.direction = parseVec3f();
        }
        else if(isToken(KeyWordID.attenuation))
        {
          eat();
          spot.attenuation = parseVec3f();
        }
        else if(isToken(KeyWordID.radius))
        {
          eat();
          spot.radius = parseFloat();
        }
        else if(isToken(KeyWordID.beamWidth))
        {
          eat();
          spot.beamWidth = parseFloat();
        }
        else if(isToken(KeyWordID.cutOffAngle))
        {
          eat();
          spot.cutOffAngle = parseFloat();
        }
        else
        {
          System.out.println("unknown token: " + scanner.buffer);
          eat();
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: Spotlight -> no definition end found");
    }
    return spot;
  }

  private ShapeD parseShape(String n) throws IOException
  {
    ShapeD shape = parseShape();
    shape.name = n;
    return shape;
  }

  private ShapeD parseShape() throws IOException
  {
    eat();
    ShapeD shape = new ShapeD();
    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        {
          if(isToken(KeyWordID.appearance))
          {
            shape.appearance = parseAppearance();
          }
          if(isToken(KeyWordID.geometry))
          {
            shape.geometry = parseGeometry();
          }
          else
          {
            System.out.println("unknown token: " + scanner.buffer);
            eat();
          }
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: Spotlight -> no definition end found");
    }
    return shape;
  }

  private AppearanceD parseAppearance() throws IOException
  {
    eat();
    AppearanceD appearance = new AppearanceD();
    boolean hasName = false;

    if(isToken(KeyWordID.USE))
    {
      eat();
      String name =  parseString();
      appearance = namedAppearances.get(name);
    }
    else
    {
      if(isToken(KeyWordID.DEF))
      {
        eat();
        appearance.name =  parseString();
        hasName = true;
      }
      if(isToken(KeyWordID.Appearance))
      {
        eat();
      }

      if(isToken(KeyWordID.blockObjectOpen))
      {
        eat();
        while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
        {
          if(isToken(KeyWordID.material))
          {
            appearance.material = parseMaterial();
          }
          else if(isToken(KeyWordID.texture))
          {
            appearance.texture = parseTexture();
          }
          else
          {
            System.out.println("unknown token: " + scanner.buffer);
            eat();
          }
        }
      }

      if(isToken(KeyWordID.blockObjectClose))
      {
         eat();
      }
      else
      {
        System.out.println(scanner.buffer);
        System.out.println("Warning: parseAppearance -> no definition end found");
      }
      if(hasName)
      {
        namedAppearances.put(appearance.name, appearance);
      }
    }
    return appearance;
  }

  private MaterialD parseMaterial() throws IOException
  {
    eat();
    MaterialD material = new MaterialD();
    boolean hasName = false;

    if(isToken(KeyWordID.USE))
    {
      eat();
      String name = parseString();
      material = namedMaterials.get(name);
    }
    else
    {
      if(isToken(KeyWordID.DEF))
      {
        eat();
        material.name =  parseString();
        hasName = true;
      }
      if(isToken(KeyWordID.Material))
      {
        eat();
      }

      if(isToken(KeyWordID.blockObjectOpen))
      {
        eat();
        while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
        {
          if(isToken(KeyWordID.ambientIntensity))
          {
            eat();
            material.ambientIntensity = parseDouble();
          }
          else if(isToken(KeyWordID.diffuseColor))
          {
            eat();
            material.diffuseColor = parseVec3d();
          }
          else if(isToken(KeyWordID.shininess))
          {
            eat();
            material.shininess = parseDouble();
          }
          else if(isToken(KeyWordID.specularColor))
          {
            eat();
            material.specularColor = parseVec3d();
          }
          else if(isToken(KeyWordID.emissiveColor))
          {
            eat();
            material.emissiveColor = parseVec3d();
          }
          else
          {
            System.out.println("unknown token: " + scanner.buffer);
            eat();
          }
        }
      }

      if(isToken(KeyWordID.blockObjectClose))
      {
         eat();
      }
      else
      {
        System.out.println(scanner.buffer);
        System.out.println("Warning: parseMaterial -> no definition end found");
      }
      if(hasName)
      {
        namedMaterials.put(material.name, material);
      }
    }
    return material;
  }

  private Texture parseTexture() throws IOException
  {
    eat();
    Texture texture = new Texture();
    
    if(isToken(KeyWordID.ImageTexture))
    {
      eat();
    }

    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.url))
        {
          texture.url = parseUrl();
        }
        else
        {
          System.out.println("unknown token: " + scanner.buffer);
          eat();
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseTexture -> no definition end found");
    }
    return texture;
  }

  private String parseUrl() throws IOException
  {
    eat();
    String url = "";

    if(isToken(KeyWordID.doubleQuote))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.doubleQuote))
      {
          url += parseString();
      }
    }

    if(isToken(KeyWordID.doubleQuote))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseUrl -> no definition end found");
    }
    return url;
  }

  private GeometryD parseGeometry() throws IOException
  {
    eat();
    GeometryD geometry = new GeometryD();
    boolean hasName = false;
    String name = "";

    if(isToken(KeyWordID.USE))
    {
      eat();
      name =  parseString();
      geometry = namedGeometries.get(name);
    }
    else
    {
      if(isToken(KeyWordID.DEF))
      {
        eat();
        name =  parseString();
        hasName = true;
      }
      if(isToken(KeyWordID.Box))
      {
        geometry = parseBox(name);
      }
      else if(isToken(KeyWordID.Sphere))
      {
        geometry = parseSphere(name);
      }
      else if(isToken(KeyWordID.Cylinder))
      {
        geometry = parseCylinder(name);
      }
      else if(isToken(KeyWordID.IndexedFaceSet))
      {
        geometry = parseIndexedFaceSet(name);
      }
      else if(isToken(KeyWordID.IndexedLineSet))
      {
        geometry = parseIndexedLineSet(name);
      }
      else
      {
        System.out.println("unknown token: " + scanner.buffer);
        eat();
      }
      if(hasName)
      {
        namedGeometries.put(name, geometry);
      }
    }
    return geometry;
  }

  private BoxD parseBox(String n) throws IOException
  {
    eat();
    BoxD box = new BoxD();
    box.name = n;

    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.size))
        {
          eat();
          box.size = parseVec3d();
        }
        else
        {
          System.out.println("unknown token: " + scanner.buffer);
          eat();
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseBox -> no definition end found");
    }
    return box;
  }

  private SphereD parseSphere(String n) throws IOException
  {
    eat();
    SphereD sphere = new SphereD();
    sphere.name = n;

    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.radius))
        {
          eat();
          sphere.radius = parseDouble();
        }
        else
        {
          System.out.println("unknown token: " + scanner.buffer);
          eat();
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseSphere -> no definition end found");
    }
    return sphere;
  }

  private CylinderD parseCylinder(String n) throws IOException
  {
    eat();
    CylinderD cylinder = new CylinderD();
    cylinder.name = n;

    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.radius))
        {
          eat();
          cylinder.radius = parseDouble();
        }
        else if(isToken(KeyWordID.height))
        {
          eat();
          cylinder.height = parseDouble();
        }
        else if(isToken(KeyWordID.top))
        {
          eat();
          if(isToken(KeyWordID.FALSE))
          {
            eat();
            cylinder.top = false;
          }
        }
        else if(isToken(KeyWordID.bottom))
        {
          eat();
          if(isToken(KeyWordID.FALSE))
          {
            eat();
            cylinder.bottom = false;
          }
        }
        else
        {
          System.out.println("unknown token: " + scanner.buffer);
          eat();
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseCylinder -> no definition end found");
    }
    return cylinder;
  }

  private IndexedFaceSetD parseIndexedFaceSet(String n) throws IOException
  {
    eat();
    IndexedFaceSetD indexedFaceSet = new IndexedFaceSetD();
    indexedFaceSet.name = n;

    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.coord))
        {
          indexedFaceSet.coords = parseCoordinates();
        }
        else if(isToken(KeyWordID.coordIndex))
        {
          indexedFaceSet.coordIndexes = parseIndexList();
        }
        else if(isToken(KeyWordID.texCoord))
        {
          indexedFaceSet.texCoords = parseTexCoordinates();
        }
        else if(isToken(KeyWordID.texCoordIndex))
        {
          indexedFaceSet.texCoordIndexes = parseIndexList();
        }
        else if(isToken(KeyWordID.creaseAngle))
        {
          eat();
          indexedFaceSet.creaseAngle = parseDouble();
        }
        else
        {
          System.out.println("unknown token: " + scanner.buffer);
          eat();
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseIndexedFaceSet -> no definition end found");
    }
    return indexedFaceSet;
  }

  private IndexedLineSetD parseIndexedLineSet(String n) throws IOException
  {
    eat();
    IndexedLineSetD indexedLineSet = new IndexedLineSetD();
    indexedLineSet.name = n;

    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.coord))
        {
          indexedLineSet.coords = parseCoordinates();
        }
        else if(isToken(KeyWordID.coordIndex))
        {
          indexedLineSet.coordIndexes = parseIndexList();
        }
        else if(isToken(KeyWordID.creaseAngle))
        {
          eat();
          indexedLineSet.creaseAngle = parseDouble();
        }
        else
        {
          System.out.println("unknown token: " + scanner.buffer);
          eat();
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseIndexedLineSet -> no definition end found");
    }
    return indexedLineSet;
  }

  private  ArrayList<Vec3d> parseCoordinates() throws IOException
  {
    ArrayList<Vec3d> pointList = new ArrayList<Vec3d>();
    boolean hasName = false;
    String name = "";
    eat();

    if(isToken(KeyWordID.USE))
    {
      eat();
      name = parseString();
      pointList = namedCoordinates.get(name);
    }
    else
    {
      if(isToken(KeyWordID.DEF))
      {
        eat();
        name = parseString();
        hasName = true;
      }
      if(isToken(KeyWordID.Coordinate))
      {
        eat();
      }
      if(isToken(KeyWordID.blockObjectOpen))
      {
        eat();
        while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
        {
          if(isToken(KeyWordID.point))
          {
            pointList = parseVec3dList();
          }
          else
          {
            System.out.println("unknown token: " + scanner.buffer);
            eat();
          }
        }
      }

      if(isToken(KeyWordID.blockObjectClose))
      {
         eat();
      }
      else
      {
        System.out.println(scanner.buffer);
        System.out.println("Warning: parseCoordinates -> no definition end found");
      }
    }
    if(hasName)
    {
      namedCoordinates.put(name, pointList);
    }
    return pointList;
  }

  private  ArrayList<Vec3d> parseTexCoordinates() throws IOException
  {
    ArrayList<Vec3d> pointList = new ArrayList<Vec3d>();
    boolean hasName = false;
    String name = "";
    eat();

    if(isToken(KeyWordID.USE))
    {
      eat();
      name = parseString();
      pointList = namedTexCoordinates.get(name);
    }
    else
    {
      if(isToken(KeyWordID.DEF))
      {
        eat();
        name = parseString();
        hasName = true;
      }
      if(isToken(KeyWordID.TextureCoordinate))
      {
        eat();
      }
      if(isToken(KeyWordID.blockObjectOpen))
      {
        eat();
        while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
        {
          if(isToken(KeyWordID.point))
          {
            pointList = parseVec3dList();
          }
          else
          {
            System.out.println("unknown token: " + scanner.buffer);
            eat();
          }
        }
      }

      if(isToken(KeyWordID.blockObjectClose))
      {
         eat();
      }
      else
      {
        System.out.println(scanner.buffer);
        System.out.println("Warning: parseTexCoordinates -> no definition end found");
      }
    }
    if(hasName)
    {
      namedTexCoordinates.put(name, pointList);
    }
    return pointList;
  }

  private  ArrayList<Vec3d> parseVec3dList() throws IOException
  {
    ArrayList<Vec3d> pointList = new ArrayList<Vec3d>();
    eat();

    if(isToken(KeyWordID.blockArrayOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockArrayClose))
      {
        Vec3d point = new Vec3d(0);
        point.x = parseDouble();
        point.y = parseDouble();
        point.z = parseDouble();
        pointList.add(point);
      }
    }

    if(isToken(KeyWordID.blockArrayClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseVec3dList -> no definition end found");
    }
    return pointList;
  }

  private  ArrayList<Integer> parseIndexList() throws IOException
  {
    ArrayList<Integer> indexList = new ArrayList<Integer>();
    eat();

    if(isToken(KeyWordID.blockArrayOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockArrayClose))
      {
        if(isToken(KeyWordID.separator))
        {
          eat();
        }
        else
        {
          int index = parseInt();
          indexList.add(index);
        }
      }
    }

    if(isToken(KeyWordID.blockArrayClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseIndexList -> no definition end found");
    }
    return indexList;
  }

  private TransformD parseTransformObject(String n) throws IOException
  {
    TransformD transform = parseTransformObject();
    transform.name = n;
    return transform;
  }

  private TransformD parseTransformObject() throws IOException
  {
    eat();
    TransformD transform = new TransformD();
    if(isToken(KeyWordID.blockObjectOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockObjectClose))
      {
        if(isToken(KeyWordID.translation))
        {
          eat();
          transform.translation.x = parseDouble();
          transform.translation.z = parseDouble();
          transform.translation.y = parseDouble();
        }
        if(isToken(KeyWordID.rotation))
        {
          eat();
          transform.rotation.x = parseDouble();
          transform.rotation.z = parseDouble();
          transform.rotation.y = parseDouble();
          transform.angle = parseDouble();
        }
        if(isToken(KeyWordID.children))
        {
          transform = parseChildren(transform);
        }
      }
    }

    if(isToken(KeyWordID.blockObjectClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseTransformObject -> no definition end found");
    }
    return transform;
  }

  public TransformD parseChildren(TransformD transform) throws IOException
  {
    eat();
    if(isToken(KeyWordID.blockArrayOpen))
    {
      eat();
      while (!this.occuredError && !isToken(KeyWordID.blockArrayClose))
      {
        if(isToken(KeyWordID.DEF))
        {
          eat();
          transform.children.add(parseNamedObject());
        }
        else if(isToken(KeyWordID.Transform))
        {
          transform.children.add(parseTransformObject());
          System.out.println("Transform found");
        }
        else if(isToken(KeyWordID.Shape))
        {
          transform.children.add(parseShape());
          System.out.println("Shape found");
        }
        else if(isToken(KeyWordID.SpotLight))
        {
          transform.children.add(parseSpotlight());
          System.out.println("Spotlight found");
        }
      }
    }

    if(isToken(KeyWordID.blockArrayClose))
    {
       eat();
    }
    else
    {
      System.out.println(scanner.buffer);
      System.out.println("Warning: parseChildren -> no definition end found");
    }
    return transform;
  }

  //parse an string
  public String parseString() throws IOException
  {
    String result = scanner.buffer;
    eat();
    return result;
  }//end parseInt

  //parse an integer
  public int parseInt() throws IOException
  {
    int result = Integer.parseInt(scanner.buffer);
    eat();
    return result;
  }//end parseInt

  // parse a float
  public float parseFloat() throws IOException
  {
    float result = (float) Float.parseFloat(scanner.buffer);
    eat();
    return result;
  }//end parseFloat

  // parse a double
  public double parseDouble() throws IOException
  {
    double result = (double) Double.parseDouble(scanner.buffer);
    eat();
    return result;
  }//end parseFloat

  // parse a double
  public Vec3f parseVec3f() throws IOException
  {
    Vec3f result = new Vec3f(0);
    result.x = (float) Float.parseFloat(scanner.buffer);
    eat();
    result.y = (float) Float.parseFloat(scanner.buffer);
    eat();
    result.z = (float) Float.parseFloat(scanner.buffer);
    eat();
    return result;
  }//end parseFloat

  // parse a double
  public Vec3d parseVec3d() throws IOException
  {
    Vec3d result = new Vec3d(0);
    result.x = (double) Double.parseDouble(scanner.buffer);
    eat();
    result.y = (double) Double.parseDouble(scanner.buffer);
    eat();
    result.z = (double) Double.parseDouble(scanner.buffer);
    eat();
    return result;
  }//end parseFloat


  
//  public void parseMotionNet() throws IOException
//  {
//    parseJointIndex();
//
//    while(!this.occuredError && isToken(KeyWordID.keyFrame) )
//    {
//      parseKeyFrame();
//    }//end while
//
//    while(!this.occuredError && isToken(KeyWordID.transition) )
//    {
//      parseTransition();
//    }//end while
//
//  }//end parseMotionNet



//  public void parseJointIndex() throws IOException
//  {
//    while(!this.occuredError && !isToken(KeyWordID.transition) && !isToken(KeyWordID.keyFrame))
//    {
//      //aktuelle Joint ID
//      JointID id = jointIDFromName(scanner.buffer);
//
//      eat(); // eat the joint name
//      isTokenAndEat(KeyWordID.separator); // ";"
//
//      if(id == JointID.numOfJoint)
//      {
//        JOptionPane.showMessageDialog(null,
//          "Error : " + getJointName(id) + " => next token: " + scanner.buffer,
//          "Error", JOptionPane.ERROR_MESSAGE);
//        this.occuredError = true;
//        return;
//      }
//      else
//      {
//        if(this.numberOfJoints < JointID.values().length - 1)//motionNet.numberOfJoints < numberOfJoints)
//        {
//          //System.out.println(getJointName(id));
//          this.Joints[this.numberOfJoints++] = getJointName(id);
//        }
//        else
//        {
//          JOptionPane.showMessageDialog(null,
//            "Error : To many joint indices detected",
//            "Error", JOptionPane.ERROR_MESSAGE);
//          this.occuredError = true;
//          return;
//        }
//      //obsolete  this.Joints[this.numberOfJoints] = getJointName(JointID.numOfJoint);
//      }
//    }//end while
//  }//end parseJointIndex
//
//  public void parseKeyFrame() throws IOException
//  {
//      float[] Keys = new float[numberOfJoints];//numberOfJoints
//
//      isTokenAndEat(KeyWordID.keyFrame); // keyword "keyframe"
//      isTokenAndEat(KeyWordID.separator); // ";"
//      int id = parseInt(); // id
//
//      // skip the coordinates
//      isTokenAndEat(KeyWordID.separator); // ";"
//      int x = parseInt(); // x
//      isTokenAndEat(KeyWordID.separator); // ";"
//      int y = parseInt(); // y
//
//      if(this.LastKeyFrame != null)
//      {
//        this.KeyFrameNodeX += 100 * this.PaintDirection;
//        if(this.KeyFrameNodeX >= 500 || this.KeyFrameNodeX <= 0)
//        {
//          this.PaintDirection *= -1;
//          this.KeyFrameNodeX += 100 * this.PaintDirection;
//          //this.KeyFrameNodeX = 0;
//          this.KeyFrameNodeY += 100;
//        }
//        if(x <= 0 || y <= 0)
//        {
//          x = this.KeyFrameNodeX;
//          y = this.KeyFrameNodeY;
//        }
//      }
//
//      // parse joint values
//      int i = 0;
//      KeyFrame Frame = new KeyFrame(this.JointDefaultConfiguration, id, x, y);
//
//      while( !isToken(KeyWordID.keyFrame) && isToken(KeyWordID.separator))
//      {
//        isTokenAndEat(KeyWordID.separator); // ";"
//        if(i < this.Joints.length)//numberOfJoints)
//        {
//          Keys[i] = Math.round(parseFloat() * 100) / 100;
//          Frame.setJointValue(this.Joints[i], Keys[i]);
//        }
//        else
//        {
//          JOptionPane.showMessageDialog(null,
//            "Error : To much keys in frame " + id + " detected",
//            "Error", JOptionPane.ERROR_MESSAGE);
//          this.occuredError = true;
//          return;
//        }
//         i++;
//      }
//      //end while
//      this.motionNet.addKeyFrame(Frame);
//      this.LastKeyFrame = Frame;
//  }
//  //end parseKeyFrame
//
//
//  public void parseTransition() throws IOException
//  {
//      // transition;id_from;id_to;duration;condition
//
//      isTokenAndEat(KeyWordID.transition); // keyword "transition"
//      isTokenAndEat(KeyWordID.separator); // ";"
//      int fromFrame = parseInt(); // id from
//      KeyFrame fromKeyFrame = this.motionNet.getKeyFrame(fromFrame);
//      if(fromKeyFrame == null)
//      {
//        JOptionPane.showMessageDialog(null,
//          "Error : Transition from unknown keyframe " + fromFrame + " detected",
//          "Error", JOptionPane.ERROR_MESSAGE);
//          this.occuredError = true;
//        return;
//      }
//      isTokenAndEat(KeyWordID.separator); // ";"
//      int toFrame = parseInt(); // id to
//      KeyFrame toKeyFrame = this.motionNet.getKeyFrame(toFrame);
//      if(fromKeyFrame == null)
//      {
//        JOptionPane.showMessageDialog(null,
//          "Error : Transition to unknown keyframe " + toFrame + " detected",
//          "Error", JOptionPane.ERROR_MESSAGE);
//          this.occuredError = true;
//        return;
//      }
//      isTokenAndEat(KeyWordID.separator); // ";"
//      float duration = parseFloat(); // duration
//
//      isTokenAndEat(KeyWordID.separator); // ";"
//
//      // get the condition and eat
//      String condition = scanner.buffer;
//      eat();
//
//      Transition transition = new Transition(condition, duration);
//      this.motionNet.addTransition(transition, fromKeyFrame, toKeyFrame);
//  }//end parseTransition

  // pars a integer (something like "- 20" is not allowed, correct would be "-20")



}
