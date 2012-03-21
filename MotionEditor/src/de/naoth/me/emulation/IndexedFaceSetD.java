/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package de.naoth.me.emulation;
import js.math.vector.Vec3d;
import java.util.ArrayList;

/**
 *
 * @author claas
 */
public class IndexedFaceSetD extends GeometryD
{
  public ArrayList<Vec3d> texCoords;
  public ArrayList<Integer> texCoordIndexes;
  public ArrayList<Vec3d> coords;
  public ArrayList<Integer> coordIndexes;

  public IndexedFaceSetD()
  {
    texCoords = new ArrayList<Vec3d>();
    texCoordIndexes = new ArrayList<Integer>();
    coords = new ArrayList<Vec3d>();
    coordIndexes = new ArrayList<Integer>();
  }

  public IndexedFaceSetD(String n)
  {
    texCoords = new ArrayList<Vec3d>();
    texCoordIndexes = new ArrayList<Integer>();
    coords = new ArrayList<Vec3d>();
    coordIndexes = new ArrayList<Integer>();
    name = n;
  }
}
